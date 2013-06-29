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
import "zoompage" as Comp
import "ModalDialog" as Modal

Rectangle
{
    id: mainframe
    width: 800
    height: 600
    color: "transparent"

    property alias zoomIndex: zoomReel.zoomIndex
    signal setZoomFactor(int index)
    property alias zoomModel: zoomReel.zoomModel

    Modal.Fader {
        id: fadeRect
        anchors.fill: parent
        state: "faded"
    }

    Rectangle {
        id: container

        width: 140
        height: 260
        color: "transparent"
        anchors.centerIn: parent

        Rectangle {
            width: container.width
            height: 20
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -48
            radius: 10
            smooth: true
            gradient: Gradient {
                GradientStop { position: 0.0; color: "white" }
                GradientStop { position: 1.0; color: "darkgray" }
            }
            Text {
                id: titledarkblue
                width: container.width
                height: 16
                color:  "darkblue"
                text: qsTr("Zoom Factor")
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                font.pixelSize: 16
            }
        }

        Comp.ZoomReel {
            id: zoomReel
            objectName: "zoomreel"
            width: parent.width
            anchors.verticalCenter: parent.verticalCenter
            onSetZoomFactor: {
              mainframe.setZoomFactor(index)
            }
        }
    }
}
