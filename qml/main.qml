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
    }

    FileSystemModel {
        id: fileSystemModel
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

        // Page 0: Processes
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

        // Page 1: Resources (Live Charts)
        ResourcesView {
            id: resourcesView
        }

        // Page 2: File Systems
        FileSystemsView {
            id: fileSystemsView
        }

        // Page 3: System Overview
        SystemInfoView {
            id: systemInfoView
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
