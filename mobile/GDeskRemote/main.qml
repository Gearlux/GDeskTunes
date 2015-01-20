import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import org.gearlux.gdeskremote 1.0

ApplicationWindow {
    id: window
    title: qsTr("Hello World")
    width: 640
    height: 960
    visible: true

    RemoteClient {
        id: remoteClient
    }

    Rectangle {
        id: bg
        color: "#E0E0E0"
        anchors.fill: parent
    }

    StackView {
        id: stackView
        anchors.fill: parent
        focus: true

        initialItem: Item {
            width: parent.width
            height: parent.height

            Player {
                anchors.fill: parent
                client: remoteClient
            }
        }
    }
}
