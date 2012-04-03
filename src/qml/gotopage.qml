/****************************************************************************
**
** Copyright (C) 2011, Bogdan Cristea. All rights reserved.
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

import QtQuick 1.0
import "Core"
import "Core/calculator.js" as CalcEngine
import "ModalDialog" as Comp

Rectangle
{
    id: mainframe
    width: 800
    height: 600
    color: "transparent"

    Comp.Fader {
        id: fadeRect
        anchors.fill: parent
        state: "faded"
    }

    function doOp(operation) { CalcEngine.doOperation(operation) }

    Rectangle {
        id: window

        width: 240; height: 300
        color: "transparent"
        anchors.centerIn: parent

        Item {
            id: main

            width: parent.width; height: parent.height; anchors.centerIn: parent

            Column {
                id: box; spacing: 8

                anchors { fill: parent; topMargin: 6; bottomMargin: 6; leftMargin: 6; rightMargin: 6 }

                Rectangle {
                    width: box.width-3
                    height: 20
                    radius: 10
                    smooth: true
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "white" }
                        GradientStop { position: 1.0; color: "darkgray" }
                    }
                    Text {
                        id: titledarkblue
                        width: box.width
                        height: 16
                        color:  "darkblue"
                        text: qsTr("GoTo Page")
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                        font.pixelSize: 16
                    }
                }

                Display {
                    id: display
                    objectName: "disp"
                    width: box.width-3
                    height: 56
                    text: ""
                }

                Column {
                    id: column; spacing: 6

                    property real h: ((box.height - 90) / 4) - ((spacing * (4 - 1)) / 4)
                    property real w: (box.width / 4) - ((spacing * (4 - 1)) / 4)

                    Grid {
                        id: grid; rows: 4; columns: 3; spacing: 6

                        property real w: (box.width / columns) - ((spacing * (columns - 1)) / columns)

                        Button { width: grid.w; height: column.h; operation: "1"; color: 'blue' }
                        Button { width: grid.w; height: column.h; operation: "2"; color: 'blue' }
                        Button { width: grid.w; height: column.h; operation: "3"; color: 'blue' }

                        Button { width: grid.w; height: column.h; operation: "4"; color: 'blue' }
                        Button { width: grid.w; height: column.h; operation: "5"; color: 'blue' }
                        Button { width: grid.w; height: column.h; operation: "6"; color: 'blue' }

                        Button { width: grid.w; height: column.h; operation: "7"; color: 'blue' }
                        Button { width: grid.w; height: column.h; operation: "8"; color: 'blue' }
                        Button { width: grid.w; height: column.h; operation: "9"; color: 'blue' }

                        Button { width: grid.w; height: column.h; operation: "OK"; color: 'blue' }
                        Button { width: grid.w; height: column.h; operation: "0"; color: 'blue' }
                        Button { width: grid.w; height: column.h; operation: "DEL"; color: 'blue' }
                    }
                }
            }
        }
    }
}
