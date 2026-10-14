/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - Process Model
 */

#ifndef PROCESSMODEL_H
#define PROCESSMODEL_H

#include <QAbstractTableModel>
#include <QTimer>
#include <QHash>
#include <QVector>
#include <QSet>

struct ProcessInfo {
    int pid = 0;
    int ppid = 0;
    QString name;
    QString cmdline;
    QString user;
    uint uid = 0;
    double cpuPercent = 0.0;
    qulonglong memoryBytes = 0;
    double memoryPercent = 0.0;
    qulonglong virtualMemoryBytes = 0;
    QString status;
    int priority = 0; // nice value (-20 to 19)
    int threads = 1;
    qulonglong readBytesPerSec = 0;
    qulonglong writeBytesPerSec = 0;
    QString iconName;
    qulonglong startTime = 0;

    // Internal tracking for deltas
    qulonglong prevUtime = 0;
    qulonglong prevStime = 0;
    qulonglong prevIoRead = 0;
    qulonglong prevIoWrite = 0;
};

class ProcessModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString searchFilter READ searchFilter WRITE setSearchFilter NOTIFY searchFilterChanged)
    Q_PROPERTY(int filterType READ filterType WRITE setFilterType NOTIFY filterTypeChanged)
    Q_PROPERTY(int sortColumn READ sortColumn WRITE setSortColumn NOTIFY sortColumnChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    Q_PROPERTY(int refreshInterval READ refreshInterval WRITE setRefreshInterval NOTIFY refreshIntervalChanged)

public:
    enum ProcessRoles {
        PidRole = Qt::UserRole + 1,
        PpidRole,
        NameRole,
        CmdlineRole,
        UserRole,
        UidRole,
        CpuPercentRole,
        CpuPercentFormattedRole,
        MemoryBytesRole,
        MemoryFormattedRole,
        MemoryPercentRole,
        VirtualMemoryBytesRole,
        VirtualMemoryFormattedRole,
        StatusRole,
        PriorityRole,
        ThreadsRole,
        IoFormattedRole,
        IconNameRole
    };
    Q_ENUM(ProcessRoles)

    enum Columns {
        ColName = 0,
        ColUser,
        ColCpu,
        ColMemory,
        ColDisk,
        ColPid,
        ColPriority,
        ColCount
    };
    Q_ENUM(Columns)

    enum FilterTypes {
        FilterAll = 0,
        FilterMine,
        FilterActive
    };
    Q_ENUM(FilterTypes)

    explicit ProcessModel(QObject *parent = nullptr);
    ~ProcessModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return m_filteredProcesses.size(); }
    QString searchFilter() const { return m_searchFilter; }
    void setSearchFilter(const QString &filter);

    int filterType() const { return m_filterType; }
    void setFilterType(int type);

    int sortColumn() const { return m_sortColumn; }
    void setSortColumn(int column);

    Qt::SortOrder sortOrder() const { return m_sortOrder; }
    void setSortOrder(Qt::SortOrder order);

    int refreshInterval() const { return m_refreshInterval; }
    void setRefreshInterval(int msec);

    Q_INVOKABLE void sortByColumn(int column);
    Q_INVOKABLE bool killProcess(int pid, int sig = 15);
    Q_INVOKABLE bool setProcessPriority(int pid, int niceValue);
    Q_INVOKABLE QVariantMap getProcessDetails(int pid) const;
    Q_INVOKABLE void refresh();

    static QString formatBytes(qulonglong bytes);
    static QString formatRate(qulonglong bytesPerSec);

signals:
    void countChanged();
    void searchFilterChanged();
    void filterTypeChanged();
    void sortColumnChanged();
    void sortOrderChanged();
    void refreshIntervalChanged();
    void processKilled(int pid, bool success, const QString &message);

private slots:
    void updateProcesses();

private:
    void applyFilterAndSort();
    QString resolveAppIcon(const QString &name, const QString &cmdline) const;
    QString resolveUsername(uint uid);
    qulonglong readTotalCpuTime();

    QTimer *m_timer;
    int m_refreshInterval = 2000; // 2 seconds default

    QHash<int, ProcessInfo> m_allProcesses;
    QVector<ProcessInfo> m_filteredProcesses;

    QString m_searchFilter;
    int m_filterType = FilterAll;
    int m_sortColumn = ColCpu;
    Qt::SortOrder m_sortOrder = Qt::DescendingOrder;

    uint m_currentUserUid;
    qulonglong m_prevTotalCpuTime = 0;
    qulonglong m_totalRamBytes = 0;
    long m_pageSize = 4096;
    long m_clockTicksPerSec = 100;
    int m_cpuCores = 1;

    QHash<uint, QString> m_userCache;
};

#endif // PROCESSMODEL_H
