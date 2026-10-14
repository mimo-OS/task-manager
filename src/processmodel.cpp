/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - Process Model Implementation
 */

#include "processmodel.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QIcon>
#include <QDebug>
#include <algorithm>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <signal.h>
#include <pwd.h>
#include <dirent.h>

ProcessModel::ProcessModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_timer(new QTimer(this))
{
    m_currentUserUid = getuid();
    m_pageSize = sysconf(_SC_PAGESIZE);
    m_clockTicksPerSec = sysconf(_SC_CLK_TCK);
    m_cpuCores = std::max(1, (int)sysconf(_SC_NPROCESSORS_ONLN));

    // Determine total RAM
    QFile meminfo("/proc/meminfo");
    if (meminfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&meminfo);
        QString line;
        while (in.readLineInto(&line)) {
            if (line.startsWith("MemTotal:")) {
                QStringList parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
                if (parts.size() >= 2) {
                    m_totalRamBytes = parts[1].toULongLong() * 1024;
                }
                break;
            }
        }
        meminfo.close();
    }
    if (m_totalRamBytes == 0) {
        m_totalRamBytes = 8ULL * 1024 * 1024 * 1024; // Fallback 8GB
    }

    m_prevTotalCpuTime = readTotalCpuTime();

    connect(m_timer, &QTimer::timeout, this, &ProcessModel::updateProcesses);
    m_timer->start(m_refreshInterval);

    // Initial population
    updateProcesses();
}

ProcessModel::~ProcessModel()
{
    m_timer->stop();
}

int ProcessModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_filteredProcesses.size();
}

int ProcessModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColCount;
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_filteredProcesses.size())
        return QVariant();

    const ProcessInfo &proc = m_filteredProcesses.at(index.row());

    switch (role) {
    case PidRole:
        return proc.pid;
    case PpidRole:
        return proc.ppid;
    case NameRole:
        return proc.name;
    case CmdlineRole:
        return proc.cmdline.isEmpty() ? proc.name : proc.cmdline;
    case UserRole:
        return proc.user;
    case UidRole:
        return proc.uid;
    case CpuPercentRole:
        return proc.cpuPercent;
    case CpuPercentFormattedRole:
        return QString::asprintf("%.1f%%", proc.cpuPercent);
    case MemoryBytesRole:
        return (qulonglong)proc.memoryBytes;
    case MemoryFormattedRole:
        return formatBytes(proc.memoryBytes);
    case MemoryPercentRole:
        return proc.memoryPercent;
    case VirtualMemoryBytesRole:
        return (qulonglong)proc.virtualMemoryBytes;
    case VirtualMemoryFormattedRole:
        return formatBytes(proc.virtualMemoryBytes);
    case StatusRole:
        return proc.status;
    case PriorityRole:
        return proc.priority;
    case ThreadsRole:
        return proc.threads;
    case IoFormattedRole:
        return formatRate(proc.readBytesPerSec + proc.writeBytesPerSec);
    case IconNameRole:
        return proc.iconName;

    // DisplayRole for generic TableView
    case Qt::DisplayRole: {
        switch (index.column()) {
        case ColName:
            return proc.name;
        case ColUser:
            return proc.user;
        case ColCpu:
            return QString::asprintf("%.1f%%", proc.cpuPercent);
        case ColMemory:
            return formatBytes(proc.memoryBytes);
        case ColDisk:
            return formatRate(proc.readBytesPerSec + proc.writeBytesPerSec);
        case ColPid:
            return proc.pid;
        case ColPriority:
            return proc.priority;
        default:
            return QVariant();
        }
    }
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ProcessModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PidRole] = "pid";
    roles[PpidRole] = "ppid";
    roles[NameRole] = "name";
    roles[CmdlineRole] = "cmdline";
    roles[UserRole] = "user";
    roles[UidRole] = "uid";
    roles[CpuPercentRole] = "cpuPercent";
    roles[CpuPercentFormattedRole] = "cpuPercentFormatted";
    roles[MemoryBytesRole] = "memoryBytes";
    roles[MemoryFormattedRole] = "memoryFormatted";
    roles[MemoryPercentRole] = "memoryPercent";
    roles[VirtualMemoryBytesRole] = "virtualMemoryBytes";
    roles[VirtualMemoryFormattedRole] = "virtualMemoryFormatted";
    roles[StatusRole] = "status";
    roles[PriorityRole] = "priority";
    roles[ThreadsRole] = "threads";
    roles[IoFormattedRole] = "ioFormatted";
    roles[IconNameRole] = "iconName";
    return roles;
}

void ProcessModel::setSearchFilter(const QString &filter)
{
    if (m_searchFilter == filter)
        return;

    m_searchFilter = filter;
    emit searchFilterChanged();
    applyFilterAndSort();
}

void ProcessModel::setFilterType(int type)
{
    if (m_filterType == type)
        return;

    m_filterType = type;
    emit filterTypeChanged();
    applyFilterAndSort();
}

void ProcessModel::setSortColumn(int column)
{
    if (m_sortColumn == column)
        return;

    m_sortColumn = column;
    emit sortColumnChanged();
    applyFilterAndSort();
}

void ProcessModel::setSortOrder(Qt::SortOrder order)
{
    if (m_sortOrder == order)
        return;

    m_sortOrder = order;
    emit sortOrderChanged();
    applyFilterAndSort();
}

void ProcessModel::sortByColumn(int column)
{
    if (m_sortColumn == column) {
        setSortOrder(m_sortOrder == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder);
    } else {
        setSortColumn(column);
        // Default descending for CPU and Memory, ascending for Name
        if (column == ColName || column == ColUser) {
            setSortOrder(Qt::AscendingOrder);
        } else {
            setSortOrder(Qt::DescendingOrder);
        }
    }
}

void ProcessModel::setRefreshInterval(int msec)
{
    if (msec < 500) msec = 500;
    if (m_refreshInterval == msec)
        return;

    m_refreshInterval = msec;
    m_timer->setInterval(m_refreshInterval);
    emit refreshIntervalChanged();
}

void ProcessModel::refresh()
{
    updateProcesses();
}

bool ProcessModel::killProcess(int pid, int sig)
{
    if (pid <= 0) return false;

    int ret = ::kill(pid, sig);
    bool success = (ret == 0);
    QString msg = success ? QString("Signal %1 sent to PID %2").arg(sig).arg(pid)
                          : QString("Failed to send signal to PID %2").arg(pid);

    emit processKilled(pid, success, msg);
    if (success) {
        // Quick update to reflect terminated state
        QTimer::singleShot(250, this, &ProcessModel::updateProcesses);
    }
    return success;
}

bool ProcessModel::setProcessPriority(int pid, int niceValue)
{
    if (pid <= 0) return false;
    if (niceValue < -20) niceValue = -20;
    if (niceValue > 19) niceValue = 19;

    int ret = ::setpriority(PRIO_PROCESS, pid, niceValue);
    bool success = (ret == 0);
    if (success) {
        QTimer::singleShot(100, this, &ProcessModel::updateProcesses);
    }
    return success;
}

QVariantMap ProcessModel::getProcessDetails(int pid) const
{
    QVariantMap map;
    if (!m_allProcesses.contains(pid))
        return map;

    const ProcessInfo &p = m_allProcesses.value(pid);
    map["pid"] = p.pid;
    map["ppid"] = p.ppid;
    map["name"] = p.name;
    map["cmdline"] = p.cmdline.isEmpty() ? p.name : p.cmdline;
    map["user"] = p.user;
    map["uid"] = p.uid;
    map["cpuPercent"] = QString::asprintf("%.1f%%", p.cpuPercent);
    map["memoryFormatted"] = formatBytes(p.memoryBytes);
    map["memoryPercent"] = QString::asprintf("%.1f%%", p.memoryPercent);
    map["virtualMemoryFormatted"] = formatBytes(p.virtualMemoryBytes);
    map["status"] = p.status;
    map["priority"] = p.priority;
    map["threads"] = p.threads;
    map["ioFormatted"] = formatRate(p.readBytesPerSec + p.writeBytesPerSec);
    map["iconName"] = p.iconName;

    return map;
}

qulonglong ProcessModel::readTotalCpuTime()
{
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0;

    QByteArray firstLine = file.readLine();
    file.close();

    const char *ptr = firstLine.constData();
    if (strncmp(ptr, "cpu ", 4) != 0)
        return 0;

    ptr += 4;
    qulonglong total = 0;
    while (*ptr) {
        while (*ptr == ' ') ptr++;
        if (!*ptr || *ptr == '\n') break;
        char *end = nullptr;
        unsigned long long val = strtoull(ptr, &end, 10);
        if (end == ptr) break;
        total += val;
        ptr = end;
    }
    return total;
}

QString ProcessModel::resolveUsername(uint uid)
{
    if (m_userCache.contains(uid))
        return m_userCache.value(uid);

    struct passwd *pw = getpwuid(uid);
    QString name = pw ? QString::fromUtf8(pw->pw_name) : QString::number(uid);
    m_userCache.insert(uid, name);
    return name;
}

QString ProcessModel::resolveAppIcon(const QString &name, const QString &cmdline) const
{
    QString n = name.toLower();
    QString cmd = cmdline.toLower();
    QString candidate;
    if (n.contains("firefox")) candidate = "firefox";
    else if (n.contains("chrome")) candidate = "google-chrome";
    else if (n.contains("code") || cmd.contains("code")) candidate = "visual-studio-code";
    else if (n.contains("terminal") || n.contains("bash") || n.contains("zsh") || n.contains("fish")) candidate = "utilities-terminal";
    else if (n.contains("calc")) candidate = "cutefish-calculator";
    else if (n.contains("file") || n.contains("nautilus") || n.contains("dolphin")) candidate = "system-file-manager";
    else if (n.contains("settings") || n.contains("control")) candidate = "preferences-system";
    else if (n.contains("monitor") || n.contains("task")) candidate = "cutefish-taskmanager";
    else if (n.contains("python")) candidate = "python";
    else if (n.contains("steam")) candidate = "steam";
    else if (n.contains("spotify")) candidate = "spotify";
    else if (n.contains("telegram")) candidate = "telegram";
    else if (n.contains("discord")) candidate = "discord";
    else if (n.contains("mpv") || n.contains("vlc")) candidate = "vlc";
    else if (n.contains("kwin") || n.contains("systemd") || n.contains("dbus") || n.contains("pipewire") || n.contains("wireplumber") || n.contains("xorg") || n.contains("wayland"))
        return QStringLiteral("process-system");

    if (!candidate.isEmpty() && QIcon::hasThemeIcon(candidate)) {
        return candidate;
    }

    return QStringLiteral("process-default");
}

QString ProcessModel::formatBytes(qulonglong bytes)
{
    if (bytes >= 1024ULL * 1024 * 1024) {
        return QString::asprintf("%.2f GiB", bytes / (1024.0 * 1024.0 * 1024.0));
    } else if (bytes >= 1024ULL * 1024) {
        return QString::asprintf("%.1f MiB", bytes / (1024.0 * 1024.0));
    } else if (bytes >= 1024ULL) {
        return QString::asprintf("%.1f KiB", bytes / 1024.0);
    } else {
        return QString("%1 B").arg(bytes);
    }
}

QString ProcessModel::formatRate(qulonglong bytesPerSec)
{
    if (bytesPerSec >= 1024ULL * 1024) {
        return QString::asprintf("%.1f MB/s", bytesPerSec / (1024.0 * 1024.0));
    } else if (bytesPerSec >= 1024ULL) {
        return QString::asprintf("%.1f KB/s", bytesPerSec / 1024.0);
    } else if (bytesPerSec > 0) {
        return QString("%1 B/s").arg(bytesPerSec);
    } else {
        return "0 KB/s";
    }
}

void ProcessModel::updateProcesses()
{
    qulonglong currentTotalCpu = readTotalCpuTime();
    qulonglong totalCpuDelta = (currentTotalCpu > m_prevTotalCpuTime) ? (currentTotalCpu - m_prevTotalCpuTime) : 1;
    m_prevTotalCpuTime = currentTotalCpu;

    DIR *dir = opendir("/proc");
    if (!dir) return;

    QSet<int> foundPids;
    struct dirent *ent = nullptr;

    while ((ent = readdir(dir)) != nullptr) {
        if (ent->d_type != DT_DIR)
            continue;

        // Check if directory name is numeric (PID)
        char *end = nullptr;
        long pidLong = strtol(ent->d_name, &end, 10);
        if (*end != '\0' || pidLong <= 0)
            continue;

        int pid = (int)pidLong;
        foundPids.insert(pid);

        // Read /proc/[pid]/stat
        char statPath[64];
        snprintf(statPath, sizeof(statPath), "/proc/%d/stat", pid);
        FILE *fStat = fopen(statPath, "r");
        if (!fStat) continue;

        char statBuf[1024];
        size_t bytesRead = fread(statBuf, 1, sizeof(statBuf) - 1, fStat);
        fclose(fStat);
        if (bytesRead == 0) continue;
        statBuf[bytesRead] = '\0';

        // Extract comm inside parentheses: pid (comm) state ppid ...
        char *openParen = strchr(statBuf, '(');
        char *closeParen = strrchr(statBuf, ')');
        if (!openParen || !closeParen || closeParen < openParen)
            continue;

        *closeParen = '\0';
        QString name = QString::fromUtf8(openParen + 1);
        char *afterComm = closeParen + 2; // skip space

        char stateChar = 'S';
        int ppid = 0;
        int pgrp = 0, session = 0, tty_nr = 0, tpgid = 0;
        unsigned int flags = 0;
        unsigned long minflt = 0, cminflt = 0, majflt = 0, cmajflt = 0;
        unsigned long utime = 0, stime = 0;
        long cutime = 0, cstime = 0, priority = 0, nice = 0, num_threads = 1;
        unsigned long itrealvalue = 0, starttime = 0, vsize = 0;
        long rss = 0;

        int fieldsParsed = sscanf(afterComm,
            "%c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %lu %lu %lu %ld",
            &stateChar, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags,
            &minflt, &cminflt, &majflt, &cmajflt,
            &utime, &stime, &cutime, &cstime, &priority, &nice, &num_threads,
            &itrealvalue, &starttime, &vsize, &rss);

        if (fieldsParsed < 10)
            continue;

        ProcessInfo proc = m_allProcesses.value(pid);
        proc.pid = pid;
        proc.ppid = ppid;
        proc.name = name;
        proc.priority = nice;
        proc.threads = (num_threads > 0) ? (int)num_threads : 1;
        proc.virtualMemoryBytes = (qulonglong)vsize;
        proc.memoryBytes = (rss > 0) ? ((qulonglong)rss * m_pageSize) : 0;
        proc.memoryPercent = (m_totalRamBytes > 0) ? ((double)proc.memoryBytes / (double)m_totalRamBytes * 100.0) : 0.0;
        proc.startTime = starttime;

        // Status string
        switch (stateChar) {
        case 'R': proc.status = QStringLiteral("Running"); break;
        case 'S': proc.status = QStringLiteral("Sleeping"); break;
        case 'D': proc.status = QStringLiteral("Disk Sleep"); break;
        case 'Z': proc.status = QStringLiteral("Zombie"); break;
        case 'T': case 't': proc.status = QStringLiteral("Stopped"); break;
        case 'I': proc.status = QStringLiteral("Idle"); break;
        default: proc.status = QString(stateChar); break;
        }

        // Calculate CPU usage
        qulonglong currentProcessCpu = utime + stime;
        if (proc.prevUtime > 0 || proc.prevStime > 0) {
            qulonglong procDelta = (currentProcessCpu >= (proc.prevUtime + proc.prevStime))
                                   ? (currentProcessCpu - (proc.prevUtime + proc.prevStime)) : 0;
            double cpu = ((double)procDelta / (double)totalCpuDelta) * 100.0;
            // Cap to reasonable percentage
            proc.cpuPercent = std::clamp(cpu, 0.0, (double)(m_cpuCores * 100.0));
        } else {
            proc.cpuPercent = 0.0;
        }
        proc.prevUtime = utime;
        proc.prevStime = stime;

        // Read status for UID if not cached
        if (proc.user.isEmpty()) {
            char statusPath[64];
            snprintf(statusPath, sizeof(statusPath), "/proc/%d/status", pid);
            FILE *fStatus = fopen(statusPath, "r");
            if (fStatus) {
                char line[256];
                while (fgets(line, sizeof(line), fStatus)) {
                    if (strncmp(line, "Uid:", 4) == 0) {
                        uint realUid = 0;
                        sscanf(line + 4, "%u", &realUid);
                        proc.uid = realUid;
                        proc.user = resolveUsername(realUid);
                        break;
                    }
                }
                fclose(fStatus);
            }
        }

        // Read /proc/[pid]/cmdline once
        if (proc.cmdline.isEmpty()) {
            char cmdlinePath[64];
            snprintf(cmdlinePath, sizeof(cmdlinePath), "/proc/%d/cmdline", pid);
            FILE *fCmd = fopen(cmdlinePath, "r");
            if (fCmd) {
                char cmdBuf[1024];
                size_t cmdBytes = fread(cmdBuf, 1, sizeof(cmdBuf) - 1, fCmd);
                fclose(fCmd);
                if (cmdBytes > 0) {
                    // Replace null terminators with space
                    for (size_t i = 0; i < cmdBytes - 1; ++i) {
                        if (cmdBuf[i] == '\0') cmdBuf[i] = ' ';
                    }
                    cmdBuf[cmdBytes] = '\0';
                    proc.cmdline = QString::fromUtf8(cmdBuf).trimmed();
                }
            }
        }

        // Resolve icon
        if (proc.iconName.isEmpty()) {
            proc.iconName = resolveAppIcon(proc.name, proc.cmdline);
        }

        m_allProcesses.insert(pid, proc);
    }
    closedir(dir);

    // Remove dead processes
    QList<int> existingPids = m_allProcesses.keys();
    for (int p : existingPids) {
        if (!foundPids.contains(p)) {
            m_allProcesses.remove(p);
        }
    }

    applyFilterAndSort();
}

void ProcessModel::applyFilterAndSort()
{
    QVector<ProcessInfo> filtered;
    filtered.reserve(m_allProcesses.size());

    QString search = m_searchFilter.trimmed();
    bool hasSearch = !search.isEmpty();

    for (auto it = m_allProcesses.constBegin(); it != m_allProcesses.constEnd(); ++it) {
        const ProcessInfo &p = it.value();

        // Filter type
        if (m_filterType == FilterMine && p.uid != m_currentUserUid)
            continue;
        if (m_filterType == FilterActive && p.cpuPercent <= 0.0 && p.status != QLatin1String("Running"))
            continue;

        // Search text
        if (hasSearch) {
            bool matches = p.name.contains(search, Qt::CaseInsensitive)
                        || QString::number(p.pid).contains(search)
                        || p.user.contains(search, Qt::CaseInsensitive)
                        || p.cmdline.contains(search, Qt::CaseInsensitive);
            if (!matches) continue;
        }

        filtered.append(p);
    }

    // Sort with strict weak ordering
    auto comparator = [this](const ProcessInfo &a, const ProcessInfo &b) -> bool {
        if (m_sortOrder == Qt::AscendingOrder) {
            switch (m_sortColumn) {
            case ColName:
                return a.name.localeAwareCompare(b.name) < 0;
            case ColUser:
                return a.user.localeAwareCompare(b.user) < 0;
            case ColCpu:
                return a.cpuPercent < b.cpuPercent;
            case ColMemory:
                return a.memoryBytes < b.memoryBytes;
            case ColDisk:
                return (a.readBytesPerSec + a.writeBytesPerSec) < (b.readBytesPerSec + b.writeBytesPerSec);
            case ColPid:
                return a.pid < b.pid;
            case ColPriority:
                return a.priority < b.priority;
            default:
                return a.cpuPercent < b.cpuPercent;
            }
        } else {
            switch (m_sortColumn) {
            case ColName:
                return a.name.localeAwareCompare(b.name) > 0;
            case ColUser:
                return a.user.localeAwareCompare(b.user) > 0;
            case ColCpu:
                return a.cpuPercent > b.cpuPercent;
            case ColMemory:
                return a.memoryBytes > b.memoryBytes;
            case ColDisk:
                return (a.readBytesPerSec + a.writeBytesPerSec) > (b.readBytesPerSec + b.writeBytesPerSec);
            case ColPid:
                return a.pid > b.pid;
            case ColPriority:
                return a.priority > b.priority;
            default:
                return a.cpuPercent > b.cpuPercent;
            }
        }
    };

    std::sort(filtered.begin(), filtered.end(), comparator);

    beginResetModel();
    m_filteredProcesses = std::move(filtered);
    endResetModel();

    emit countChanged();
}
