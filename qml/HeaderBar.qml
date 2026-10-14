/*
 * Copyright (C) 2026 CutefishOS Team.
 *
 * Cutefish Task Manager - Header Bar
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FishUI 1.0 as FishUI

Item {
    id: root

    property int currentTab: 0
    property alias searchFilter: searchInput.text
    property int filterType: 0
    property bool hasSelectedProcess: false
    property string selectedProcessName: ""

    signal tabChanged(int index)
    signal endProcessClicked()
    signal refreshClicked()
    signal filterTypeSelected(int type)

    implicitHeight: 46

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 4
        anchors.rightMargin: 12
        spacing: 12

        // Segmented Tab Switcher
        SegmentedTabs {
            id: tabs
            currentIndex: root.currentTab
            Layout.alignment: Qt.AlignVCenter
            model: [
                { title: qsTr("Processes"), icon: "qrc:/icons/tab-processes.svg" },
                { title: qsTr("Resources"), icon: "qrc:/icons/tab-resources.svg" },
                { title: qsTr("File Systems"), icon: "qrc:/icons/tab-storage.svg" },
                { title: qsTr("System"), icon: "qrc:/icons/tab-system.svg" }
            ]
            onTabSelected: function(index) {
                root.currentTab = index
                root.tabChanged(index)
            }
        }

        Item { Layout.fillWidth: true }

        // Process search and actions (only visible on Processes tab)
        RowLayout {
            visible: root.currentTab === 0
            spacing: 8
            Layout.alignment: Qt.AlignVCenter

            // Search input box
            Rectangle {
                implicitWidth: 180
                implicitHeight: 30
                radius: height / 2
                color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.08) : Qt.rgba(0, 0, 0, 0.05)
                border.width: 1
                border.color: searchInput.activeFocus ? FishUI.Theme.highlightColor : FishUI.Theme.glassBorderColor

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 8
                    spacing: 6

                    Image {
                        width: 14
                        height: 14
                        source: "qrc:/icons/tab-processes.svg"
                        sourceSize: Qt.size(14, 14)
                        opacity: 0.5
                    }

                    TextInput {
                        id: searchInput
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        font.pixelSize: 12
                        color: FishUI.Theme.textColor
                        selectByMouse: true
                        clip: true

                        Text {
                            anchors.fill: parent
                            text: qsTr("Search...")
                            font: searchInput.font
                            color: FishUI.Theme.disabledTextColor
                            visible: !searchInput.text && !searchInput.activeFocus
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    // Clear button
                    MouseArea {
                        width: 16
                        height: 16
                        visible: searchInput.text.length > 0
                        cursorShape: Qt.PointingHandCursor
                        onClicked: searchInput.text = ""

                        Text {
                            anchors.centerIn: parent
                            text: "×"
                            font.pixelSize: 14
                            color: FishUI.Theme.disabledTextColor
                        }
                    }
                }
            }

            // Filter Selector (All / Mine / Active)
            Rectangle {
                implicitHeight: 30
                implicitWidth: filterRow.implicitWidth + 8
                radius: height / 2
                color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.08) : Qt.rgba(0, 0, 0, 0.05)
                border.width: 1
                border.color: FishUI.Theme.glassBorderColor

                Row {
                    id: filterRow
                    anchors.centerIn: parent
                    spacing: 2

                    Repeater {
                        model: [qsTr("All"), qsTr("Mine"), qsTr("Active")]
                        delegate: MouseArea {
                            width: optText.implicitWidth + 14
                            height: 24
                            cursorShape: Qt.PointingHandCursor

                            Rectangle {
                                anchors.fill: parent
                                radius: height / 2
                                color: root.filterType === index
                                       ? (FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.16) : "#FFFFFF")
                                       : "transparent"
                                border.width: root.filterType === index ? 1 : 0
                                border.color: FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.1) : Qt.rgba(0, 0, 0, 0.08)
                            }

                            Text {
                                id: optText
                                anchors.centerIn: parent
                                text: modelData
                                font.pixelSize: 11
                                font.weight: root.filterType === index ? Font.Medium : Font.Normal
                                color: root.filterType === index
                                       ? FishUI.Theme.textColor
                                       : FishUI.Theme.disabledTextColor
                            }

                            onClicked: {
                                root.filterType = index
                                root.filterTypeSelected(index)
                            }
                        }
                    }
                }
            }

            // End Process Button
            Rectangle {
                id: endProcBtn
                implicitHeight: 30
                implicitWidth: endProcContent.implicitWidth + 18
                radius: height / 2
                color: root.hasSelectedProcess
                       ? (endProcMouse.pressed ? "#D70015" : (endProcMouse.containsMouse ? "#FF453A" : "#FF3B30"))
                       : (FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.05) : Qt.rgba(0, 0, 0, 0.04))
                border.width: 1
                border.color: root.hasSelectedProcess ? "#E0382E" : "transparent"

                opacity: root.hasSelectedProcess ? 1.0 : 0.45

                Row {
                    id: endProcContent
                    anchors.centerIn: parent
                    spacing: 6

                    Image {
                        width: 14
                        height: 14
                        anchors.verticalCenter: parent.verticalCenter
                        source: "qrc:/icons/action-end.svg"
                        sourceSize: Qt.size(14, 14)
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("End Process")
                        font.pixelSize: 12
                        font.weight: Font.Medium
                        color: root.hasSelectedProcess ? "#FFFFFF" : FishUI.Theme.disabledTextColor
                    }
                }

                MouseArea {
                    id: endProcMouse
                    anchors.fill: parent
                    enabled: root.hasSelectedProcess
                    hoverEnabled: true
                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                    onClicked: root.endProcessClicked()
                }
            }
        }

        // Global Refresh Button
        Rectangle {
            id: refreshBtn
            implicitWidth: 30
            implicitHeight: 30
            radius: 15
            color: refreshMouse.containsMouse
                   ? (FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.12) : Qt.rgba(0, 0, 0, 0.08))
                   : (FishUI.Theme.darkMode ? Qt.rgba(255, 255, 255, 0.06) : Qt.rgba(0, 0, 0, 0.04))
            border.width: 1
            border.color: FishUI.Theme.glassBorderColor

            Image {
                id: refreshIcon
                anchors.centerIn: parent
                width: 14
                height: 14
                source: "qrc:/icons/tab-resources.svg"
                sourceSize: Qt.size(14, 14)
                opacity: 0.7

                RotationAnimation on rotation {
                    id: rotAnim
                    from: 0
                    to: 360
                    duration: 600
                    running: false
                }
            }

            MouseArea {
                id: refreshMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    rotAnim.restart()
                    root.refreshClicked()
                }
            }
        }
    }
}
