/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - File Systems View
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
        spacing: 10

        Repeater {
            model: fileSystemModel

            delegate: Rectangle {
                Layout.fillWidth: true
                implicitHeight: 88
                radius: 14
                color: FishUI.Theme.darkMode ? Qt.rgba(36/255, 36/255, 38/255, 0.65)
                                             : Qt.rgba(255/255, 255/255, 255/255, 0.75)
                border.width: 1
                border.color: FishUI.Theme.glassBorderColor

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 14

                    // Storage Drive Icon
                    Rectangle {
                        width: 44
                        height: 44
                        radius: 12
                        color: Qt.rgba(255/255, 149/255, 0/255, 0.15)
                        border.width: 1
                        border.color: Qt.rgba(255/255, 149/255, 0/255, 0.25)

                        Image {
                            anchors.centerIn: parent
                            width: 24
                            height: 24
                            source: "qrc:/icons/tab-storage.svg"
                            sourceSize: Qt.size(24, 24)
                        }
                    }

                    // Disk Details and Usage
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        // Top Row: Mount Point, Device, Type, and Used %
                        RowLayout {
                            Layout.fillWidth: true

                            Text {
                                text: model.mountPoint === "/" ? qsTr("Root File System (/)") : model.mountPoint
                                font.pixelSize: 14
                                font.weight: Font.DemiBold
                                color: FishUI.Theme.textColor
                            }

                            Rectangle {
                                implicitHeight: 18
                                implicitWidth: typeText.implicitWidth + 10
                                radius: 9
                                color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.08) : Qt.rgba(0, 0, 0, 0.05)

                                Text {
                                    id: typeText
                                    anchors.centerIn: parent
                                    text: model.fsType.toUpperCase()
                                    font.pixelSize: 10
                                    font.weight: Font.Medium
                                    color: FishUI.Theme.disabledTextColor
                                }
                            }

                            Text {
                                text: "(" + model.device + ")"
                                font.pixelSize: 11
                                font.family: "monospace"
                                color: FishUI.Theme.disabledTextColor
                            }

                            Item { Layout.fillWidth: true }

                            Text {
                                text: model.usedPercent.toFixed(1) + "% Used"
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                                color: model.usedPercent > 90 ? "#FF3B30" : (model.usedPercent > 80 ? "#FF9500" : "#007AFF")
                            }
                        }

                        // Progress Bar
                        Rectangle {
                            Layout.fillWidth: true
                            height: 8
                            radius: 4
                            color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.1) : Qt.rgba(0, 0, 0, 0.08)

                            Rectangle {
                                height: parent.height
                                radius: parent.radius
                                width: Math.min(parent.width, Math.max(0, (model.usedPercent / 100.0) * parent.width))
                                color: model.usedPercent > 90 ? "#FF3B30" : (model.usedPercent > 80 ? "#FF9500" : "#007AFF")

                                Behavior on width {
                                    NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
                                }
                            }
                        }

                        // Bottom Row: Capacities
                        RowLayout {
                            Layout.fillWidth: true

                            Text {
                                text: qsTr("Used: %1 of %2").arg(model.usedFormatted).arg(model.totalFormatted)
                                font.pixelSize: 11
                                color: FishUI.Theme.disabledTextColor
                            }

                            Item { Layout.fillWidth: true }

                            Text {
                                text: qsTr("Free: %1 available").arg(model.freeFormatted)
                                font.pixelSize: 11
                                color: FishUI.Theme.textColor
                            }
                        }
                    }
                }
            }
        }
    }
}
