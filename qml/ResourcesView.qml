/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - Resources View (Real-time Charts)
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
    contentHeight: resourceCol.implicitHeight + 20

    ScrollBar.vertical: ScrollBar {
        active: true
        width: 8
        policy: ScrollBar.AsNeeded
    }

    ColumnLayout {
        id: resourceCol
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 14
        anchors.rightMargin: 14
        anchors.topMargin: 4
        anchors.bottomMargin: 14
        spacing: 12

        // Card 1: CPU History
        ResourceCard {
            Layout.fillWidth: true
            implicitHeight: 180
            title: qsTr("CPU")
            iconSource: "qrc:/icons/metric-cpu.svg"
            headerColor: "#007AFF"
            badgeText: systemMonitor.cpuPercent.toFixed(1) + "%"
            badgeColor: "#007AFF"

            ColumnLayout {
                anchors.fill: parent
                spacing: 8

                // Live Chart
                HistoryChart {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    values: systemMonitor.cpuHistory
                    color: "#007AFF"
                    maxValue: 100.0
                    fillOpacity: 0.22
                    isDark: FishUI.Theme.darkMode
                }

                // Info line and per-core overview
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Text {
                        text: systemMonitor.cpuModel + " (" + systemMonitor.cpuCount + " Cores)"
                        font.pixelSize: 11
                        color: FishUI.Theme.disabledTextColor
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    // Per-core mini meters (up to 8 or 16 shown concisely)
                    Row {
                        spacing: 6
                        Repeater {
                            model: Math.min(8, systemMonitor.cpuPerCore.length)
                            delegate: Row {
                                spacing: 3
                                Text {
                                    text: "C" + index + ":"
                                    font.pixelSize: 9
                                    color: FishUI.Theme.disabledTextColor
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                Rectangle {
                                    width: 32
                                    height: 7
                                    radius: 3.5
                                    color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.1) : Qt.rgba(0, 0, 0, 0.08)
                                    anchors.verticalCenter: parent.verticalCenter

                                    Rectangle {
                                        height: parent.height
                                        radius: parent.radius
                                        width: Math.min(parent.width, Math.max(0, (systemMonitor.cpuPerCore[index] / 100.0) * parent.width))
                                        color: systemMonitor.cpuPerCore[index] > 80 ? "#FF3B30" : "#007AFF"
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // Card 2: Memory & Swap History
        ResourceCard {
            Layout.fillWidth: true
            implicitHeight: 180
            title: qsTr("Memory and Swap")
            iconSource: "qrc:/icons/metric-memory.svg"
            headerColor: "#AF52DE"

            ColumnLayout {
                anchors.fill: parent
                spacing: 8

                // Dual Live Chart
                HistoryChart {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    values: systemMonitor.memoryHistory
                    values2: systemMonitor.swapHistory
                    color: "#AF52DE"
                    color2: "#FF9500"
                    maxValue: 100.0
                    fillOpacity: 0.20
                    isDark: FishUI.Theme.darkMode
                }

                // Legend row
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 18

                    // Memory Legend
                    Row {
                        spacing: 6
                        Rectangle {
                            width: 10; height: 10; radius: 5
                            color: "#AF52DE"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            text: qsTr("Memory: %1").arg(systemMonitor.memFormatted)
                            font.pixelSize: 11
                            color: FishUI.Theme.textColor
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    // Swap Legend
                    Row {
                        spacing: 6
                        Rectangle {
                            width: 10; height: 10; radius: 5
                            color: "#FF9500"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text {
                            text: qsTr("Swap: %1").arg(systemMonitor.swapFormatted)
                            font.pixelSize: 11
                            color: FishUI.Theme.disabledTextColor
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    Item { Layout.fillWidth: true }
                }
            }
        }

        // 2-column row for Network and Disk
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            // Card 3: Network History
            ResourceCard {
                Layout.fillWidth: true
                implicitHeight: 180
                title: qsTr("Network")
                iconSource: "qrc:/icons/metric-network.svg"
                headerColor: "#34C759"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    HistoryChart {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        values: systemMonitor.netRxHistory
                        values2: systemMonitor.netTxHistory
                        color: "#34C759"
                        color2: "#007AFF"
                        autoScale: true
                        fillOpacity: 0.16
                        isDark: FishUI.Theme.darkMode
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 14

                        Row {
                            spacing: 5
                            Rectangle { width: 8; height: 8; radius: 4; color: "#34C759"; anchors.verticalCenter: parent.verticalCenter }
                            Text {
                                text: qsTr("Receiving: %1").arg(systemMonitor.netRxRateFormatted)
                                font.pixelSize: 11
                                color: FishUI.Theme.textColor
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        Row {
                            spacing: 5
                            Rectangle { width: 8; height: 8; radius: 4; color: "#007AFF"; anchors.verticalCenter: parent.verticalCenter }
                            Text {
                                text: qsTr("Sending: %1").arg(systemMonitor.netTxRateFormatted)
                                font.pixelSize: 11
                                color: FishUI.Theme.textColor
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        Item { Layout.fillWidth: true }

                        Text {
                            text: qsTr("Total: %1").arg(systemMonitor.netTotalRxFormatted)
                            font.pixelSize: 10
                            color: FishUI.Theme.disabledTextColor
                        }
                    }
                }
            }

            // Card 4: Disk Activity
            ResourceCard {
                Layout.fillWidth: true
                implicitHeight: 180
                title: qsTr("Disk Activity")
                iconSource: "qrc:/icons/metric-disk.svg"
                headerColor: "#FF9500"

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    HistoryChart {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        values: systemMonitor.diskReadHistory
                        values2: systemMonitor.diskWriteHistory
                        color: "#FF9500"
                        color2: "#00BCD4"
                        autoScale: true
                        fillOpacity: 0.16
                        isDark: FishUI.Theme.darkMode
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 14

                        Row {
                            spacing: 5
                            Rectangle { width: 8; height: 8; radius: 4; color: "#FF9500"; anchors.verticalCenter: parent.verticalCenter }
                            Text {
                                text: qsTr("Read: %1").arg(systemMonitor.diskReadRateFormatted)
                                font.pixelSize: 11
                                color: FishUI.Theme.textColor
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        Row {
                            spacing: 5
                            Rectangle { width: 8; height: 8; radius: 4; color: "#00BCD4"; anchors.verticalCenter: parent.verticalCenter }
                            Text {
                                text: qsTr("Write: %1").arg(systemMonitor.diskWriteRateFormatted)
                                font.pixelSize: 11
                                color: FishUI.Theme.textColor
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        Item { Layout.fillWidth: true }
                    }
                }
            }
        }
    }
}
