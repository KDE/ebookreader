import Qt 4.7

Item {
  anchors.fill: parent
  id: book

  Component {
    id: pageDelegate

    Rectangle {
      id: page
      color: "black"
      border.color: "gray"
      border.width: 1
      height: parent.height
      width: 800

      Flickable {
        anchors.fill: parent
        anchors.margins: 15
        contentWidth: image.width
        contentHeight: image.height
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.HorizontalAndVerticalFlick
        clip: true
        Image {
          id: image
          //anchors.horizontalCenter: parent.horizontalCenter
          source: modelData
        }
      }
    }
  }

  ListView {
    id: list
    objectName: "list"
    anchors.fill: parent
    model: dataModel
    delegate: pageDelegate
    orientation: ListView.Horizontal
    snapMode: ListView.SnapOneItem
    focus: true
    cacheBuffer: 0
    currentIndex: 1
  }
}
