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

Rectangle {
    id: box
    width: 800
    height: 600
    color:  "transparent"

    signal changeDirectory(int index)
    signal showDocument(string document, int page)

    Image {
      id: background
      anchors.fill: parent
      source: ":/filebrowser/qml/filebrowser/img/wooden-empty-shelf.svg"
    }
    property int rowsCount: 5
    property int headerHeight: 50

    GridView {
        id: pdfPreviewListView
        snapMode: GridView.SnapToRow
        clip: true
        model: pdfPreviewModel
        cellWidth: 120

        function cellHeight() {
          return Math.floor((box.height-box.headerHeight)/box.rowsCount)
        }
        function gridYPos() {
          return box.headerHeight+Math.floor(cellHeight()/4.0)
        }
        function gridXPos() {
          var colsCount = Math.floor(box.width/pdfPreviewListView.cellWidth)
          colsCount = (1>colsCount)?1:colsCount
          return Math.floor((box.width-(colsCount-1)*pdfPreviewListView.cellWidth)/2)
        }
        cellHeight: cellHeight()
        x: gridXPos()
        y: gridYPos()
        width: box.width-2*x
        height: box.height-y

        delegate: Component {
            Rectangle {
                id: previewBox
                property int margin: 5
                width: pdfPreviewListView.cellWidth-margin
                height: fbitem.height
                color: "transparent"
                clip: true
                BorderImage {
                  id: itemPressedImage
                  source: ":/toolbar/qml/Toolbar/pics/toolbaritem_p.svg"
                  width: parent.width
                  height: parent.height
                  border.left: 10; border.top: 10
                  border.right: 10; border.bottom: 10
                  opacity: 0
                }
                MouseArea {
                  id: mouseArea
                  anchors.fill: parent
                  Column {
                    id: fbitem
                    Image {
                      id: previewImage
                      source: model.image
                      anchors.horizontalCenter: parent.horizontalCenter
                      width: 44
                      height: 44
                    }
                    Text {
                      id: title
                      elide: Text.ElideRight
                      wrapMode: Text.Wrap
                      textFormat: Text.PlainText
                      horizontalAlignment: Text.AlignHCenter
                      maximumLineCount: 3
                      text: model.title
                      color: "black"
                      font.pointSize: 11
                      font.bold: true
                      clip: true
                      width: previewBox.width
                    }
                  }
                  onClicked: {
                      if (model.file) {
                          showDocument(model.path, model.page)
                      } else {
                          // Change dir
                          changeDirectory(index);
                      }
                  }
                  states: [
                    State {
                        name: 'pressed'; when: mouseArea.pressed
                        PropertyChanges { target: itemPressedImage; opacity: 1 }
                    }
                  ]
                }
           }
        }
     }
}
