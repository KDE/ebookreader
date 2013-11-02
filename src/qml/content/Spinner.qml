/****************************************************************************
**
** Copyright (C) 2013, Bogdan Cristea. All rights reserved.
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2, or (at your option)  any later version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
** Street - Fifth Floor, Boston, MA 02110-1301, USA.
**
****************************************************************************/

import QtQuick 1.1

Image {
  property alias model: view.model
  property alias delegate: view.delegate
  property real itemWidth: 30
  property int currentIndex: 0
  property int nbPages: 0

  source: ":/qml/icons/wood-table-top.svg"

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
    if (!mediator.cacheReady()) {
      console.log("not ready")
      return false
    }
    if (currentIndex < (nbPages-1)) {
      view.incrementCurrentIndex()
      currentIndex = currentIndex+1
    }
    return true
  }
  function prevPage() {
    if (!mediator.cacheReady()) {
      console.log("not ready")
      return false
    }
    if (currentIndex > 0) {
      view.decrementCurrentIndex()
      currentIndex = currentIndex-1
    }
    return true
  }

  Keys.onRightPressed: {
    nextPage()
  }
  Keys.onLeftPressed: {
    prevPage()
  }
}
