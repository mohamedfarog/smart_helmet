import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: root
    width: 600
    height: 900
    color: "#f0f4f8"

    Component.onCompleted: {
        if (device.controllerError) {
            info.visible = false
            menu.menuText = device.update
        }
    }

    // --- Top Buttons Row ---
    Rectangle {
        id: firmwareUpdateBox
        width: root.width / 2 - 20
        height: 50
        color: "#4caf50"
        radius: 10
        border.color: "#388e3c"
        border.width: 1
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 15
        anchors.leftMargin: 10

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: firmwareUpdateBox.color = "#66bb6a"
            onExited: firmwareUpdateBox.color = "#4caf50"
            onClicked: {
                pageLoader.source = "Dfu.qml"
                console.log("Firmware Update clicked")
            }
        }

        Text {
            anchors.centerIn: parent
            text: "Firmware Update"
            font.pixelSize: 16
            font.bold: true
            color: "white"
        }
    }

    Rectangle {
        id: firmwareRollback
        width: root.width / 2 - 20
        height: 50
        color: "#ffb300"
        radius: 10
        border.color: "#f57c00"
        border.width: 1
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 15
        anchors.rightMargin: 15

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: firmwareRollback.color = "#ffc947"
            onExited: firmwareRollback.color = "#ffb300"
            onClicked: {
                pageLoader.source = "Dfu.qml"
                console.log("Firmware Rollback clicked")
            }
        }

        Text {
            anchors.centerIn: parent
            text: "Rollback Firmware"
            font.pixelSize: 16
            font.bold: true
            color: "white"
        }
    }

    // --- Header ---
    Rectangle {
        id: header
        anchors.top: firmwareUpdateBox.bottom
//        anchors.left: parent.left
//        anchors.right: parent.right
        anchors.topMargin: 20
        width: parent.width - 10
        anchors.horizontalCenter: parent.horizontalCenter
        height: 50
        color: "#e3f2fd"
        border.color: "#90caf9"
        border.width: 1
        radius: 8

        Text {
            anchors.centerIn: parent
            text: "Services List"
            font.pixelSize: 20
            font.bold: true
            color: "#1e88e5"
        }
    }

    // --- Info Dialog ---
    Rectangle {
        id: info
        visible: true
        width: 240
        height: 50
        radius: 8
        color: "#ffffff"
        border.color: "#b0bec5"
        border.width: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: header.bottom
        anchors.topMargin: 15

        Text {
            id: infoText
            text: "Scanning for services..."
            anchors.centerIn: parent
            font.pixelSize: 14
            color: "#37474f"
        }

        property alias dialogText: infoText.text
    }

    // --- Services List ---
    ListView {
        id: servicesview
        anchors.top: info.visible ? info.bottom : header.bottom
        anchors.topMargin: 20
//        anchors.left: parent.left
//        anchors.right: parent.right
        anchors.bottom: menu.top
        anchors.bottomMargin: 15
        width: parent.width - 10
        anchors.horizontalCenter: parent.horizontalCenter
        model: device.servicesList
        clip: true
        spacing: 10

        delegate: Rectangle {
            id: servicebox
            width: parent.width - 30
            height: 90
            radius: 10
            color: "#ffffff"
            border.color: "#cfd8dc"
            border.width: 1
            anchors.horizontalCenter: parent.horizontalCenter

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: servicebox.color = "#f1f8e9"
                onExited: servicebox.color = "#ffffff"
                onClicked: {
                    device.connectToService(modelData.serviceUuid)
                    pageLoader.source = "Characteristics.qml"
                }
            }

            Column {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 4

                Label {
                    text: modelData.serviceName
                    font.pixelSize: 16
                    font.bold: true
                    color: "#263238"
                    elide: Label.ElideRight
                }

                Label {
                    text: modelData.serviceType
                    font.pixelSize: 13
                    color: "#546e7a"
                    elide: Label.ElideRight
                }

                Label {
                    text: modelData.serviceUuid
                    font.pixelSize: 12
                    color: "#78909c"
                    elide: Label.ElideMiddle
                }
            }

            Component.onCompleted: {
                info.visible = false
            }
        }
    }

    // --- Bottom Menu (Custom Styled) ---
    Rectangle {
        id: menu
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: root.height / 6
        color: "#eceff1"
        border.color: "#b0bec5"
        border.width: 1

        Text {
            id: menuLabel
            anchors.centerIn: parent
            text: device.update
            font.pixelSize: 18
            color: "#37474f"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                device.disconnectFromDevice()
                pageLoader.source = "main.qml"
                device.update = "Search"
            }
        }

        property alias menuText: menuLabel.text
        property alias menuWidth: menu.width
        property alias menuHeight: menu.height
    }

    // --- Connections ---
    Connections {
        target: device

        onServicesUpdated: {
            if (servicesview.count === 0)
                info.dialogText = "No services found"
            else
                info.visible = false
        }

        onDisconnected: {
            pageLoader.source = "main.qml"
        }
    }
}
