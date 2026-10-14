/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - System Info Overview
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FishUI 1.0 as FishUI
import Cutefish.TaskManager 1.0

ScrollView {
    id: root
    clip: true
    contentWidth: availableWidth

    ScrollBar.vertical: ScrollBar {
        active: true
        width: 8
        policy: ScrollBar.AsNeeded
    }

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 14
        anchors.rightMargin: 14
        anchors.topMargin: 4
        anchors.bottomMargin: 14
        spacing: 14

        // Top Banner Card
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 120
            radius: 14
            color: FishUI.Theme.darkMode ? Qt.rgba(36/255, 36/255, 38/255, 0.65)
                                         : Qt.rgba(255/255, 255/255, 255/255, 0.75)
            border.width: 1
            border.color: FishUI.Theme.glassBorderColor

            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20

                // Cutefish App/System Icon with Glow
                Item {
                    width: 84
                    height: 84

                    Image {
                        anchors.fill: parent
                        source: "qrc:/icons/cutefish-taskmanager.svg"
                        sourceSize: Qt.size(84, 84)
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4

                    Text {
                        text: systemMonitor.osName
                        font.pixelSize: 20
                        font.weight: Font.Bold
                        color: FishUI.Theme.textColor
                    }

                    Text {
                        text: "mimo OS Desktop Environment"
                        font.pixelSize: 13
                        font.weight: Font.Medium
                        color: "#007AFF"
                    }

                    Text {
                        text: qsTr("Hostname: %1 • Kernel %2").arg(systemMonitor.hostname).arg(systemMonitor.kernelRelease)
                        font.pixelSize: 12
                        color: FishUI.Theme.disabledTextColor
                    }
                }
            }
        }

        // Hardware Specs Card
        ResourceCard {
            Layout.fillWidth: true
            implicitHeight: 220
            title: qsTr("Hardware & Specifications")
            iconSource: "qrc:/icons/tab-system.svg"
            headerColor: "#AF52DE"

            GridLayout {
                anchors.fill: parent
                columns: 2
                rowSpacing: 12
                columnSpacing: 20

                // Processor
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    Rectangle {
                        width: 36; height: 36; radius: 9
                        color: Qt.rgba(0, 122/255, 255/255, 0.12)
                        Image {
                            anchors.centerIn: parent; width: 20; height: 20
                            source: "qrc:/icons/metric-cpu.svg"
                            sourceSize: Qt.size(20, 20)
                        }
                    }
                    ColumnLayout {
                        spacing: 2
                        Text { text: qsTr("Processor"); font.pixelSize: 11; color: FishUI.Theme.disabledTextColor }
                        Text { text: systemMonitor.cpuModel; font.pixelSize: 13; font.weight: Font.Medium; color: FishUI.Theme.textColor; elide: Text.ElideRight }
                        Text { text: qsTr("%1 Cores • %2").arg(systemMonitor.cpuCount).arg(systemMonitor.cpuFreq); font.pixelSize: 11; color: FishUI.Theme.disabledTextColor }
                    }
                }

                // Memory
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    Rectangle {
                        width: 36; height: 36; radius: 9
                        color: Qt.rgba(175/255, 82/255, 222/255, 0.12)
                        Image {
                            anchors.centerIn: parent; width: 20; height: 20
                            source: "qrc:/icons/metric-memory.svg"
                            sourceSize: Qt.size(20, 20)
                        }
                    }
                    ColumnLayout {
                        spacing: 2
                        Text { text: qsTr("Memory (RAM)"); font.pixelSize: 11; color: FishUI.Theme.disabledTextColor }
                        Text { text: systemMonitor.formatBytes(systemMonitor.memTotal); font.pixelSize: 13; font.weight: Font.Medium; color: FishUI.Theme.textColor }
                        Text { text: qsTr("Swap: %1").arg(systemMonitor.formatBytes(systemMonitor.swapTotal)); font.pixelSize: 11; color: FishUI.Theme.disabledTextColor }
                    }
                }

                // System Uptime
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    Rectangle {
                        width: 36; height: 36; radius: 9
                        color: Qt.rgba(52/255, 199/255, 89/255, 0.12)
                        Image {
                            anchors.centerIn: parent; width: 20; height: 20
                            source: "qrc:/icons/action-resume.svg"
                            sourceSize: Qt.size(20, 20)
                        }
                    }
                    ColumnLayout {
                        spacing: 2
                        Text { text: qsTr("System Uptime"); font.pixelSize: 11; color: FishUI.Theme.disabledTextColor }
                        Text { text: systemMonitor.uptimeFormatted; font.pixelSize: 13; font.weight: Font.Medium; color: FishUI.Theme.textColor }
                        Text { text: qsTr("Running smoothly"); font.pixelSize: 11; color: "#34C759" }
                    }
                }

                // Storage Partitions Count
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    Rectangle {
                        width: 36; height: 36; radius: 9
                        color: Qt.rgba(255/255, 149/255, 0/255, 0.12)
                        Image {
                            anchors.centerIn: parent; width: 20; height: 20
                            source: "qrc:/icons/tab-storage.svg"
                            sourceSize: Qt.size(20, 20)
                        }
                    }
                    ColumnLayout {
                        spacing: 2
                        Text { text: qsTr("Storage Partitions"); font.pixelSize: 11; color: FishUI.Theme.disabledTextColor }
                        Text { text: qsTr("%1 Mounted Filesystems").arg(fileSystemModel.count); font.pixelSize: 13; font.weight: Font.Medium; color: FishUI.Theme.textColor }
                        Text { text: qsTr("Monitored via statvfs"); font.pixelSize: 11; color: FishUI.Theme.disabledTextColor }
                    }
                }
            }
        }
    }
}
