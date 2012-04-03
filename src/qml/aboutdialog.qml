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
import "ModalDialog" as Modal

Rectangle {
    id: container

    width: 360
    height: 640
    color: "transparent"

    // Visual is an item that defines some constants for the application
    // look and feel, e.g. the font sizes & colors etc.
    Modal.Visual {
        id: visual
    }

    // Example on using ModalDialog component.
    Modal.ModalDialog {
        id: dialog
        objectName: "aboutDialog"

        // Shown text can be set by modifying "text" property.
        text: "NOT SET"
        // Always remember to define the size for the dialog.
        anchors.fill: parent

        // Demonstrating how one could keep application wide variables saved
        // in one place and use them conveniently all around the application.
        fontName: visual.defaultFontFamily
        fontColor: visual.defaultFontColor
        fontColorButton: visual.defaultFontColorButton
        fontSize: visual.defaultFontSize
        showCancelButton: false
        showOkButton: false
        state: "show"
    }
}
