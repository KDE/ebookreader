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

#ifndef WINDOW_H
#define WINDOW_H

#include <QElapsedTimer>
#include <QtDeclarative>
#include <QDeclarativeView>
#include "config.h"

#ifndef NO_QTMOBILITY
#include <qmobilityglobal.h>
#endif
#include "pageprovider.h"
#include "qmlcppmediator.h"

class FileBrowserModel;
#ifndef NO_QTMOBILITY
QTM_BEGIN_NAMESPACE
class QSystemBatteryInfo;
QTM_END_NAMESPACE
#endif

class Window : public QDeclarativeView
{
  Q_OBJECT

public:
  Window();
  ~Window();
  enum {
        MIN_SCREEN_WIDTH = 1366,
        MIN_SCREEN_HEIGHT = 768,
        FIT_WIDTH_ZOOM_FACTOR = -1,
        FIT_WIDTH_ZOOM_INDEX = 7 //this value needs to be set manually
      };

protected:
  void closeEvent(QCloseEvent *);

signals:
  void wait();
  void warning(QVariant msg);

private slots:
  void onGotoPage(int page);
  void onShowDocument(const QString &doc, int page);
  void onSetZoomFactor(int value, int index);
  void onSetProperties();
  void onFullScreen();
  void onNormalScreen();
  void onShowHelp(bool show);
  void onQuit();
  void onQuitApp();

private:
  void openFile(const QString &filePath, int page = 0);
  void showNextPage();
  void showPrevPage();
  void gotoPage(int pageNb, int numPages);
  QString elapsedTime();
  QString batteryStatus();
  bool hasTouchScreen();
  void normalScreen();
  void updateViewForFitWidth() {
    if ((NULL != document_) && (FIT_WIDTH_ZOOM_FACTOR == document_->scaleFactor())) {
      document_->setScale(FIT_WIDTH_ZOOM_FACTOR, FIT_WIDTH_ZOOM_INDEX);
      gotoPage(document_->currentPage(), document_->numPages());
      refreshPage();
    }
  }
  void loadSettings(QString &filePath, int &page, qreal &scale, int &index);
  void saveSettings();
  void setCurrentPage(int page, int numPages) {
    qDebug() << "Window::setCurrentPage" << page << "/" << numPages;
    if (page >= numPages) {
      qDebug() << "resetting the number of pages";
      page = 0;
    }
    if (NULL != rootObj_) {
      if (false == rootObj_->setProperty("index", page)) {
        qDebug() << "cannot set index";
      }
      if (false == rootObj_->setProperty("pages", numPages)) {
        qDebug() << "cannot set the number of pages";
      }
    }
  }
  void setWindowSize(int width, int height) {
    qDebug() << "Window::setWindowSize: width" << width << ", height" << height;
    if (NULL != mediator_) {
      mediator_->setWinWidth(width);
      mediator_->setWinHeight(height);
    }
    else {
      qDebug() << "cannot set win size";
    }
  }
  void setZoomIndex(int index) {
    if (NULL != rootObj_) {
      if (false == rootObj_->setProperty("zoomIndex", index)) {
        qDebug() << "cannot set width";
      }
    }
  }
  void refreshPage() {
    if (NULL != rootObj_) {
      if (false == rootObj_->setProperty("refreshImg", true)) {
        qDebug() << "cannot set formats";
      }
      if (false == rootObj_->setProperty("refreshImg", false)) {
        qDebug() << "cannot set formats";
      }
    }
  }

  PageProvider *document_;
  FileBrowserModel *fileBrowserModel_;
#ifndef NO_QTMOBILITY
  QTM_NAMESPACE::QSystemBatteryInfo *batteryInfo_;
#endif
  QElapsedTimer eTime_;
  bool fullScreen_;
  QRect normScrGeometry_;
  struct {
    QString fileName;
    int page;
  } prev_;//used by showHelp to restore the previous document
  QString helpFile_;
  QGraphicsObject *rootObj_;
  QmlCppMediator *mediator_;
};

#endif
