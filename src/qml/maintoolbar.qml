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

import "Toolbar"

Rectangle {
  id: mainframe

  width: 800
  height: 64
  focus: true
  signal sendBtnCommand(string cmd)

  Toolbar {
      id: toolbar
      objectName: "toolbar"
      anchors.centerIn: parent
      onSendCommand: {
        sendBtnCommand(cmd)
      }

      ToolbarView {
          toolbarItemTitle: qsTr("Open")
<<<<<<< HEAD
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-document-open-folder-icon.svg"
=======
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-document-open-folder-icon.png"
>>>>>>> 3c873d57b4bda5b08cea27d7f7bd43141377be18
          clip: true
      }

      ToolbarView {
          toolbarItemTitle: qsTr("Favorites")
<<<<<<< HEAD
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-bookmark-new-list-icon.svg"
=======
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-favorites-icon.png"
>>>>>>> 3c873d57b4bda5b08cea27d7f7bd43141377be18
          clip: true
      }

      ToolbarView {
        toolbarItemTitle: (true == mediator.fullScr)?qsTr("Full Screen"):qsTr("Normal Screen")
<<<<<<< HEAD
        toolbarItemIcon: (true == mediator.fullScr)?":/toolbar/qml/Toolbar/pics/Actions-view-fullscreen-icon.svg":":/toolbar/qml/Toolbar/pics/Actions-view-close-icon.svg"
=======
        toolbarItemIcon: (true == mediator.fullScr)?":/toolbar/qml/Toolbar/pics/Actions-view-fullscreen-icon.png":":/toolbar/qml/Toolbar/pics/Actions-view-close-icon.png"
>>>>>>> 3c873d57b4bda5b08cea27d7f7bd43141377be18
          clip: true
      }

      ToolbarView {
          toolbarItemTitle: qsTr("Go To Page")
<<<<<<< HEAD
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-go-jump-locationbar-icon.svg"
=======
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-go-jump-locationbar-icon.png"
>>>>>>> 3c873d57b4bda5b08cea27d7f7bd43141377be18
          clip: true
      }

      ToolbarView {
          toolbarItemTitle: qsTr("Zoom")
<<<<<<< HEAD
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-page-zoom-icon.svg"
=======
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-page-zoom-icon.png"
>>>>>>> 3c873d57b4bda5b08cea27d7f7bd43141377be18
          clip: true
      }

      ToolbarView {
          toolbarItemTitle: qsTr("Properties")
<<<<<<< HEAD
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-document-properties-icon.svg"
=======
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-document-properties-icon.png"
>>>>>>> 3c873d57b4bda5b08cea27d7f7bd43141377be18
          clip: true
      }

      ToolbarView {
          toolbarItemTitle: (true == mediator.hlpBck)?qsTr("Help"):qsTr("Back")
<<<<<<< HEAD
          toolbarItemIcon: (true == mediator.hlpBck)?":/toolbar/qml/Toolbar/pics/Actions-help-contents-icon.svg":":/toolbar/qml/Toolbar/pics/Actions-go-previous-view-icon.svg"
=======
          toolbarItemIcon: (true == mediator.hlpBck)?":/toolbar/qml/Toolbar/pics/help-icon.png":":/toolbar/qml/Toolbar/pics/back.png"
>>>>>>> 3c873d57b4bda5b08cea27d7f7bd43141377be18
          clip: true
      }

      ToolbarView {
          toolbarItemTitle: qsTr("About")
<<<<<<< HEAD
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-help-about-icon.svg"
=======
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-help-about-icon.png"
>>>>>>> 3c873d57b4bda5b08cea27d7f7bd43141377be18
          clip: true
      }

      ToolbarView {
          toolbarItemTitle: qsTr("Quit")
<<<<<<< HEAD
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-application-exit-icon.svg"
=======
          toolbarItemIcon: ":/toolbar/qml/Toolbar/pics/Actions-application-exit-icon.png"
>>>>>>> 3c873d57b4bda5b08cea27d7f7bd43141377be18
          clip: true
      }
  }
}
