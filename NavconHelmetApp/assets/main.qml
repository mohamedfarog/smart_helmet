/***************************************************************************
**
** Copyright (C) 2013 BlackBerry Limited. All rights reserved.
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtBluetooth module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0

Rectangle {
    id: back
    width: 600
    height: 900
    property bool deviceState: device.state
    property string dialogText: ""

    onDeviceStateChanged: {
        if (!device.state)
            infoBox.visible = false;
    }

    Header {
        id: header
        anchors.top: parent.top
        width: parent.width - 10
        anchors.horizontalCenter: parent.horizontalCenter
        height: 60
        headerText: "Start Discovery"
    }

    Rectangle {
        id: infoBox
        anchors.centerIn: parent
        visible: false
        width: parent.width * 0.6
        height: parent.height * 0.15
        radius: 10
        color: "#ecf0f1"
        border.color: "#3498db"
        border.width: 2

        Text {
            anchors.fill: parent
            anchors.margins: 20
            text: back.dialogText.length > 0 ? back.dialogText : "Information"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "#34495e"
            font.pixelSize: 16
            font.bold: true
        }
    }

    ListView {
        id: theListView
        width: parent.width - 10
        anchors.horizontalCenter: parent.horizontalCenter
        clip: true
        anchors.topMargin: 13
        anchors.top: header.bottom
        anchors.bottom: connectToggle.top
        model: device.devicesList

        delegate: Rectangle {
            id: box
            height:90
            width: parent.width
            color: "lightsteelblue"
            border.width: 2
            border.color: "black"
            radius: 5

            Component.onCompleted: {
                infoBox.visible = false;
                header.headerText = "Select a device";
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    device.scanServices(modelData.deviceAddress);
                    pageLoader.source = "Services.qml"
                }
            }

            Label {
                id: deviceName
                textContent: modelData.deviceName
                anchors.top: parent.top
                anchors.topMargin: 5
            }

            Label {
                id: deviceAddress
                textContent: modelData.deviceAddress
                font.pointSize: deviceName.font.pointSize*0.7
                anchors.bottom: box.bottom
                anchors.bottomMargin: 5
            }
        }
    }

    Menu {
        id: connectToggle
        menuWidth: parent.width - 10
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: menu.top
        menuText: { if (device.devicesList.length)
                        visible = true
                    else
                        visible = false
                    if (device.useRandomAddress)
                        "Address type: Random"
                    else
                        "Address type: Public"
        }

        onButtonClick: device.useRandomAddress = !device.useRandomAddress;
    }

    Menu {
        id: menu
        anchors.bottom: parent.bottom
        menuWidth: parent.width - 10
        menuHeight: (parent.height/6)
        anchors.horizontalCenter: parent.horizontalCenter
        menuText: device.update
        onButtonClick: {
            device.startDeviceDiscovery();
            // if startDeviceDiscovery() failed device.state is not set
            if (device.state) {
                back.dialogText = "Searching...";
                infoBox.visible = true;
            }
        }
    }

    Loader {
        id: pageLoader
        anchors.fill: parent
    }
}
