/****************************************************************************
**
** Copyright (C) 2012, Bogdan Cristea. All rights reserved.
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
import "PopupMenu" as Comp
import "ModalDialog" as Modal

Rectangle
{
    id: mainframe
    width: 800
    height: 600
    color: "transparent"

    Modal.Fader {
        id: fadeRect
        anchors.fill: parent
        state: "faded"
    }

    Item {
        id: container
        width: 250
        height: 325
        anchors.centerIn: parent
        Comp.PopupList {
            id: list
            anchors.fill: parent
            objectName: "popuplist"
        }
    }
}
