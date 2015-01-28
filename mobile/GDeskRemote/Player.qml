import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

import org.gearlux.gdeskremote 1.0

Item {
    id: player
    width: 400
    height: 400

    property bool portrait: parent.height > parent.width
    property RemoteClient client

    Rectangle {
        id: toolbar
        height: portrait ? parent.height / 20 : parent.height / 10
        border.width: 0
        anchors.right: parent.right
        anchors.left: portrait ? parent.left : art.right
        anchors.top: parent.top
        gradient: Gradient {
                GradientStop { position: 0.0; color: portrait ? "#f7f7f7" : "#e6e6e6" }
                GradientStop { position: 1.0; color: portrait ? "#e6e6e6" : "#f7f7f7" }
            }
    }

    Rectangle {
        id: art
        height: portrait ? Math.min(parent.width, parent.height / 1.61) : undefined
        width: portrait ? undefined : Math.min(parent.height, parent.width / 2)
        anchors.top: portrait ? toolbar.bottom : parent.top
        anchors.bottom: portrait ? undefined : parent.bottom
        anchors.left: portrait ? parent.left : undefined
        anchors.right: portrait ? parent.right: undefined
        color: "black"

        Image {
            anchors.fill: parent
            source: client.mode == -1 ? "qrc:///images/icon_512x512.png" : client.albumArt
            fillMode: Image.PreserveAspectFit
        }

    }


    // Controls
    Rectangle {
        id: controls
        anchors.bottom: parent.bottom
        anchors.top: portrait ? art.bottom : toolbar.bottom
        anchors.left: portrait ? parent.left : art.right
        anchors.right: parent.right
        gradient: Gradient {
                GradientStop { position: 0.0; color: "#f7f7f7" }
                GradientStop { position: 1.0; color: "#e6e6e6" }
            }

        ColumnLayout {
            anchors.fill: parent
            anchors.leftMargin: parent.width / 20
            anchors.rightMargin: parent.width / 20

            // Title
            Label {
                font.bold: true
                font.pixelSize: controls.height / 8
                Layout.fillWidth: true
                verticalAlignment: "AlignVCenter"
                horizontalAlignment: "AlignHCenter"
                text: client.title
                opacity: client.mode == -1 ? 0.25 : 1;
            }
            // Artist - Album
            Label {
                font.pixelSize: controls.height / 12
                Layout.fillWidth: true
                verticalAlignment: "AlignVCenter"
                horizontalAlignment: "AlignHCenter"
                text: client.artist +" - " + client.album
                opacity: client.mode == -1 ? 0.25 : 1;
            }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    Layout.fillWidth: true
                    Layout.minimumHeight: controls.height / 5
                    Layout.minimumWidth: Layout.minimumHeight
                    enabled: client.mode >= 0
                    opacity: client.mode == -1 ? 0.25 : 1;
                    style : ButtonStyle {
                        background: Rectangle {
                            anchors.fill: parent
                            border.width: 0
                            color: "transparent"
                            Image {
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectFit
                                source: "qrc:///images/backward.svg"
                            }
                        }
                    }
                    onClicked: client.onPrevious()
                }
                Button {
                    Layout.fillWidth: true
                    Layout.minimumHeight: controls.height / 5
                    Layout.minimumWidth: Layout.minimumHeight
                    enabled: client.mode > 0
                    opacity: client.mode == -1 ? 0.25 : 1;
                    style : ButtonStyle {
                        background: Rectangle {
                            anchors.fill: parent
                            border.width: 0
                            color: "transparent"
                            Image {
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectFit
                                source: client.mode == 2 ? "qrc:///images/pause.svg" : "qrc:///images/play.svg"
                            }
                        }
                    }
                    onClicked: client.onPlay()
                }
                Button {
                    Layout.fillWidth: true
                    Layout.minimumHeight: controls.height / 5
                    Layout.minimumWidth: Layout.minimumHeight
                    enabled: client.mode >= 0
                    opacity: client.mode == -1 ? 0.25 : 1;
                    style : ButtonStyle {
                        background: Rectangle {
                            anchors.fill: parent
                            border.width: 0
                            color: "transparent"
                            Image {
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectFit
                                source: "qrc:///images/forward.svg"
                            }
                        }
                    }
                    onClicked: client.onNext()
                }
            }


            //
            // Volume Controls
            //
            RowLayout {
                Layout.fillWidth: true

                Button {
                    Layout.minimumHeight: controls.height / 10
                    Layout.minimumWidth: controls.height / 10
                    opacity: client.mode == -1 ? 0.25 : 1;
                    style : ButtonStyle {
                        background: Rectangle {
                            anchors.fill: parent
                            border.width: 0
                            color: "transparent"
                            Image {
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectFit
                                source: "qrc:///images/volume_down.svg"
                            }
                        }
                    }
                    onClicked:  volume.value = volume.value - 5

                }
                Slider
                {
                    id: volume
                    value: client.volume
                    Layout.fillWidth: true
                    opacity: client.mode == -1 ? 0.25 : 1;
                    stepSize: 1
                    updateValueWhileDragging: false
                    minimumValue: 0
                    maximumValue: 100
                    onValueChanged: client.volume = value
                    style: SliderStyle {
                           groove: Rectangle {
                               implicitHeight: 6
                               radius: 6
                               color: "#ff5e3d"
                           }
                    }
                }
                Button {
                    Layout.minimumHeight: controls.height / 10
                    Layout.minimumWidth: controls.height / 10
                    opacity: client.mode == -1 ? 0.25 : 1;
                    style : ButtonStyle {
                        background: Rectangle {
                            anchors.fill: parent
                            border.width: 0
                            color: "transparent"
                            Image {
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectFit
                                source: "qrc:///images/volume_up.svg"
                            }
                        }
                    }
                    onClicked:  volume.value = volume.value + 5
                }

            }


            //
            // Shuffle - Repeat
            //
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: controls.height / 10

                Button {
                    id: repeat
                    Layout.minimumWidth: parent.width / 5
                    text: client.repeat == 1 ? "Repeat Song" : "Repeat"
                    opacity: client.mode == -1 ? 0.25 : 1;
                    style: ButtonStyle {
                        background: Rectangle {
                            radius: 5
                            anchors.fill: parent
                            border.width: 0
                            color: client.repeat == 0 ? "transparent" : "#ff5e3d"
                        }
                        label: Label {
                            anchors.fill: parent
                            horizontalAlignment: "AlignHCenter"
                            color: client.repeat == 0 ? "#ff5e3d" : "white"
                            text: repeat.text
                        }
                    }
                    onClicked: client.repeat = client.repeat == 0 ? 2 : client.repeat - 1
                }
                Rectangle {
                    Layout.fillWidth: true
                }

                Button {
                    id: shuffle
                    Layout.minimumWidth: parent.width / 5
                    text: "Shuffle"
                    opacity: client.mode == -1 ? 0.25 : 1;
                    style: ButtonStyle {
                        background: Rectangle {
                            radius: 5
                            anchors.fill: parent
                            border.width: 0
                            color: client.shuffle == 1 ? "#ff5e3d" : "transparent"
                        }
                        label: Label {
                            anchors.fill: parent
                            horizontalAlignment: "AlignHCenter"
                            color: client.shuffle == 1 ? "white" : "#ff5e3d"
                            text: shuffle.text
                        }
                    }
                    onClicked: client.shuffle = client.shuffle == 1 ? 0 : 1;
                }
            }
        }
    }
}

