import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    id: item1
    width: 640
    height: 480

    property alias button3: button3
    property alias button2: button2
    property alias button1: button1

    RowLayout {
        id: rowLayout1
        spacing: 15
        anchors.verticalCenterOffset: 185
        anchors.horizontalCenterOffset: 0
        scale: 1
        anchors.centerIn: parent

        Button {
            id: button1
            text: qsTr("Previous")
        }

        Button {
            id: button2
            text: qsTr("Play")
        }

        Button {
            id: button3
            text: qsTr("Next")
        }
    }

    Slider {
        id: sliderHorizontal1
        y: 444
        height: 28
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
    }

    Image {
        id: image1
        x: 199
        y: 119
        width: 243
        height: 243
        source: "qrc:/qtquickplugin/images/template_image.png"
    }

    ListView {
        id: listView1
        x: 160
        y: 80
        width: 110
        height: 160
        model: ListModel {
            ListElement {
                name: "Grey"
                colorCode: "grey"
            }

            ListElement {
                name: "Red"
                colorCode: "red"
            }

            ListElement {
                name: "Blue"
                colorCode: "blue"
            }

            ListElement {
                name: "Green"
                colorCode: "green"
            }
        }
        opacity: 0
        delegate: Item {
            x: 5
            width: 80
            height: 40
            Row {
                id: row1
                Rectangle {
                    width: 40
                    height: 40
                    color: colorCode
                }

                Text {
                    text: name
                    font.bold: true
                    anchors.verticalCenter: parent.verticalCenter
                }
                spacing: 10
            }
        }
    }
}
