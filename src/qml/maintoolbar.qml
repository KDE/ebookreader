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

import Qt 4.7

import "Toolbar"

Rectangle {
    width: 800
    height: 64
    property bool hlpBck: true

    Toolbar {
        id: toolbar
        objectName: "toolbar"
        anchors.centerIn: parent

        ToolbarView {
            toolbarItemTitle: qsTr("Open")
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-document-open-folder-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: qsTr("Full Screen")
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-view-fullscreen-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: qsTr("Go To Page")
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-go-jump-locationbar-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: qsTr("Zoom")
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-page-zoom-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: qsTr("Properties")
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-document-properties-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: (true == hlpBck)?qsTr("Help"):qsTr("Back")
            toolbarItemIcon: (true == hlpBck)?":/toolbar/qml/Toolbar/pics/help-icon.png":":/toolbar/qml/Toolbar/pics/back.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: qsTr("About")
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-help-about-icon.png"
            clip: true
        }

        ToolbarView {
            toolbarItemTitle: qsTr("Exit")
            toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-application-exit-icon.png"
            clip: true
        }
    }
}
