import QtQuick 2.0

Rectangle {
    width: 600
    height: 900
    color: "#f0f4f8" // Light background color

    Header {
        id: header
        anchors.top: parent.top
        width: parent.width - 10
        anchors.horizontalCenter: parent.horizontalCenter
        headerText: "Characteristics list"
    }

    Dialog {
        id: info
        anchors.centerIn: parent
        visible: true
        dialogText: "Scanning for characteristics..."
    }

    Connections {
        target: device
        onCharacteristicsUpdated: {
            menu.menuText = "Back"
            if (characteristicview.count === 0) {
                info.dialogText = "No characteristic found"
                info.busyImage = false
            } else {
                info.visible = false
                info.busyImage = true
            }
        }

        onDisconnected: {
            pageLoader.source = "main.qml"
        }
    }

    ListView {
        id: characteristicview
        width: parent.width
        height: parent.height - header.height - menu.menuHeight
        anchors.top: header.bottom
        anchors.topMargin: 20
        model: device.characteristicList
        clip: true

        delegate: Rectangle {
            id: characteristicbox
            height: 220
            width: parent.width - 20
            color: "#e3efff"
            border.width: 1
            border.color: "#4a90e2"
            radius: 8
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 10


            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 6

                Label {
                    textContent: modelData.characteristicName
                    font.bold: true
                    font.pointSize: 16
                    color: "#2a2a2a"
                }

                Label {
                    textContent: modelData.characteristicUuid
                    font.pointSize: 12
                    color: "#555"
                }

                Label {
                    textContent: "Value: " + modelData.characteristicValue
                    font.pointSize: 12
                    color: "#333"
                }

                Label {
                    textContent: "Handlers: " + modelData.characteristicHandle
                    font.pointSize: 12
                    color: "#444"
                }

                Label {
                    textContent: modelData.characteristicPermission
                    font.pointSize: 12
                    color: "#777"
                }
            }
        }
    }
}
