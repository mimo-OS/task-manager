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

#include "processmodel.h"
#include "systemmonitor.h"
#include "filesystemmodel.h"
#include "historychartitem.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
#endif

    QGuiApplication app(argc, argv);
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
