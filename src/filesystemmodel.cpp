/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - File Systems Model Implementation
 */

#include "filesystemmodel.h"

#include <QFile>
#include <QTextStream>
#include <QSet>
#include <sys/statvfs.h>

FileSystemModel::FileSystemModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &FileSystemModel::updateFileSystems);
    if (m_active) {
        updateFileSystems();
        m_timer->start(5000);
    }
}

FileSystemModel::~FileSystemModel()
{
    m_timer->stop();
}

void FileSystemModel::setActive(bool active)
{
    if (m_active == active)
        return;

    m_active = active;
    if (m_active) {
        updateFileSystems();
        m_timer->start(5000);
    } else {
        m_timer->stop();
    }
    emit activeChanged();
}

int FileSystemModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_fileSystems.size();
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_fileSystems.size())
        return QVariant();

    const FileSystemInfo &fs = m_fileSystems.at(index.row());

    switch (role) {
    case DeviceRole:
        return fs.device;
    case MountPointRole:
        return fs.mountPoint;
    case FsTypeRole:
        return fs.fsType;
    case TotalBytesRole:
        return (qulonglong)fs.totalBytes;
    case UsedBytesRole:
        return (qulonglong)fs.usedBytes;
    case FreeBytesRole:
        return (qulonglong)fs.freeBytes;
    case UsedPercentRole:
        return fs.usedPercent;
    case TotalFormattedRole:
        return fs.totalFormatted;
    case UsedFormattedRole:
        return fs.usedFormatted;
    case FreeFormattedRole:
        return fs.freeFormatted;
    case IsRootRole:
        return fs.isRoot;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> FileSystemModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[DeviceRole] = "device";
    roles[MountPointRole] = "mountPoint";
    roles[FsTypeRole] = "fsType";
    roles[TotalBytesRole] = "totalBytes";
    roles[UsedBytesRole] = "usedBytes";
    roles[FreeBytesRole] = "freeBytes";
    roles[UsedPercentRole] = "usedPercent";
    roles[TotalFormattedRole] = "totalFormatted";
    roles[UsedFormattedRole] = "usedFormatted";
    roles[FreeFormattedRole] = "freeFormatted";
    roles[IsRootRole] = "isRoot";
    return roles;
}

void FileSystemModel::refresh()
{
    updateFileSystems();
}

void FileSystemModel::updateFileSystems()
{
    QFile file("/proc/mounts");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString line;

    // Filter set of ignorable virtual fs types
    static const QSet<QString> ignoredTypes = {
        "proc", "sysfs", "devtmpfs", "devpts", "cgroup", "cgroup2",
        "pstore", "securityfs", "configfs", "autofs", "mqueue",
        "hugetlbfs", "debugfs", "tracefs", "fusectl", "bpf", "ramfs",
        "overlay", "efivarfs", "tmpfs", "squashfs", "binfmt_misc"
    };

    QVector<FileSystemInfo> list;
    QSet<QString> seenMounts;

    while (in.readLineInto(&line)) {
        QStringList parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        if (parts.size() < 3) continue;

        QString device = parts[0];
        QString mountPoint = parts[1];
        QString fsType = parts[2];

        // Only physical block devices or specific network mounts
        if (!device.startsWith("/dev/") && !fsType.contains("zfs") && !fsType.contains("btrfs"))
            continue;

        if (ignoredTypes.contains(fsType))
            continue;

        // Skip mount points that are sub-cgroups or snap mounts or duplicates
        if (seenMounts.contains(mountPoint))
            continue;

        struct statvfs stat;
        if (statvfs(mountPoint.toUtf8().constData(), &stat) != 0)
            continue;

        if (stat.f_blocks == 0)
            continue;

        seenMounts.insert(mountPoint);

        FileSystemInfo info;
        info.device = device;
        info.mountPoint = mountPoint;
        info.fsType = fsType;
        info.isRoot = (mountPoint == QLatin1String("/"));

        info.totalBytes = (qulonglong)stat.f_blocks * stat.f_frsize;
        info.freeBytes = (qulonglong)stat.f_bavail * stat.f_frsize;
        info.usedBytes = (info.totalBytes >= info.freeBytes) ? (info.totalBytes - info.freeBytes) : 0;
        info.usedPercent = (info.totalBytes > 0) ? ((double)info.usedBytes / (double)info.totalBytes * 100.0) : 0.0;

        info.totalFormatted = formatBytes(info.totalBytes);
        info.usedFormatted = formatBytes(info.usedBytes);
        info.freeFormatted = formatBytes(info.freeBytes);

        list.append(info);
    }
    file.close();

    // Sort: Root mount '/' first, then alphabetical by mount point
    std::sort(list.begin(), list.end(), [](const FileSystemInfo &a, const FileSystemInfo &b) {
        if (a.isRoot) return true;
        if (b.isRoot) return false;
        return a.mountPoint.localeAwareCompare(b.mountPoint) < 0;
    });

    beginResetModel();
    m_fileSystems = std::move(list);
    endResetModel();

    emit countChanged();
}

QString FileSystemModel::formatBytes(qulonglong bytes)
{
    if (bytes >= 1024ULL * 1024 * 1024) {
        return QString::asprintf("%.1f GB", bytes / (1000.0 * 1000.0 * 1000.0));
    } else if (bytes >= 1024ULL * 1024) {
        return QString::asprintf("%.1f MB", bytes / (1000.0 * 1000.0));
    } else {
        return QString("%1 KB").arg(bytes / 1000);
    }
}
