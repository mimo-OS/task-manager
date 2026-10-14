/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - Main Entry Point
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QCommandLineParser>
#include <QIcon>
#include <QTranslator>
#include <QLocale>
#include <QFile>
#include <QDir>
#include <QTimer>

#if defined(__linux__)
#include <malloc.h>
#endif
#include <QPixmapCache>

#include "processmodel.h"
#include "systemmonitor.h"
#include "filesystemmodel.h"
#include "historychartitem.h"

int main(int argc, char *argv[])
{
#if defined(__linux__)
    mallopt(M_ARENA_MAX, 2);
    mallopt(M_TRIM_THRESHOLD, 128 * 1024);
    mallopt(M_MMAP_THRESHOLD, 64 * 1024);
#endif

    // If no NVIDIA GPU device exists, restrict EGL ICD loader to Mesa/system driver
    // to prevent libglvnd from unconditionally mapping huge unused proprietary NVIDIA libraries.
    if (!QFile::exists(QStringLiteral("/dev/nvidia0"))
        && QFile::exists(QStringLiteral("/usr/share/glvnd/egl_vendor.d/50_mesa.json"))
        && qEnvironmentVariableIsEmpty("__EGL_VENDOR_LIBRARY_FILENAMES")) {
        qputenv("__EGL_VENDOR_LIBRARY_FILENAMES", "/usr/share/glvnd/egl_vendor.d/50_mesa.json");
    }

    if (qEnvironmentVariableIsEmpty("QSG_RENDER_LOOP")) {
        qputenv("QSG_RENDER_LOOP", "basic");
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
#endif

    QGuiApplication app(argc, argv);
    QPixmapCache::setCacheLimit(4096);

#if defined(__linux__)
    QTimer *trimTimer = new QTimer(&app);
    QObject::connect(trimTimer, &QTimer::timeout, []() {
        malloc_trim(0);
    });
    trimTimer->start(30000);
    QTimer::singleShot(1500, []() {
        malloc_trim(0);
    });
#endif
    app.setApplicationName(QStringLiteral("cutefish-taskmanager"));
    app.setApplicationDisplayName(QStringLiteral("Task Manager"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setOrganizationName(QStringLiteral("cutefishos"));
    app.setOrganizationDomain(QStringLiteral("cutefishos.com"));

    // Set Window Icon with fallback to our bundled SVG
    QIcon appIcon = QIcon::fromTheme(QStringLiteral("cutefish-taskmanager"),
                                     QIcon(QStringLiteral(":/icons/cutefish-taskmanager.svg")));
    app.setWindowIcon(appIcon);

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Cutefish System Monitor and Task Manager"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption screenshotOption(QStringLiteral("screenshot"),
                                        QStringLiteral("Capture window screenshot and save to image path"),
                                        QStringLiteral("path"));
    parser.addOption(screenshotOption);

    QCommandLineOption tabOption(QStringLiteral("tab"),
                                 QStringLiteral("Open at tab index (0=Processes, 1=Resources, 2=FileSystems, 3=System)"),
                                 QStringLiteral("index"),
                                 QStringLiteral("0"));
    parser.addOption(tabOption);

    parser.process(app);

    const QString screenshotPath = parser.value(screenshotOption);
    const int initialTab = parser.value(tabOption).toInt();

    // Register QML Types
    qmlRegisterType<ProcessModel>("Cutefish.TaskManager", 1, 0, "ProcessModel");
    qmlRegisterType<SystemMonitor>("Cutefish.TaskManager", 1, 0, "SystemMonitor");
    qmlRegisterType<FileSystemModel>("Cutefish.TaskManager", 1, 0, "FileSystemModel");
    qmlRegisterType<HistoryChartItem>("Cutefish.TaskManager", 1, 0, "HistoryChart");

    QQmlApplicationEngine engine;

#ifdef QT_DEBUG
    engine.rootContext()->setContextProperty(QStringLiteral("debug"), true);
#else
    engine.rootContext()->setContextProperty(QStringLiteral("debug"), false);
#endif

    // Translations
    QLocale locale;
    QString qmFilePath = QString("%1/%2.qm")
        .arg(QStringLiteral("/usr/share/cutefish-taskmanager/translations/"))
        .arg(locale.name());
    if (QFile::exists(qmFilePath)) {
        QTranslator *translator = new QTranslator(QGuiApplication::instance());
        if (translator->load(qmFilePath)) {
            QGuiApplication::installTranslator(translator);
        } else {
            translator->deleteLater();
        }
    }

    engine.addImportPath(QStringLiteral("qrc:/"));
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    QObject *rootObj = engine.rootObjects().first();
    QQuickWindow *window = qobject_cast<QQuickWindow*>(rootObj);

    if (initialTab > 0 && rootObj) {
        QObject *viewStack = rootObj->findChild<QObject*>(QStringLiteral("viewStack"));
        if (viewStack) {
            viewStack->setProperty("currentIndex", initialTab);
        }
        QObject *headerBar = rootObj->findChild<QObject*>(QStringLiteral("headerBar"));
        if (headerBar) {
            headerBar->setProperty("currentTab", initialTab);
        }
    }

    if (!screenshotPath.isEmpty() && window) {
        window->show();
        QTimer::singleShot(1000, [window, screenshotPath]() {
            if (window) {
                QImage img = window->grabWindow();
                if (!img.isNull()) {
                    img.save(screenshotPath);
                }
            }
            QCoreApplication::quit();
        });
    }

    return app.exec();
}
