import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import com.example.dfu 1.0  // Import the registered module

Item {
    id: dfuRoot
    width: 500
    height: 400

//    property alias address: bleAddressField.text
    property alias filePath: firmwarePath.text
    property alias logText: logArea.text
    DfuManager {
          id: dfuManager
      }

    Column {
        anchors.fill: parent
        spacing: 10
        anchors.margins: 10

//        TextField {
//            id: bleAddressField
//            placeholderText: "Enter BLE address (e.g., C1:23:AB:45:78:EF)"
//            width: parent.width
//        }

        FileDialog {
            id: fileDialog
            title: "Select Firmware File"
            nameFilters: ["Firmware Files (*.bin *.img)"]
            onAccepted: {
                firmwarePath.text = fileDialog.fileUrl.toString().replace("file://", "")
            }
        }

        Row {
            spacing: 10
            width: parent.width

            TextField {
                id: firmwarePath
                placeholderText: "Select firmware file"
                width: parent.width - browseButton.width - 20
            }

            Button {
                id: browseButton
                text: "Browse"
                onClicked: fileDialog.open()
            }
        }

        Button {
            text: "Start DFU"
            width: parent.width
            onClicked: {
                if (firmwarePath.text !== "") {
                    logArea.text = "Starting DFU...\n"
                    dfuManager.startDfu(firmwarePath.text)
                } else {
                    logArea.text = "Please fill in all fields.\n"
                }
            }
        }

        TextArea {
            id: logArea
            width: parent.width
            height: parent.height - 200
            readOnly: true
            wrapMode: Text.Wrap
        }
    }

    Component.onCompleted: {
        dfuManager.dfuOutput.connect(function(text) {
            logArea.text += text
        })

        dfuManager.dfuFinished.connect(function() {
            logArea.text += "\nDFU Finished.\n"
        })
    }
}
