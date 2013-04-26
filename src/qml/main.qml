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
import "content"

Rectangle {
  id: book

  width: 1024
  height: 768

  property alias pages: spinner.nbPages
  property alias index: spinner.currentIndex

  //about properties
  property string version: "0.0"
  property string formats: "none"

  //file browser signals
  signal chDir(int idx)
  signal showDoc(string doc)

  //full screen
  signal fullScreen()
  signal normalScreen()
  property bool fullScr: true

  //goto page signal
  signal gotoPage(int page)

  //zoom page signal
  signal setZoomFactor(int value, int index)
  property int zoomIndex: 0
  ListModel {
    id: zoomFactors
    ListElement { number: "25%" }
    ListElement { number: "50%" }
    ListElement { number: "75%" }
    ListElement { number: "100%" }
    ListElement { number: "125%" }
    ListElement { number: "150%" }
    ListElement { number: "200%" }
    ListElement { number: "Fit Width" }
    ListElement { number: "300%" }
    ListElement { number: "400%" }
  }

  //properties page
  property string filePath: ""
  property string elapsedTime: ""
  property string batteryStatus: "N/A"
  signal setProperties()

  //help handler
  signal showHelp(bool show)

  //quit button
  signal quit()

  Component {
    id: pageDelegate

      Rectangle {
        id: page
        color: "black"
        height: book.height

        Component.onCompleted: page.width = book.width

        Flickable {
          id: flick
          y: anchors.margins
          width: book.width
          height: parent.height-2*anchors.margins
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.margins: 0
          clip: true
          contentWidth: image.width
          contentHeight: image.height
          boundsBehavior: Flickable.StopAtBounds
          flickableDirection: Flickable.HorizontalAndVerticalFlick
          Image {
            id: image
            anchors.centerIn: parent
            cache: false //should not keep in internal cache the pages
            asynchronous: true
            source: "image://pageprovider/"+spinner.currentIndex
          }
          //add zoom handling through pinch
          PinchArea {
            property int oldZoomIndex: -1
            anchors.fill: parent
            onPinchStarted: {
              oldZoomIndex = book.zoomIndex
            }
            onPinchFinished: {
              //do nothing if the cache is not ready
              if (!mediator.cacheReady()) {
                return
              }
              if (1 < pinch.scale) {
                book.zoomIndex = book.zoomIndex+1
                if (9 < book.zoomIndex) {
                  book.zoomIndex = 9
                }
              }
              else if (1 > pinch.scale) {
                book.zoomIndex = book.zoomIndex-1
                if (0 > book.zoomIndex) {
                  book.zoomIndex = 0
                }
              }
              if (oldZoomIndex != book.zoomIndex) {
                selectZoomFactor(book.zoomIndex)
              }
            }
          }
        }
        Text {
          id: pagenumber
          z: 1 //stay on top
          anchors.bottom: page.bottom
          anchors.horizontalCenter: parent.horizontalCenter
          color: "grey"
          text: spinner.currentIndex+1 + "/" + book.pages
        }
      }
    }

  Spinner {
    id: spinner
    anchors.fill: parent
    focus: true
    model: 2 //buffer with book pages
    itemWidth: book.width
    delegate: pageDelegate
  }

  Loader {
    id: mainLoader
    objectName: "mainLoader"
    z: 1
  }

  function onWait() {
    mainLoader.source = "waitdialog.qml"
    mainLoader.width = book.width
    mainLoader.height = book.height
  }

  function onWarning(msg) {
    mainLoader.source = "aboutdialog.qml"
    mainLoader.item.abouttext = "<H3>"+msg+"</H3>"
    mainLoader.width = book.width
    mainLoader.height = book.height
  }

  function selectZoomFactor(index) {
    var idx = parseInt(zoomFactors.get(index).number)
    if (isNaN(idx)) {
      idx = -100 //code for "Fit Width"
    }
    setZoomFactor(idx, index)
  }

  function btnHnd(cmd) {
    console.log(cmd)
    if (qsTr("About") == cmd) {
      mainLoader.source = "aboutdialog.qml"
      mainLoader.item.abouttext = "<H2>tabletReader v"+version+"</H2>"+
                                  qsTr("<H3>e-book reader for touch-enabled devices</H3>")+
                                  qsTr("<H4>Supported formats:</H4>")+
                                  "<H4>"+formats+"</H4>"+
                                  qsTr("<br>Copyright (C) 2013, Bogdan Cristea. All rights reserved.<br>")+
                                  "<i>e-mail: cristeab@gmail.com</i><br><br>"+
                                  qsTr("This program is distributed in the hope that it will be useful, ")+
                                  qsTr("but WITHOUT ANY WARRANTY; without even the implied warranty of ")+
                                  qsTr("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the ")+
                                  qsTr("GNU General Public License for more details.<br><br>")
    mainLoader.width = book.width
    mainLoader.height = book.height
    }
    else if (qsTr("Open") == cmd) {
      mainLoader.source = "filebrowser.qml"
      mainLoader.width = book.width
      mainLoader.height = book.height
      mainLoader.item.changeDirectory.connect(book.chDir)
      mainLoader.item.showDocument.connect(book.showDoc)
    }
    else if (qsTr("Full Screen") == cmd) {
      fullScreen()
      mediator.fullScr = false
    }
    else if (qsTr("Normal Screen") == cmd) {
      normalScreen()
      mediator.fullScr = true
    }
    else if (qsTr("Go To Page") == cmd) {
      mainLoader.source = "gotopage.qml"
      mainLoader.width = book.width
      mainLoader.height = book.height
      mainLoader.item.pages = book.pages
      mainLoader.item.currentPage = spinner.currentIndex+1
      mainLoader.item.setPage.connect(book.gotoPage)
    }
    else if (qsTr("Zoom") == cmd) {
      mainLoader.source = "zoompage.qml"
      mainLoader.width = book.width
      mainLoader.height = book.height
      mainLoader.item.setZoomFactor.connect(book.selectZoomFactor)
      mainLoader.item.zoomModel = zoomFactors
      mainLoader.item.zoomIndex = book.zoomIndex //set zoom index AFTER setting the model
    }
    else if (qsTr("Properties") == cmd) {
      book.setProperties()
      mainLoader.source = "aboutdialog.qml"
      mainLoader.item.abouttext = qsTr("<H3>Document path:<br><i>")+filePath+"</i></H3>"+
      qsTr("<H3>Current page / Number of pages:<br><b>")+(spinner.currentIndex+1)+
      " / "+spinner.nbPages+"</b></H3>"+
      qsTr("<H3>Elapsed time:<br>")+book.elapsedTime+"</H3>"+
      qsTr("<H3>Battery status:<br>")+batteryStatus+"</H3>"
      mainLoader.width = book.width
      mainLoader.height = book.height
    }
    else if (qsTr("Help") == cmd) {
      showHelp(true)
      //hlpBck is set from C++ only if the help file is found
    }
    else if (qsTr("Back") == cmd) {
      showHelp(false)
      mediator.hlpBck = true
    }
    else if (qsTr("Quit") == cmd) {
      quit()
    }
  }

  //toolbar handler
  MouseArea{
    objectName: "toolbararea"
    anchors.top:  parent.top
    width: parent.width
    height: 64
    enabled: true
    onPressAndHold: {
      mainLoader.anchors.top = parent.top
      mainLoader.width = parent.width
      mainLoader.height = 64
      mainLoader.source = "maintoolbar.qml"
      mainLoader.item.sendBtnCommand.connect(btnHnd)
    }
    onClicked: {
      mainLoader.source = ""
    }
  }

  //handlers for left and right
  MouseArea {
    anchors.left: parent.left
    width: 64
    height: parent.height
    onClicked: {
      spinner.prevPage()
    }
  }
  MouseArea {
    anchors.right: parent.right
    width: 64
    height: parent.height
    onClicked: {
      spinner.nextPage()
    }
  }

}

