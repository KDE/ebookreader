import QtQuick 1.1

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
    // zoom index
    property int zoomIndex: 0
    //model for the reel element
    property alias zoomModel: zoom.model

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
    height: 60

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
        //model: zoomfactors
        delegate: zoomDelegate
        autoClose: false
        index:  zoomIndex
    }
}

