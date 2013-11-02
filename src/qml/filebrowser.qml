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
    property int rowCount: 5
    property int lineCount: 2
    property real headerHeight: 0.05*box.height

    GridView {
        id: pdfPreviewListView
        snapMode: GridView.SnapToRow
        clip: true
        model: pdfPreviewModel
        cellWidth: 120
        //font size is selected according to window height
        function textPixelSize() {
          if (box.height <= 480) {
            return 12 //px (800 x 480)
          } else if (box.height <= 600) {
            return 14 //px (800 x 600)
          } else {
            return 16 //px (1366 x 768)
          }
        }
        //icon size is selected according to window height
        function iconSize() {
          if (box.height <= 480) {
            return 36 //px
          } else if (box.height <= 600) {
            return 40 //px
          } else {
            return 44 //px
          }
        }
        //correction factor (need in order to align grid rows on the background image)
        function correctionFactor() {
          if (box.height <= 480) {
            return 2 //px
          } else if (box.height <= 600) {
            return 4 //px
          } else {
            return 6 //px
          }
        }
        function gridYPos() {
          var shelfHeight = (box.height-box.headerHeight)/box.rowCount
          var elemHeight = iconSize()+box.lineCount*textPixelSize()
          return Math.round(box.headerHeight+(shelfHeight-elemHeight)/2)
        }
        function cellHeight() {
          return Math.round((box.height-gridYPos())/box.rowCount)
        }
        function gridXPos() {
          var colsCount = Math.round(box.width/pdfPreviewListView.cellWidth)
          colsCount = (1>colsCount)?1:colsCount
          return Math.round((box.width-(colsCount-1)*pdfPreviewListView.cellWidth)/2)
        }
        cellHeight: cellHeight()+correctionFactor()
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
                      width: pdfPreviewListView.iconSize()
                      height: pdfPreviewListView.iconSize()
                    }
                    Text {
                      id: title
                      elide: Text.ElideRight
                      wrapMode: Text.Wrap
                      textFormat: Text.PlainText
                      horizontalAlignment: Text.AlignHCenter
                      maximumLineCount: box.lineCount
                      text: model.title
                      color: "black"
                      font.pixelSize: pdfPreviewListView.textPixelSize()
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
