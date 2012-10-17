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
        clip: true
        anchors.fill: parent
        anchors.margins: 15
        Image {
          anchors.horizontalCenter: parent.horizontalCenter
          source: modelData
        }
      }
    }
  }

  ListView {
    id: list
    anchors.fill: parent
    model: dataModel
    delegate: pageDelegate
    orientation: ListView.Horizontal
    snapMode: ListView.SnapOneItem
  }
}
