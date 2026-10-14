/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - Main Window
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FishUI 1.0 as FishUI
import Cutefish.TaskManager 1.0

FishUI.Window {
    id: rootWindow

    width: 960
    height: 640
    minimumWidth: 720
    minimumHeight: 480
    title: qsTr("Task Manager")

    background.color: FishUI.Theme.backgroundColor
    background.opacity: FishUI.Theme.blurEnabled ? (FishUI.Theme.darkMode ? 0.75 : 0.78) : 1.0

    header.height: 48

    // Backend Models
    ProcessModel {
        id: processModel
        onProcessKilled: function(pid, success, message) {
            rootWindow.showPassiveNotification(message, 3000)
        }
    }

    SystemMonitor {
        id: systemMonitor
        active: viewStack.currentIndex === 1 || viewStack.currentIndex === 3
    }

    FileSystemModel {
        id: fileSystemModel
        active: viewStack.currentIndex === 2
    }

    // Header Bar Integration
    headerItem: HeaderBar {
        id: headerBar
        objectName: "headerBar"
        currentTab: viewStack.currentIndex
        hasSelectedProcess: processesView.selectedPid > 0
        selectedProcessName: processesView.selectedName

        onTabChanged: function(index) {
            viewStack.currentIndex = index
        }

        onEndProcessClicked: {
            if (processesView.selectedPid > 0) {
                processModel.killProcess(processesView.selectedPid, 15) // SIGTERM
            }
        }

        onRefreshClicked: {
            processModel.refresh()
            fileSystemModel.refresh()
        }

        onFilterTypeSelected: function(type) {
            processModel.filterType = type
        }

        onSearchFilterChanged: {
            processModel.searchFilter = headerBar.searchFilter
        }
    }

    // Main Content Views
    StackLayout {
        id: viewStack
        objectName: "viewStack"
        anchors.fill: parent
        anchors.topMargin: 6
        currentIndex: 0

        // Page 0: Processes (Always active default)
        ProcessesView {
            id: processesView
            model: processModel

            onEndProcess: function(pid) {
                processModel.killProcess(pid, 15)
            }

            onKillProcess: function(pid) {
                processModel.killProcess(pid, 9) // SIGKILL
            }

            onOpenDetails: function(pid) {
                var details = processModel.getProcessDetails(pid)
                detailsDialog.processDetails = details
                detailsDialog.open()
            }

            onChangePriority: function(pid, name, curPriority) {
                priorityDialog.pid = pid
                priorityDialog.processName = name
                priorityDialog.currentPriority = curPriority
                priorityDialog.open()
            }
        }

        // Page 1: Resources (Live Charts, loaded on first visit)
        Loader {
            id: resourcesLoader
            active: viewStack.currentIndex === 1 || loaded
            property bool loaded: false
            source: "ResourcesView.qml"
            onLoaded: loaded = true
        }

        // Page 2: File Systems (loaded on first visit)
        Loader {
            id: fileSystemsLoader
            active: viewStack.currentIndex === 2 || loaded
            property bool loaded: false
            source: "FileSystemsView.qml"
            onLoaded: loaded = true
        }

        // Page 3: System Overview (loaded on first visit)
        Loader {
            id: systemInfoLoader
            active: viewStack.currentIndex === 3 || loaded
            property bool loaded: false
            source: "SystemInfoView.qml"
            onLoaded: loaded = true
        }
    }

    // Process Details Dialog
    ProcessDetailsDialog {
        id: detailsDialog

        onEndProcessRequested: function(pid) {
            processModel.killProcess(pid, 15)
        }

        onKillProcessRequested: function(pid) {
            processModel.killProcess(pid, 9)
        }
    }

    // Priority Change Dialog
    PriorityDialog {
        id: priorityDialog

        onPriorityChanged: function(pid, niceVal) {
            processModel.setProcessPriority(pid, niceVal)
        }
    }
}
