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
    signal showDocument(string document)

    ListView {
        id: pdfPreviewListView
        anchors.fill: parent
        model: pdfPreviewModel

        delegate: Component {
            Rectangle {
                property int margin: 7
                id: previewBox
                width: box.width
                height:64
                color: ((index%2)?"#f794969a":"#f766676a")
                Image {
                    id: previewImage
                    source: model.image
                    sourceSize {
                        width: height
                        height: height
                    }                    
                    width: 64-margin
                    height: previewBox.height-margin
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: (parent.height - width)/2
                    anchors.topMargin: (parent.height - height)/2
                }
                Text {
                    id: title
                    elide: Text.ElideRight
                    text: model.title
                    color: "white"
                    font.pointSize: 11
                    font.bold: true
                    anchors.top: parent.top
                    anchors.left: previewImage.right
                    anchors.bottom: parent.verticalCenter
                    anchors.leftMargin: 10
                    verticalAlignment: Text.AlignBottom
                }
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    onClicked:  {
                        if (model.file) {
                            showDocument(model.path)
                        } else {
                            // Change dir
                            changeDirectory(index);
                        }
                    }
                }
                states: [
                    State {
                        name: 'pressed'; when: mouseArea.pressed
                        PropertyChanges { target: previewBox; color: "lightgreen" }
                    }
                ]
           }
        }
     }
}
