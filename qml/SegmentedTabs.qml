/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - Segmented Tabs
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FishUI 1.0 as FishUI

Item {
    id: root

    property int currentIndex: 0
    property var model: []

    signal tabSelected(int index)

    implicitHeight: 34
    implicitWidth: tabRow.implicitWidth + 8

    Rectangle {
        id: bg
        anchors.fill: parent
        radius: height / 2
        color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.08) : Qt.rgba(0, 0, 0, 0.06)
        border.width: 1
        border.color: FishUI.Theme.glassBorderColor
    }

    // Sliding highlight indicator
    Rectangle {
        id: indicator
        height: root.height - 6
        y: 3
        radius: height / 2
        color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.16) : "#FFFFFF"
        border.width: 1
        border.color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.12) : Qt.rgba(0, 0, 0, 0.08)

        // Find geometry of current active item
        property Item currentItem: repeater.itemAt(root.currentIndex)

        x: currentItem ? currentItem.x + 3 : 3
        width: currentItem ? currentItem.width - 6 : 0

        Behavior on x {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
        Behavior on width {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
    }

    Row {
        id: tabRow
        anchors.fill: parent
        anchors.margins: 3
        spacing: 2

        Repeater {
            id: repeater
            model: root.model

            delegate: MouseArea {
                id: tabItem
                width: contentRow.implicitWidth + 24
                height: tabRow.height
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                property bool selected: root.currentIndex === index

                Row {
                    id: contentRow
                    anchors.centerIn: parent
                    spacing: 6

                    Image {
                        id: iconImg
                        width: 16
                        height: 16
                        anchors.verticalCenter: parent.verticalCenter
                        source: modelData.icon || ""
                        sourceSize: Qt.size(16, 16)
                        opacity: tabItem.selected ? 1.0 : (tabItem.containsMouse ? 0.85 : 0.65)
                    }

                    Text {
                        id: labelText
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData.title || ""
                        font.pixelSize: 12
                        font.weight: tabItem.selected ? Font.Medium : Font.Normal
                        color: tabItem.selected
                               ? (FishUI.Theme.darkMode ? "#FFFFFF" : "#1A1A1A")
                               : FishUI.Theme.disabledTextColor
                    }
                }

                onClicked: {
                    root.currentIndex = index
                    root.tabSelected(index)
                }
            }
        }
    }
}
