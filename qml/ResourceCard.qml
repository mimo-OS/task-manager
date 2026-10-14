/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - Resource Card Container
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FishUI 1.0 as FishUI

Rectangle {
    id: root

    property string title: ""
    property string iconSource: ""
    property color headerColor: FishUI.Theme.highlightColor
    property string badgeText: ""
    property color badgeColor: FishUI.Theme.highlightColor
    default property alias content: contentItem.data

    radius: 14
    color: FishUI.Theme.darkMode ? Qt.rgba(36/255, 36/255, 38/255, 0.65)
                                 : Qt.rgba(255/255, 255/255, 255/255, 0.75)
    border.width: 1
    border.color: FishUI.Theme.glassBorderColor

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        // Card Header
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Rectangle {
                width: 26
                height: 26
                radius: 7
                color: Qt.rgba(root.headerColor.r, root.headerColor.g, root.headerColor.b, 0.15)
                visible: root.iconSource.length > 0

                Image {
                    anchors.centerIn: parent
                    width: 16
                    height: 16
                    source: root.iconSource
                    sourceSize: Qt.size(16, 16)
                }
            }

            Text {
                text: root.title
                font.pixelSize: 13
                font.weight: Font.DemiBold
                color: FishUI.Theme.textColor
                Layout.alignment: Qt.AlignVCenter
            }

            Item { Layout.fillWidth: true }

            // Optional Status Badge
            Rectangle {
                visible: root.badgeText.length > 0
                implicitHeight: 22
                implicitWidth: badgeLabel.implicitWidth + 14
                radius: 11
                color: Qt.rgba(root.badgeColor.r, root.badgeColor.g, root.badgeColor.b, 0.16)
                border.width: 1
                border.color: Qt.rgba(root.badgeColor.r, root.badgeColor.g, root.badgeColor.b, 0.3)

                Text {
                    id: badgeLabel
                    anchors.centerIn: parent
                    text: root.badgeText
                    font.pixelSize: 11
                    font.weight: Font.DemiBold
                    color: root.badgeColor
                }
            }
        }

        // Card Body
        Item {
            id: contentItem
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
