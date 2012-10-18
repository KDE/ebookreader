import Qt 4.7
import "flickbook"
 
Rectangle {
  width: 800
  height: 600
  color: "black"

  Loader {
    id: someLoader
    source: "waitpage.qml"
  }

  FlickBook {
    anchors.fill: parent
  }
}

