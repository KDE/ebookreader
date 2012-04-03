import QtQuick 1.0

BorderImage {
    id: image

    signal setPage(string value)
    property alias text : displayText.text

    source: "images/display.png"
    border { left: 10; top: 10; right: 10; bottom: 10 }

    Text {
        id: displayText
        anchors {
            right: parent.right; verticalCenter: parent.verticalCenter; verticalCenterOffset: -1
            rightMargin: 6; left: parent.left
        }
        font.pixelSize: parent.height * .6; text: "0"; horizontalAlignment: Text.AlignHCenter; elide: Text.ElideRight
        color: "#343434"; smooth: true; font.bold: true
    }
}
