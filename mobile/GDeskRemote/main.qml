import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import org.gearlux.gdeskremote 1.0

ApplicationWindow {
    title: qsTr("Hello World")
    width: 640
    height: 480
    visible: true

    RemoteClient {
        id: remoteClient
    }

    MainForm {
        anchors.fill: parent
        button1.onClicked: {
            // messageDialog.show(qsTr("Button 1 pressed"))
            remoteClient.onPrevious()
        }
        button2.onClicked: {
            // messageDialog.show(qsTr("Button 2 pressed"))
            remoteClient.onPlay()
        }
        button3.onClicked: {
            // messageDialog.show(qsTr("Button 3 pressed"))
            remoteClient.onNext()
        }
    }

    MessageDialog {
        id: messageDialog
        title: qsTr("May I have your attention, please?")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
    }
}
