import QtQuick 1.1

Rectangle {
    property int margin: 20
    property int contentWidth: toolbarItemText.width + margin

    height: parent.minimumItemHeight
    color: "black"

    signal doAction(string btnText)

    // Toolbar item background
    BorderImage {
        id: toolbarItemImage
        source: ":/toolbar/qml/Toolbar/pics/toolbaritem_gray.png"
        width: parent.width
        height: parent.height
        border.left: 10; border.top: 10
        border.right: 10; border.bottom: 10
    }

    // Selection highlight
    BorderImage {
        id: toolbarItemPressedImage
        source: ":/toolbar/qml/Toolbar/pics/toolbaritem_p.png"
        width: parent.width
        height: parent.height
        border.left: 10; border.top: 10
        border.right: 10; border.bottom: 10

        opacity: 0
    }

    // Icon and title
    Column {
        anchors.centerIn: parent

        Image {
            id: toolbarItemIcon
            width: 40
            height: 40

            property string iconSource: toolbar.views[index].toolbarItemIcon

            source: iconSource
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            id: toolbarItemText

            anchors.horizontalCenter: parent.horizontalCenter
            text: toolbar.views[index].toolbarItemTitle
            color: "lightgray"
            style: "Raised"
            font.pointSize: 11
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: toolbarItemImage
        onClicked: {
            doAction(toolbarItemText.text)
        }
    }

    states: [
        State {
            name: 'pressed'; when: mouseArea.pressed
            PropertyChanges { target: toolbarItemPressedImage; opacity: 1 }
        }
    ]
}
