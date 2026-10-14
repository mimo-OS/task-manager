/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - Processes View
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FishUI 1.0 as FishUI
import Cutefish.TaskManager 1.0

Item {
    id: root

    property alias model: processList.model
    property alias selectedPid: root.currentSelectedPid
    property alias selectedName: root.currentSelectedName
    property int currentSelectedPid: -1
    property string currentSelectedName: ""
    property int currentSelectedIndex: -1

    signal endProcess(int pid)
    signal killProcess(int pid)
    signal openDetails(int pid)
    signal changePriority(int pid, string name, int curPriority)

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 14
        anchors.rightMargin: 14
        anchors.bottomMargin: 10
        spacing: 0

        // Column Headers Bar
        Rectangle {
            Layout.fillWidth: true
            height: 32
            radius: 8
            color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.05) : Qt.rgba(0, 0, 0, 0.04)
            border.width: 1
            border.color: FishUI.Theme.glassBorderColor

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 6

                // Column: Name
                HeaderColumnButton {
                    Layout.fillWidth: true
                    title: qsTr("Process Name")
                    column: ProcessModel.ColName
                    activeColumn: processModel.sortColumn
                    sortOrder: processModel.sortOrder
                    onClicked: processModel.sortByColumn(ProcessModel.ColName)
                }

                // Column: User
                HeaderColumnButton {
                    Layout.preferredWidth: 90
                    title: qsTr("User")
                    column: ProcessModel.ColUser
                    activeColumn: processModel.sortColumn
                    sortOrder: processModel.sortOrder
                    onClicked: processModel.sortByColumn(ProcessModel.ColUser)
                }

                // Column: % CPU
                HeaderColumnButton {
                    Layout.preferredWidth: 80
                    title: qsTr("% CPU")
                    alignment: Qt.AlignRight
                    column: ProcessModel.ColCpu
                    activeColumn: processModel.sortColumn
                    sortOrder: processModel.sortOrder
                    onClicked: processModel.sortByColumn(ProcessModel.ColCpu)
                }

                // Column: Memory
                HeaderColumnButton {
                    Layout.preferredWidth: 95
                    title: qsTr("Memory")
                    alignment: Qt.AlignRight
                    column: ProcessModel.ColMemory
                    activeColumn: processModel.sortColumn
                    sortOrder: processModel.sortOrder
                    onClicked: processModel.sortByColumn(ProcessModel.ColMemory)
                }

                // Column: Disk I/O
                HeaderColumnButton {
                    Layout.preferredWidth: 85
                    title: qsTr("Disk")
                    alignment: Qt.AlignRight
                    column: ProcessModel.ColDisk
                    activeColumn: processModel.sortColumn
                    sortOrder: processModel.sortOrder
                    onClicked: processModel.sortByColumn(ProcessModel.ColDisk)
                }

                // Column: PID
                HeaderColumnButton {
                    Layout.preferredWidth: 65
                    title: qsTr("PID")
                    alignment: Qt.AlignRight
                    column: ProcessModel.ColPid
                    activeColumn: processModel.sortColumn
                    sortOrder: processModel.sortOrder
                    onClicked: processModel.sortByColumn(ProcessModel.ColPid)
                }

                // Column: Priority (Nice)
                HeaderColumnButton {
                    Layout.preferredWidth: 65
                    title: qsTr("Priority")
                    alignment: Qt.AlignRight
                    column: ProcessModel.ColPriority
                    activeColumn: processModel.sortColumn
                    sortOrder: processModel.sortOrder
                    onClicked: processModel.sortByColumn(ProcessModel.ColPriority)
                }
            }
        }

        Item { height: 6 }

        // Process List
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 12
            color: FishUI.Theme.darkMode ? Qt.rgba(30/255, 30/255, 32/255, 0.45)
                                         : Qt.rgba(255/255, 255/255, 255/255, 0.55)
            border.width: 1
            border.color: FishUI.Theme.glassBorderColor
            clip: true

            ListView {
                id: processList
                anchors.fill: parent
                anchors.margins: 4
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                keyNavigationEnabled: true
                focus: true
                reuseItems: true
                cacheBuffer: 128

                ScrollBar.vertical: ScrollBar {
                    id: vScroll
                    active: true
                    width: 8
                    policy: ScrollBar.AsNeeded
                }

                delegate: Item {
                    id: rowDelegate
                    width: processList.width - (vScroll.visible ? vScroll.width + 4 : 0)
                    height: 32

                    readonly property bool isSelected: root.currentSelectedPid === model.pid
                    readonly property bool isHovered: rowMouseArea.containsMouse

                    Rectangle {
                        anchors.fill: parent
                        anchors.leftMargin: 2
                        anchors.rightMargin: 2
                        radius: 6
                        color: rowDelegate.isSelected
                               ? FishUI.Theme.highlightColor
                               : (rowDelegate.isHovered
                                  ? (FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.08) : Qt.rgba(0, 0, 0, 0.04))
                                  : "transparent")
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        spacing: 6

                        // Process Name and Icon
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Image {
                                width: 16
                                height: 16
                                sourceSize: Qt.size(16, 16)
                                Layout.alignment: Qt.AlignVCenter
                                fillMode: Image.PreserveAspectFit
                                source: {
                                    if (!model.iconName || model.iconName.length === 0 || model.iconName === "process-default")
                                        return "qrc:/icons/process-default.svg"
                                    if (model.iconName === "process-system")
                                        return "qrc:/icons/process-system.svg"
                                    return "image://icontheme/" + model.iconName
                                }
                            }

                            Text {
                                text: model.name
                                font.pixelSize: 12
                                font.weight: rowDelegate.isSelected ? Font.DemiBold : Font.Normal
                                color: rowDelegate.isSelected ? "#FFFFFF" : FishUI.Theme.textColor
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }

                        // User
                        Text {
                            Layout.preferredWidth: 90
                            text: model.user
                            font.pixelSize: 11
                            color: rowDelegate.isSelected ? "#FFFFFF" : FishUI.Theme.disabledTextColor
                            elide: Text.ElideRight
                        }

                        // CPU %
                        Text {
                            Layout.preferredWidth: 80
                            text: model.cpuPercentFormatted
                            font.pixelSize: 12
                            font.weight: model.cpuPercent > 10.0 ? Font.DemiBold : Font.Normal
                            horizontalAlignment: Text.AlignRight
                            color: rowDelegate.isSelected
                                   ? "#FFFFFF"
                                   : (model.cpuPercent > 20.0 ? "#FF3B30" : (model.cpuPercent > 5.0 ? "#007AFF" : FishUI.Theme.textColor))
                        }

                        // Memory
                        Text {
                            Layout.preferredWidth: 95
                            text: model.memoryFormatted
                            font.pixelSize: 11
                            horizontalAlignment: Text.AlignRight
                            color: rowDelegate.isSelected ? "#FFFFFF" : FishUI.Theme.textColor
                        }

                        // Disk I/O
                        Text {
                            Layout.preferredWidth: 85
                            text: model.ioFormatted
                            font.pixelSize: 11
                            horizontalAlignment: Text.AlignRight
                            color: rowDelegate.isSelected ? "#FFFFFF" : FishUI.Theme.disabledTextColor
                        }

                        // PID
                        Text {
                            Layout.preferredWidth: 65
                            text: String(model.pid)
                            font.pixelSize: 11
                            font.family: "monospace"
                            horizontalAlignment: Text.AlignRight
                            color: rowDelegate.isSelected ? "#FFFFFF" : FishUI.Theme.disabledTextColor
                        }

                        // Priority (Nice)
                        Text {
                            Layout.preferredWidth: 65
                            text: String(model.priority)
                            font.pixelSize: 11
                            horizontalAlignment: Text.AlignRight
                            color: rowDelegate.isSelected ? "#FFFFFF" : FishUI.Theme.disabledTextColor
                        }
                    }

                    MouseArea {
                        id: rowMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onClicked: function(mouse) {
                            processList.forceActiveFocus()
                            root.currentSelectedPid = model.pid
                            root.currentSelectedName = model.name
                            root.currentSelectedIndex = index

                            if (mouse.button === Qt.RightButton) {
                                contextMenu.popup(mouse.x, mouse.y)
                            }
                        }

                        onDoubleClicked: {
                            root.openDetails(model.pid)
                        }
                    }
                }

                // Keyboard handling
                Keys.onDeletePressed: {
                    if (root.currentSelectedPid > 0) {
                        root.endProcess(root.currentSelectedPid)
                    }
                }
                Keys.onReturnPressed: {
                    if (root.currentSelectedPid > 0) {
                        root.openDetails(root.currentSelectedPid)
                    }
                }
            }

            // Context Menu
            FishUI.Menu {
                id: contextMenu

                FishUI.MenuItem {
                    text: qsTr("Process Details...")
                    icon.name: "qrc:/icons/action-details.svg"
                    onTriggered: root.openDetails(root.currentSelectedPid)
                }

                FishUI.MenuSeparator {}

                FishUI.MenuItem {
                    text: qsTr("End Process (SIGTERM)")
                    icon.name: "qrc:/icons/action-end.svg"
                    onTriggered: root.endProcess(root.currentSelectedPid)
                }

                FishUI.MenuItem {
                    text: qsTr("Force Kill (SIGKILL)")
                    icon.name: "qrc:/icons/action-kill.svg"
                    onTriggered: root.killProcess(root.currentSelectedPid)
                }

                FishUI.MenuItem {
                    text: qsTr("Pause Process (SIGSTOP)")
                    icon.name: "qrc:/icons/action-pause.svg"
                    onTriggered: processModel.killProcess(root.currentSelectedPid, 19)
                }

                FishUI.MenuItem {
                    text: qsTr("Resume Process (SIGCONT)")
                    icon.name: "qrc:/icons/action-resume.svg"
                    onTriggered: processModel.killProcess(root.currentSelectedPid, 18)
                }

                FishUI.MenuSeparator {}

                FishUI.MenuItem {
                    text: qsTr("Change Priority...")
                    icon.name: "qrc:/icons/action-priority.svg"
                    onTriggered: {
                        var details = processModel.getProcessDetails(root.currentSelectedPid)
                        root.changePriority(root.currentSelectedPid, root.currentSelectedName, details.priority || 0)
                    }
                }

                FishUI.MenuSeparator {}

                FishUI.MenuItem {
                    text: qsTr("Copy PID")
                    onTriggered: {
                        clipboardHelper.copyText(String(root.currentSelectedPid))
                    }
                }
            }

            // Empty state placeholder
            Item {
                anchors.centerIn: parent
                visible: processList.count === 0
                width: 200
                height: 100

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 8

                    Image {
                        Layout.alignment: Qt.AlignHCenter
                        width: 32; height: 32
                        source: "qrc:/icons/tab-processes.svg"
                        sourceSize: Qt.size(32, 32)
                        opacity: 0.4
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("No matching processes")
                        font.pixelSize: 13
                        color: FishUI.Theme.disabledTextColor
                    }
                }
            }
        }

        // Status bar
        Item {
            Layout.fillWidth: true
            height: 26

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 6
                anchors.rightMargin: 6

                Text {
                    text: qsTr("%1 processes running").arg(processModel.count)
                    font.pixelSize: 11
                    color: FishUI.Theme.disabledTextColor
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: qsTr("CPU: %1% • RAM: %2").arg(systemMonitor.cpuPercent.toFixed(1)).arg(systemMonitor.memFormatted)
                    font.pixelSize: 11
                    color: FishUI.Theme.disabledTextColor
                }
            }
        }
    }

    // Helper item for header column buttons
    component HeaderColumnButton: MouseArea {
        id: colBtn
        property string title: ""
        property int column: 0
        property int activeColumn: 0
        property int sortOrder: Qt.DescendingOrder
        property int alignment: Qt.AlignLeft
        height: 28
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        Rectangle {
            anchors.fill: parent
            radius: 5
            color: colBtn.containsMouse
                   ? (FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.08) : Qt.rgba(0, 0, 0, 0.05))
                   : "transparent"
        }

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 6
            anchors.rightMargin: 6
            spacing: 4

            Text {
                Layout.fillWidth: true
                text: colBtn.title
                font.pixelSize: 11
                font.weight: colBtn.column === colBtn.activeColumn ? Font.DemiBold : Font.Normal
                color: colBtn.column === colBtn.activeColumn ? FishUI.Theme.textColor : FishUI.Theme.disabledTextColor
                horizontalAlignment: colBtn.alignment === Qt.AlignRight ? Text.AlignRight : Text.AlignLeft
            }

            Text {
                visible: colBtn.column === colBtn.activeColumn
                text: colBtn.sortOrder === Qt.AscendingOrder ? "▲" : "▼"
                font.pixelSize: 9
                color: FishUI.Theme.highlightColor
            }
        }
    }

    // Clipboard helper object
    QtObject {
        id: clipboardHelper
        function copyText(txt) {
            // Can use temporary text edit or context
        }
    }
}
