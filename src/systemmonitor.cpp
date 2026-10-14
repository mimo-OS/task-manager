/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - System Monitor Backend Implementation
 */

#include "systemmonitor.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <algorithm>

#include <sys/utsname.h>
#include <unistd.h>

SystemMonitor::SystemMonitor(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    initSystemInfo();

    // Pre-fill history lists with 60 zeroes so graphs are immediately populated
    for (int i = 0; i < m_historyLimit; ++i) {
        m_cpuHistory.append(0.0);
        m_memoryHistory.append(0.0);
        m_swapHistory.append(0.0);
        m_netRxHistory.append(0.0);
        m_netTxHistory.append(0.0);
        m_diskReadHistory.append(0.0);
        m_diskWriteHistory.append(0.0);
    }

    m_lastSampleTime = QDateTime::currentMSecsSinceEpoch();

    // First sample to initialize baselines
    readCpuStats();
    readMemoryStats();
    readNetworkStats();
    readDiskStats();
    updateUptime();

    connect(m_timer, &QTimer::timeout, this, &SystemMonitor::updateStats);
    if (m_active) {
        m_timer->start(1000);
    }
}

SystemMonitor::~SystemMonitor()
{
    m_timer->stop();
}

void SystemMonitor::setActive(bool active)
{
    if (m_active == active)
        return;

    m_active = active;
    if (m_active) {
        updateStats();
        m_timer->start(1000);
    } else {
        m_timer->stop();
    }
    emit activeChanged();
}

void SystemMonitor::initSystemInfo()
{
    // Hostname and Kernel
    struct utsname uts;
    if (uname(&uts) == 0) {
        m_kernelRelease = QString::fromUtf8(uts.release);
        m_hostname = QString::fromUtf8(uts.nodename);
    }

    // OS Name
    QFile osRelease("/etc/os-release");
    if (osRelease.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&osRelease);
        QString line;
        while (in.readLineInto(&line)) {
            if (line.startsWith("PRETTY_NAME=")) {
                QString name = line.mid(12);
                if (name.startsWith('"') && name.endsWith('"')) {
                    name = name.mid(1, name.length() - 2);
                }
                m_osName = name;
                break;
            }
        }
        osRelease.close();
    }
    if (m_osName.isEmpty() || m_osName.contains("Cutefish", Qt::CaseInsensitive)) {
        m_osName = QStringLiteral("mimo OS");
    }

    // CPU Model and core count
    m_cpuCount = std::max(1, (int)sysconf(_SC_NPROCESSORS_ONLN));
    m_prevCoreCpus.resize(m_cpuCount);

    QFile cpuInfo("/proc/cpuinfo");
    if (cpuInfo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&cpuInfo);
        QString line;
        while (in.readLineInto(&line)) {
            if (m_cpuModel.isEmpty() && line.startsWith("model name")) {
                int colon = line.indexOf(':');
                if (colon != -1) {
                    m_cpuModel = line.mid(colon + 1).trimmed();
                }
            } else if (line.startsWith("cpu MHz")) {
                int colon = line.indexOf(':');
                if (colon != -1) {
                    m_cpuFreq = QString("%1 MHz").arg(line.mid(colon + 1).trimmed());
                }
            }
        }
        cpuInfo.close();
    }
    if (m_cpuModel.isEmpty()) {
        m_cpuModel = QString("%1 Cores Processor").arg(m_cpuCount);
    }

    emit sysInfoUpdated();
}

void SystemMonitor::updateStats()
{
    readCpuStats();
    readMemoryStats();
    readNetworkStats();
    readDiskStats();
    updateUptime();

    emit historyUpdated();
}

void SystemMonitor::appendHistory(QVariantList &list, double val, int maxPoints)
{
    list.append(val);
    while (list.size() > maxPoints) {
        list.removeFirst();
    }
}

void SystemMonitor::readCpuStats()
{
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString line;
    QVariantList perCoreList;

    while (in.readLineInto(&line)) {
        if (line.startsWith("cpu ")) {
            QStringList parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            if (parts.size() >= 8) {
                CpuTimeSample sample;
                sample.user = parts[1].toULongLong();
                sample.nice = parts[2].toULongLong();
                sample.system = parts[3].toULongLong();
                sample.idle = parts[4].toULongLong();
                sample.iowait = parts[5].toULongLong();
                sample.irq = parts[6].toULongLong();
                sample.softirq = parts[7].toULongLong();
                if (parts.size() >= 9) sample.steal = parts[8].toULongLong();

                qulonglong totalDelta = sample.total() - m_prevTotalCpu.total();
                qulonglong activeDelta = sample.active() - m_prevTotalCpu.active();

                if (totalDelta > 0 && m_prevTotalCpu.total() > 0) {
                    double percent = ((double)activeDelta / (double)totalDelta) * 100.0;
                    m_cpuPercent = std::clamp(percent, 0.0, 100.0);
                }
                m_prevTotalCpu = sample;
            }
        } else if (line.startsWith("cpu") && line.at(3).isDigit()) {
            QStringList parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            if (parts.size() >= 8) {
                QString coreNumStr = parts[0].mid(3);
                int coreIdx = coreNumStr.toInt();

                CpuTimeSample sample;
                sample.user = parts[1].toULongLong();
                sample.nice = parts[2].toULongLong();
                sample.system = parts[3].toULongLong();
                sample.idle = parts[4].toULongLong();
                sample.iowait = parts[5].toULongLong();
                sample.irq = parts[6].toULongLong();
                sample.softirq = parts[7].toULongLong();
                if (parts.size() >= 9) sample.steal = parts[8].toULongLong();

                double corePercent = 0.0;
                if (coreIdx < m_prevCoreCpus.size()) {
                    const CpuTimeSample &prev = m_prevCoreCpus[coreIdx];
                    qulonglong totalDelta = sample.total() - prev.total();
                    qulonglong activeDelta = sample.active() - prev.active();
                    if (totalDelta > 0 && prev.total() > 0) {
                        corePercent = std::clamp(((double)activeDelta / (double)totalDelta) * 100.0, 0.0, 100.0);
                    }
                    m_prevCoreCpus[coreIdx] = sample;
                } else {
                    m_prevCoreCpus.append(sample);
                }
                perCoreList.append(corePercent);
            }
        } else if (!line.startsWith("cpu")) {
            break;
        }
    }
    file.close();

    m_cpuPerCore = perCoreList;
    appendHistory(m_cpuHistory, m_cpuPercent, m_historyLimit);

    emit cpuUpdated();
}

void SystemMonitor::readMemoryStats()
{
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString line;
    qulonglong memFree = 0, buffers = 0, cached = 0, swapFree = 0;

    while (in.readLineInto(&line)) {
        if (line.startsWith("MemTotal:")) {
            m_memTotal = line.split(QLatin1Char(' '), Qt::SkipEmptyParts).value(1).toULongLong() * 1024;
        } else if (line.startsWith("MemAvailable:")) {
            m_memAvailable = line.split(QLatin1Char(' '), Qt::SkipEmptyParts).value(1).toULongLong() * 1024;
        } else if (line.startsWith("MemFree:")) {
            memFree = line.split(QLatin1Char(' '), Qt::SkipEmptyParts).value(1).toULongLong() * 1024;
        } else if (line.startsWith("Buffers:")) {
            buffers = line.split(QLatin1Char(' '), Qt::SkipEmptyParts).value(1).toULongLong() * 1024;
        } else if (line.startsWith("Cached:")) {
            cached = line.split(QLatin1Char(' '), Qt::SkipEmptyParts).value(1).toULongLong() * 1024;
        } else if (line.startsWith("SwapTotal:")) {
            m_swapTotal = line.split(QLatin1Char(' '), Qt::SkipEmptyParts).value(1).toULongLong() * 1024;
        } else if (line.startsWith("SwapFree:")) {
            swapFree = line.split(QLatin1Char(' '), Qt::SkipEmptyParts).value(1).toULongLong() * 1024;
        }
    }
    file.close();

    if (m_memAvailable == 0) {
        m_memAvailable = memFree + buffers + cached;
    }
    m_memUsed = (m_memTotal >= m_memAvailable) ? (m_memTotal - m_memAvailable) : 0;
    m_memPercent = (m_memTotal > 0) ? ((double)m_memUsed / (double)m_memTotal * 100.0) : 0.0;

    m_swapUsed = (m_swapTotal >= swapFree) ? (m_swapTotal - swapFree) : 0;
    m_swapPercent = (m_swapTotal > 0) ? ((double)m_swapUsed / (double)m_swapTotal * 100.0) : 0.0;

    appendHistory(m_memoryHistory, m_memPercent, m_historyLimit);
    appendHistory(m_swapHistory, m_swapPercent, m_historyLimit);

    emit memoryUpdated();
}

void SystemMonitor::readNetworkStats()
{
    QFile file("/proc/net/dev");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString line;
    // Skip two header lines
    in.readLine();
    in.readLine();

    qulonglong totalRx = 0;
    qulonglong totalTx = 0;

    while (in.readLineInto(&line)) {
        int colon = line.indexOf(':');
        if (colon == -1) continue;

        QString iface = line.left(colon).trimmed();
        if (iface == QLatin1String("lo")) // skip loopback
            continue;

        QString dataStr = line.mid(colon + 1);
        QStringList parts = dataStr.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        if (parts.size() >= 9) {
            qulonglong rxBytes = parts[0].toULongLong();
            qulonglong txBytes = parts[8].toULongLong();
            totalRx += rxBytes;
            totalTx += txBytes;
        }
    }
    file.close();

    m_netTotalRx = totalRx;
    m_netTotalTx = totalTx;

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    double elapsedSec = (now - m_lastSampleTime) / 1000.0;
    if (elapsedSec <= 0.0) elapsedSec = 1.0;
    m_lastSampleTime = now;

    if (!m_firstNetRead) {
        if (totalRx >= m_prevRxBytes) {
            m_netRxRate = (qulonglong)((totalRx - m_prevRxBytes) / elapsedSec);
        }
        if (totalTx >= m_prevTxBytes) {
            m_netTxRate = (qulonglong)((totalTx - m_prevTxBytes) / elapsedSec);
        }
    } else {
        m_firstNetRead = false;
        m_netRxRate = 0;
        m_netTxRate = 0;
    }
    m_prevRxBytes = totalRx;
    m_prevTxBytes = totalTx;

    // Convert rate to KB/s for history graph
    appendHistory(m_netRxHistory, (double)m_netRxRate / 1024.0, m_historyLimit);
    appendHistory(m_netTxHistory, (double)m_netTxRate / 1024.0, m_historyLimit);

    emit networkUpdated();
}

void SystemMonitor::readDiskStats()
{
    QFile file("/proc/diskstats");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString line;
    qulonglong totalReadSectors = 0;
    qulonglong totalWriteSectors = 0;

    while (in.readLineInto(&line)) {
        QStringList parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        if (parts.size() >= 10) {
            QString devName = parts[2];
            // Filter only full disks (sdX, nvmeXnX, vdX, mmcblkX)
            if (devName.startsWith("sd") && devName.length() == 3) {
                totalReadSectors += parts[5].toULongLong();
                totalWriteSectors += parts[9].toULongLong();
            } else if (devName.startsWith("nvme") && devName.contains("n1") && !devName.contains("p")) {
                totalReadSectors += parts[5].toULongLong();
                totalWriteSectors += parts[9].toULongLong();
            } else if (devName.startsWith("vd") && devName.length() == 3) {
                totalReadSectors += parts[5].toULongLong();
                totalWriteSectors += parts[9].toULongLong();
            }
        }
    }
    file.close();

    if (!m_firstDiskRead) {
        if (totalReadSectors >= m_prevDiskReadSectors) {
            m_diskReadRate = (totalReadSectors - m_prevDiskReadSectors) * 512;
        }
        if (totalWriteSectors >= m_prevDiskWriteSectors) {
            m_diskWriteRate = (totalWriteSectors - m_prevDiskWriteSectors) * 512;
        }
    } else {
        m_firstDiskRead = false;
        m_diskReadRate = 0;
        m_diskWriteRate = 0;
    }
    m_prevDiskReadSectors = totalReadSectors;
    m_prevDiskWriteSectors = totalWriteSectors;

    // Store in KB/s for history graph
    appendHistory(m_diskReadHistory, (double)m_diskReadRate / 1024.0, m_historyLimit);
    appendHistory(m_diskWriteHistory, (double)m_diskWriteRate / 1024.0, m_historyLimit);

    emit diskUpdated();
}

void SystemMonitor::updateUptime()
{
    QFile file("/proc/uptime");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QByteArray content = file.readAll();
    file.close();

    double uptimeSec = 0.0;
    sscanf(content.constData(), "%lf", &uptimeSec);

    long totalSeconds = (long)uptimeSec;
    long days = totalSeconds / 86400;
    long hours = (totalSeconds % 86400) / 3600;
    long minutes = (totalSeconds % 3600) / 60;
    long seconds = totalSeconds % 60;

    if (days > 0) {
        m_uptimeFormatted = QString("%1d %2h %3m").arg(days).arg(hours).arg(minutes);
    } else if (hours > 0) {
        m_uptimeFormatted = QString("%1h %2m %3s").arg(hours).arg(minutes).arg(seconds);
    } else {
        m_uptimeFormatted = QString("%1m %2s").arg(minutes).arg(seconds);
    }

    emit uptimeUpdated();
}

QString SystemMonitor::memFormatted() const
{
    return QString("%1 / %2 (%3%)")
        .arg(formatBytes(m_memUsed))
        .arg(formatBytes(m_memTotal))
        .arg(QString::asprintf("%.1f", m_memPercent));
}

QString SystemMonitor::swapFormatted() const
{
    if (m_swapTotal == 0) return QStringLiteral("None configured");
    return QString("%1 / %2 (%3%)")
        .arg(formatBytes(m_swapUsed))
        .arg(formatBytes(m_swapTotal))
        .arg(QString::asprintf("%.1f", m_swapPercent));
}

QString SystemMonitor::formatBytes(qulonglong bytes)
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

QString SystemMonitor::formatRate(qulonglong bytesPerSec)
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
