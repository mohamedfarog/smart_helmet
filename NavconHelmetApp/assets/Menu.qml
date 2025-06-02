import QtQuick 2.0

Rectangle {
    id: root
    property real menuWidth: 100
    property real menuHeight: 40
    property string menuText: "Search"
    signal buttonClick()

    width: menuWidth
    height: menuHeight
    radius: 8
    color: "#2c3e50"           // dark blue-gray background
    border.color: "#34495e"    // slightly lighter border
    border.width: 1
    smooth: true

    Rectangle {
        id: buttonBg
        anchors.fill: parent
        radius: root.radius
        color: mouseArea.pressed ? "#1a252f" : "#34495e"
        border.color: mouseArea.pressed ? "#1a252f" : "#3b5368"
        border.width: 1
    }

    Text {
        id: searchText
        anchors.centerIn: parent
        text: menuText
        font.pixelSize: 16
        font.bold: true
        color: "#ecf0f1"    // light gray text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideMiddle
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        onPressed: {
            // visually handled by buttonBg color change
        }
        onReleased: {
            // visually handled by buttonBg color change
        }
        onClicked: {
            root.buttonClick()
        }
    }
}
