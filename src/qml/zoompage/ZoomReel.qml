import QtQuick 1.0

Item {
    id: container

    // Zoom item dimensions
    property int zoomWidth: width
    property int zoomHeight: height
    // Font properties
    property string fontName: 'Helvetica'
    property int fontSize: 22
    property color fontColor: "#666666"
    // Spacing between items
    property int spacing: 8
    //zoom index
    property int zoomIndex: 0

    signal setZoomFactor(int index)

    property Component itemBackground: Component {
        BorderImage {
            border { top: 8; bottom: 8; left: 8; right: 8 }
            source: "gfx/button.png"
        }
    }
    property Component itemBackgroundPressed: Component {
        BorderImage {
            border { top: 8; bottom: 8; left: 8; right: 8 }
            source: "gfx/button_pressed.png"
        }
    }

    width: 240
    height:  60

    Component {
        id: zoomDelegate
        Button {
            width: container.zoomWidth
            height: container.zoomHeight
            text: number
            fontColor: container.fontColor
            fontName: container.fontName
            fontSize: container.fontSize
            bg: itemBackground
            bgPressed: itemBackgroundPressed
            onClicked: {
                zoom.index = index
                zoom.toggle()
                if (true == zoom.clip) {
                    setZoomFactor(index)
                }
            }
        }
    }

    Reel {
        id: zoom
        width: container.zoomWidth
        height: container.zoomHeight
        model: zoomfactors
        delegate: zoomDelegate
        autoClose: false
        index:  zoomIndex
    }

    ListModel{
        id: zoomfactors
        ListElement { number: "Fit Width" }
        ListElement { number: "25%" }
        ListElement { number: "50%" }
        ListElement { number: "75%" }
        ListElement { number: "100%" }
        ListElement { number: "125%" }
        ListElement { number: "150%" }
        ListElement { number: "200%" }
        ListElement { number: "300%" }
        ListElement { number: "400%" }
    }
}
