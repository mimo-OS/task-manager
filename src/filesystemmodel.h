/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - File Systems Model
 */

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QAbstractListModel>
#include <QTimer>
#include <QVector>

struct FileSystemInfo {
    QString device;
    QString mountPoint;
    QString fsType;
    qulonglong totalBytes = 0;
    qulonglong usedBytes = 0;
    qulonglong freeBytes = 0;
    double usedPercent = 0.0;
    QString totalFormatted;
    QString usedFormatted;
    QString freeFormatted;
    bool isRoot = false;
};

class FileSystemModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

public:
    enum FsRoles {
        DeviceRole = Qt::UserRole + 1,
        MountPointRole,
        FsTypeRole,
        TotalBytesRole,
        UsedBytesRole,
        FreeBytesRole,
        UsedPercentRole,
        TotalFormattedRole,
        UsedFormattedRole,
        FreeFormattedRole,
        IsRootRole
    };
    Q_ENUM(FsRoles)

    explicit FileSystemModel(QObject *parent = nullptr);
    ~FileSystemModel() override;

    bool active() const { return m_active; }
    void setActive(bool active);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return m_fileSystems.size(); }
    Q_INVOKABLE void refresh();

signals:
    void countChanged();
    void activeChanged();

private slots:
    void updateFileSystems();

private:
    static QString formatBytes(qulonglong bytes);

    QTimer *m_timer;
    bool m_active = false;
    QVector<FileSystemInfo> m_fileSystems;
};

#endif // FILESYSTEMMODEL_H
