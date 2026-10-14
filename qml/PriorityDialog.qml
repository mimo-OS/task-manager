/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - Process Priority Dialog
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FishUI 1.0 as FishUI

Popup {
    id: root

    property int pid: 0
    property string processName: ""
    property int currentPriority: 0

    signal priorityChanged(int pid, int niceValue)

    width: 360
    implicitHeight: contentCol.implicitHeight + 36
    modal: true
    focus: true
    anchors.centerIn: parent

    background: Rectangle {
        radius: FishUI.Theme.hugeRadius
        color: FishUI.Theme.darkMode ? Qt.rgba(28/255, 28/255, 30/255, 0.95)
                                     : Qt.rgba(248/255, 248/255, 250/255, 0.96)
        border.width: 1
        border.color: FishUI.Theme.glassBorderColor
    }

    contentItem: ColumnLayout {
        id: contentCol
        spacing: 14

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Image {
                width: 18; height: 18
                source: "qrc:/icons/action-priority.svg"
                sourceSize: Qt.size(18, 18)
            }

            Text {
                text: qsTr("Change Priority")
                font.pixelSize: 14
                font.weight: Font.DemiBold
                color: FishUI.Theme.textColor
            }

            Item { Layout.fillWidth: true }

            MouseArea {
                width: 22; height: 22
                cursorShape: Qt.PointingHandCursor
                onClicked: root.close()
                Text {
                    anchors.centerIn: parent
                    text: "×"
                    font.pixelSize: 16
                    color: FishUI.Theme.disabledTextColor
                }
            }
        }

        Text {
            text: qsTr("Set execution priority for ") + root.processName + " (PID " + root.pid + ")"
            font.pixelSize: 12
            color: FishUI.Theme.disabledTextColor
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        // Preset buttons
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 6

            Repeater {
                model: [
                    { label: qsTr("Very High (-20)"), value: -20 },
                    { label: qsTr("High (-10)"), value: -10 },
                    { label: qsTr("Normal (0)"), value: 0 },
                    { label: qsTr("Low (10)"), value: 10 },
                    { label: qsTr("Very Low (19)"), value: 19 }
                ]

                delegate: Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 32
                    radius: 8
                    color: optMouse.containsMouse
                           ? (FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.12) : Qt.rgba(0, 0, 0, 0.08))
                           : (root.currentPriority === modelData.value
                              ? Qt.rgba(0, 122/255, 255/255, 0.15)
                              : (FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.04) : Qt.rgba(0, 0, 0, 0.03)))
                    border.width: 1
                    border.color: root.currentPriority === modelData.value
                                  ? "#007AFF"
                                  : FishUI.Theme.glassBorderColor

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12

                        Text {
                            text: modelData.label
                            font.pixelSize: 12
                            font.weight: root.currentPriority === modelData.value ? Font.Medium : Font.Normal
                            color: root.currentPriority === modelData.value ? "#007AFF" : FishUI.Theme.textColor
                        }

                        Item { Layout.fillWidth: true }

                        Text {
                            visible: root.currentPriority === modelData.value
                            text: "✓"
                            font.pixelSize: 12
                            color: "#007AFF"
                        }
                    }

                    MouseArea {
                        id: optMouse
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            root.priorityChanged(root.pid, modelData.value)
                            root.close()
                        }
                    }
                }
            }
        }
    }
}
