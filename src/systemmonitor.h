/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - System Monitor Backend
 */

#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QStringList>
#include <QVector>

struct CpuTimeSample {
    qulonglong user = 0;
    qulonglong nice = 0;
    qulonglong system = 0;
    qulonglong idle = 0;
    qulonglong iowait = 0;
    qulonglong irq = 0;
    qulonglong softirq = 0;
    qulonglong steal = 0;

    qulonglong total() const {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }
    qulonglong active() const {
        return user + nice + system + irq + softirq + steal;
    }
};

class SystemMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

    // CPU Properties
    Q_PROPERTY(double cpuPercent READ cpuPercent NOTIFY cpuUpdated)
    Q_PROPERTY(QVariantList cpuPerCore READ cpuPerCore NOTIFY cpuUpdated)
    Q_PROPERTY(int cpuCount READ cpuCount NOTIFY cpuUpdated)
    Q_PROPERTY(QString cpuModel READ cpuModel NOTIFY cpuUpdated)
    Q_PROPERTY(QString cpuFreq READ cpuFreq NOTIFY cpuUpdated)
    Q_PROPERTY(QVariantList cpuHistory READ cpuHistory NOTIFY historyUpdated)

    // Memory & Swap Properties
    Q_PROPERTY(qulonglong memTotal READ memTotal NOTIFY memoryUpdated)
    Q_PROPERTY(qulonglong memUsed READ memUsed NOTIFY memoryUpdated)
    Q_PROPERTY(qulonglong memAvailable READ memAvailable NOTIFY memoryUpdated)
    Q_PROPERTY(double memPercent READ memPercent NOTIFY memoryUpdated)
    Q_PROPERTY(QString memFormatted READ memFormatted NOTIFY memoryUpdated)
    Q_PROPERTY(qulonglong swapTotal READ swapTotal NOTIFY memoryUpdated)
    Q_PROPERTY(qulonglong swapUsed READ swapUsed NOTIFY memoryUpdated)
    Q_PROPERTY(double swapPercent READ swapPercent NOTIFY memoryUpdated)
    Q_PROPERTY(QString swapFormatted READ swapFormatted NOTIFY memoryUpdated)
    Q_PROPERTY(QVariantList memoryHistory READ memoryHistory NOTIFY historyUpdated)
    Q_PROPERTY(QVariantList swapHistory READ swapHistory NOTIFY historyUpdated)

    // Network Properties
    Q_PROPERTY(qulonglong netRxRate READ netRxRate NOTIFY networkUpdated)
    Q_PROPERTY(qulonglong netTxRate READ netTxRate NOTIFY networkUpdated)
    Q_PROPERTY(QString netRxRateFormatted READ netRxRateFormatted NOTIFY networkUpdated)
    Q_PROPERTY(QString netTxRateFormatted READ netTxRateFormatted NOTIFY networkUpdated)
    Q_PROPERTY(qulonglong netTotalRx READ netTotalRx NOTIFY networkUpdated)
    Q_PROPERTY(qulonglong netTotalTx READ netTotalTx NOTIFY networkUpdated)
    Q_PROPERTY(QString netTotalRxFormatted READ netTotalRxFormatted NOTIFY networkUpdated)
    Q_PROPERTY(QString netTotalTxFormatted READ netTotalTxFormatted NOTIFY networkUpdated)
    Q_PROPERTY(QVariantList netRxHistory READ netRxHistory NOTIFY historyUpdated)
    Q_PROPERTY(QVariantList netTxHistory READ netTxHistory NOTIFY historyUpdated)

    // Disk I/O Properties
    Q_PROPERTY(qulonglong diskReadRate READ diskReadRate NOTIFY diskUpdated)
    Q_PROPERTY(qulonglong diskWriteRate READ diskWriteRate NOTIFY diskUpdated)
    Q_PROPERTY(QString diskReadRateFormatted READ diskReadRateFormatted NOTIFY diskUpdated)
    Q_PROPERTY(QString diskWriteRateFormatted READ diskWriteRateFormatted NOTIFY diskUpdated)
    Q_PROPERTY(QVariantList diskReadHistory READ diskReadHistory NOTIFY historyUpdated)
    Q_PROPERTY(QVariantList diskWriteHistory READ diskWriteHistory NOTIFY historyUpdated)

    // System Overview Properties
    Q_PROPERTY(QString osName READ osName NOTIFY sysInfoUpdated)
    Q_PROPERTY(QString kernelRelease READ kernelRelease NOTIFY sysInfoUpdated)
    Q_PROPERTY(QString hostname READ hostname NOTIFY sysInfoUpdated)
    Q_PROPERTY(QString uptimeFormatted READ uptimeFormatted NOTIFY uptimeUpdated)

public:
    explicit SystemMonitor(QObject *parent = nullptr);
    ~SystemMonitor() override;

    bool active() const { return m_active; }
    void setActive(bool active);

    double cpuPercent() const { return m_cpuPercent; }
    QVariantList cpuPerCore() const { return m_cpuPerCore; }
    int cpuCount() const { return m_cpuCount; }
    QString cpuModel() const { return m_cpuModel; }
    QString cpuFreq() const { return m_cpuFreq; }
    QVariantList cpuHistory() const { return m_cpuHistory; }

    qulonglong memTotal() const { return m_memTotal; }
    qulonglong memUsed() const { return m_memUsed; }
    qulonglong memAvailable() const { return m_memAvailable; }
    double memPercent() const { return m_memPercent; }
    QString memFormatted() const;

    qulonglong swapTotal() const { return m_swapTotal; }
    qulonglong swapUsed() const { return m_swapUsed; }
    double swapPercent() const { return m_swapPercent; }
    QString swapFormatted() const;

    QVariantList memoryHistory() const { return m_memoryHistory; }
    QVariantList swapHistory() const { return m_swapHistory; }

    qulonglong netRxRate() const { return m_netRxRate; }
    qulonglong netTxRate() const { return m_netTxRate; }
    QString netRxRateFormatted() const { return formatRate(m_netRxRate); }
    QString netTxRateFormatted() const { return formatRate(m_netTxRate); }
    qulonglong netTotalRx() const { return m_netTotalRx; }
    qulonglong netTotalTx() const { return m_netTotalTx; }
    QString netTotalRxFormatted() const { return formatBytes(m_netTotalRx); }
    QString netTotalTxFormatted() const { return formatBytes(m_netTotalTx); }
    QVariantList netRxHistory() const { return m_netRxHistory; }
    QVariantList netTxHistory() const { return m_netTxHistory; }

    qulonglong diskReadRate() const { return m_diskReadRate; }
    qulonglong diskWriteRate() const { return m_diskWriteRate; }
    QString diskReadRateFormatted() const { return formatRate(m_diskReadRate); }
    QString diskWriteRateFormatted() const { return formatRate(m_diskWriteRate); }
    QVariantList diskReadHistory() const { return m_diskReadHistory; }
    QVariantList diskWriteHistory() const { return m_diskWriteHistory; }

    QString osName() const { return m_osName; }
    QString kernelRelease() const { return m_kernelRelease; }
    QString hostname() const { return m_hostname; }
    QString uptimeFormatted() const { return m_uptimeFormatted; }

    Q_INVOKABLE static QString formatBytes(qulonglong bytes);
    Q_INVOKABLE static QString formatRate(qulonglong bytesPerSec);

signals:
    void activeChanged();
    void cpuUpdated();
    void memoryUpdated();
    void networkUpdated();
    void diskUpdated();
    void historyUpdated();
    void sysInfoUpdated();
    void uptimeUpdated();

private slots:
    void updateStats();

private:
    void initSystemInfo();
    void readCpuStats();
    void readMemoryStats();
    void readNetworkStats();
    void readDiskStats();
    void updateUptime();

    void appendHistory(QVariantList &list, double val, int maxPoints = 60);

    QTimer *m_timer;
    bool m_active = false;
    const int m_historyLimit = 60; // 60 points of history

    // CPU
    double m_cpuPercent = 0.0;
    QVariantList m_cpuPerCore;
    int m_cpuCount = 1;
    QString m_cpuModel;
    QString m_cpuFreq;
    CpuTimeSample m_prevTotalCpu;
    QVector<CpuTimeSample> m_prevCoreCpus;
    QVariantList m_cpuHistory;

    // Memory
    qulonglong m_memTotal = 0;
    qulonglong m_memUsed = 0;
    qulonglong m_memAvailable = 0;
    double m_memPercent = 0.0;
    qulonglong m_swapTotal = 0;
    qulonglong m_swapUsed = 0;
    double m_swapPercent = 0.0;
    QVariantList m_memoryHistory;
    QVariantList m_swapHistory;

    // Network
    qulonglong m_prevRxBytes = 0;
    qulonglong m_prevTxBytes = 0;
    qulonglong m_netRxRate = 0;
    qulonglong m_netTxRate = 0;
    qulonglong m_netTotalRx = 0;
    qulonglong m_netTotalTx = 0;
    bool m_firstNetRead = true;
    QVariantList m_netRxHistory;
    QVariantList m_netTxHistory;

    // Disk
    qulonglong m_prevDiskReadSectors = 0;
    qulonglong m_prevDiskWriteSectors = 0;
    qulonglong m_diskReadRate = 0;
    qulonglong m_diskWriteRate = 0;
    bool m_firstDiskRead = true;
    QVariantList m_diskReadHistory;
    QVariantList m_diskWriteHistory;

    // System info
    QString m_osName;
    QString m_kernelRelease;
    QString m_hostname;
    QString m_uptimeFormatted;
    qint64 m_lastSampleTime = 0;
};

#endif // SYSTEMMONITOR_H
