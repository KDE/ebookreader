import QtQuick 1.1

Rectangle {
  property alias model: view.model
  property alias delegate: view.delegate
  property real itemWidth: 30
  property int currentIndex: 0
  property int nbPages: 0
  color: "black"

  clip: true

  PathView {
    id: view
    objectName: "view"
    anchors.fill: parent

    pathItemCount: 1 //show one page at a time
    preferredHighlightBegin: 0.5
    preferredHighlightEnd: 0.5
    dragMargin: view.width/2
    interactive: false

    path: Path {
              startX: -itemWidth
              startY: view.height/2
              PathLine { x: (view.pathItemCount+1)*itemWidth
                         y: view.height/2
              }
        }
  }

  function nextPage() {
    if (false == mediator.cacheReady()) {
      console.log("not ready")
      return
    }
    if (currentIndex < (nbPages-1)) {
      view.incrementCurrentIndex()
      currentIndex = currentIndex+1
    }
  }
  function prevPage() {
    if (false == mediator.cacheReady()) {
      console.log("not ready")
      return
    }
    if (currentIndex > 0) {
      view.decrementCurrentIndex()
      currentIndex = currentIndex-1
    }
  }

  Keys.onRightPressed: {
    nextPage()
  }
  Keys.onLeftPressed: {
    prevPage()
  }
}
