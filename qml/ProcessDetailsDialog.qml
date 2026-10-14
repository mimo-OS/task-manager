/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - Process Details Dialog
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FishUI 1.0 as FishUI

Popup {
    id: root

    property var processDetails: ({})
    signal endProcessRequested(int pid)
    signal killProcessRequested(int pid)

    width: 480
    implicitHeight: contentColumn.implicitHeight + 40
    modal: true
    focus: true
    anchors.centerIn: parent

    background: Rectangle {
        radius: FishUI.Theme.hugeRadius
        color: FishUI.Theme.darkMode ? Qt.rgba(28/255, 28/255, 30/255, 0.94)
                                     : Qt.rgba(248/255, 248/255, 250/255, 0.96)
        border.width: 1
        border.color: FishUI.Theme.glassBorderColor
    }

    contentItem: ColumnLayout {
        id: contentColumn
        spacing: 16

        // Title and Process Icon
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Rectangle {
                width: 42
                height: 42
                radius: 10
                color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.08) : Qt.rgba(0, 0, 0, 0.05)
                border.width: 1
                border.color: FishUI.Theme.glassBorderColor

                Image {
                    anchors.centerIn: parent
                    width: 24
                    height: 24
                    source: root.processDetails.iconName
                            ? "qrc:/icons/" + root.processDetails.iconName + ".svg"
                            : "qrc:/icons/process-default.svg"
                    sourceSize: Qt.size(24, 24)
                }
            }

            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true

                Text {
                    text: root.processDetails.name || qsTr("Process")
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                    color: FishUI.Theme.textColor
                }

                Row {
                    spacing: 8
                    Text {
                        text: "PID " + (root.processDetails.pid || "")
                        font.pixelSize: 12
                        color: FishUI.Theme.disabledTextColor
                    }

                    Rectangle {
                        implicitHeight: 18
                        implicitWidth: statusText.implicitWidth + 10
                        radius: 9
                        color: root.processDetails.status === "Running"
                               ? Qt.rgba(52/255, 199/255, 89/255, 0.2)
                               : Qt.rgba(142/255, 142/255, 147/255, 0.2)

                        Text {
                            id: statusText
                            anchors.centerIn: parent
                            text: root.processDetails.status || ""
                            font.pixelSize: 10
                            font.weight: Font.Medium
                            color: root.processDetails.status === "Running" ? "#34C759" : FishUI.Theme.disabledTextColor
                        }
                    }
                }
            }

            // Close button
            MouseArea {
                width: 26
                height: 26
                cursorShape: Qt.PointingHandCursor
                onClicked: root.close()

                Rectangle {
                    anchors.fill: parent
                    radius: 13
                    color: parent.containsMouse
                           ? (FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.15) : Qt.rgba(0, 0, 0, 0.08))
                           : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "×"
                        font.pixelSize: 18
                        color: FishUI.Theme.disabledTextColor
                    }
                }
            }
        }

        // Details Grid Card
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: detailsGrid.implicitHeight + 20
            radius: 12
            color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.04) : Qt.rgba(0, 0, 0, 0.03)
            border.width: 1
            border.color: FishUI.Theme.glassBorderColor

            GridLayout {
                id: detailsGrid
                anchors.fill: parent
                anchors.margins: 12
                columns: 2
                rowSpacing: 8
                columnSpacing: 16

                Text { text: qsTr("User / UID:"); color: FishUI.Theme.disabledTextColor; font.pixelSize: 12 }
                Text { text: (root.processDetails.user || "") + " (" + (root.processDetails.uid || 0) + ")"; color: FishUI.Theme.textColor; font.pixelSize: 12; font.weight: Font.Medium }

                Text { text: qsTr("Parent PID:"); color: FishUI.Theme.disabledTextColor; font.pixelSize: 12 }
                Text { text: String(root.processDetails.ppid || ""); color: FishUI.Theme.textColor; font.pixelSize: 12 }

                Text { text: qsTr("CPU Usage:"); color: FishUI.Theme.disabledTextColor; font.pixelSize: 12 }
                Text { text: root.processDetails.cpuPercent || "0.0%"; color: "#007AFF"; font.pixelSize: 12; font.weight: Font.Medium }

                Text { text: qsTr("Memory (Resident):"); color: FishUI.Theme.disabledTextColor; font.pixelSize: 12 }
                Text { text: (root.processDetails.memoryFormatted || "0 B") + " (" + (root.processDetails.memoryPercent || "0%") + ")"; color: FishUI.Theme.textColor; font.pixelSize: 12 }

                Text { text: qsTr("Virtual Memory:"); color: FishUI.Theme.disabledTextColor; font.pixelSize: 12 }
                Text { text: root.processDetails.virtualMemoryFormatted || "0 B"; color: FishUI.Theme.textColor; font.pixelSize: 12 }

                Text { text: qsTr("Threads:"); color: FishUI.Theme.disabledTextColor; font.pixelSize: 12 }
                Text { text: String(root.processDetails.threads || 1); color: FishUI.Theme.textColor; font.pixelSize: 12 }

                Text { text: qsTr("Priority (Nice):"); color: FishUI.Theme.disabledTextColor; font.pixelSize: 12 }
                Text { text: String(root.processDetails.priority || 0); color: FishUI.Theme.textColor; font.pixelSize: 12 }

                Text { text: qsTr("Disk I/O:"); color: FishUI.Theme.disabledTextColor; font.pixelSize: 12 }
                Text { text: root.processDetails.ioFormatted || "0 KB/s"; color: FishUI.Theme.textColor; font.pixelSize: 12 }
            }
        }

        // Command line box
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4

            Text {
                text: qsTr("Command Line:")
                font.pixelSize: 11
                color: FishUI.Theme.disabledTextColor
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 54
                radius: 8
                color: FishUI.Theme.darkMode ? "#18181A" : "#FFFFFF"
                border.width: 1
                border.color: FishUI.Theme.glassBorderColor

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 6
                    clip: true

                    TextEdit {
                        text: root.processDetails.cmdline || root.processDetails.name || ""
                        font.family: "monospace"
                        font.pixelSize: 11
                        color: FishUI.Theme.textColor
                        readOnly: true
                        selectByMouse: true
                        wrapMode: TextEdit.WrapAnywhere
                    }
                }
            }
        }

        // Actions Bottom Row
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            // Force Kill
            Rectangle {
                implicitHeight: 32
                implicitWidth: killContent.implicitWidth + 20
                radius: height / 2
                color: killMouse.containsMouse ? "#E0382E" : (FishUI.Theme.darkMode ? Qt.rgba(255, 59, 48, 0.15) : Qt.rgba(255, 59, 48, 0.12))
                border.width: 1
                border.color: "#FF3B30"

                Row {
                    id: killContent
                    anchors.centerIn: parent
                    spacing: 6
                    Image {
                        width: 14; height: 14
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:/icons/action-kill.svg"
                        sourceSize: Qt.size(14, 14)
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("Force Kill")
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        color: killMouse.containsMouse ? "#FFFFFF" : "#FF3B30"
                    }
                }

                MouseArea {
                    id: killMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        root.killProcessRequested(root.processDetails.pid)
                        root.close()
                    }
                }
            }

            // End Process
            Rectangle {
                implicitHeight: 32
                implicitWidth: endContent.implicitWidth + 20
                radius: height / 2
                color: endMouse.containsMouse ? "#FF453A" : "#FF3B30"

                Row {
                    id: endContent
                    anchors.centerIn: parent
                    spacing: 6
                    Image {
                        width: 14; height: 14
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:/icons/action-end.svg"
                        sourceSize: Qt.size(14, 14)
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("End Process")
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        color: "#FFFFFF"
                    }
                }

                MouseArea {
                    id: endMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        root.endProcessRequested(root.processDetails.pid)
                        root.close()
                    }
                }
            }

            Item { Layout.fillWidth: true }

            // Close
            Rectangle {
                implicitHeight: 32
                implicitWidth: closeText.implicitWidth + 24
                radius: height / 2
                color: closeMouse.containsMouse
                       ? (FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.12) : Qt.rgba(0, 0, 0, 0.08))
                       : (FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.06) : Qt.rgba(0, 0, 0, 0.05))
                border.width: 1
                border.color: FishUI.Theme.glassBorderColor

                Text {
                    id: closeText
                    anchors.centerIn: parent
                    text: qsTr("Close")
                    font.pixelSize: 12
                    color: FishUI.Theme.textColor
                }

                MouseArea {
                    id: closeMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.close()
                }
            }
        }
    }
}
