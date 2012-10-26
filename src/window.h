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

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QElapsedTimer>
#include <QQueue>
#include <QtDeclarative>

#include "config.h"

#ifndef NO_QTMOBILITY
#include <qmobilityglobal.h>
#endif
#include "documentwidget.h"

class QScrollArea;
class QSpinBox;
class QComboBox;
class SlidingStackedWidget;
class FileBrowserModel;
class QDeclarativeView;
class Flickable;
class QTimer;
#ifndef NO_QTMOBILITY
QTM_BEGIN_NAMESPACE
class QSystemBatteryInfo;
QTM_END_NAMESPACE
#endif

class Window : public QMainWindow
{
  Q_OBJECT

public:
  Window(QWidget *parent = NULL);
  ~Window();
  bool hasTouchScreen();
  QString batteryStatus();
  enum {HORIZONTAL_SLIDE_SPEED_MS = 500,
        SWIPE_THRESHOLD = 5,
        LONG_PRESS_TIMEOUT_MS = 1000,
        WAIT_TIMER_INTERVAL_MS = 1000,
        MIN_SCREEN_WIDTH = 1024,
        MIN_SCREEN_HEIGHT = 768,
        FIT_WIDTH_ZOOM_FACTOR = -1};

protected:
  void closeEvent(QCloseEvent *);

private slots:
  void showFileBrowser();
  void closeFileBrowser(const QString &doc);
  void showGotoPage();
  void closeGotoPage(const QString &pageNb = "");
  void showZoomPage();
  void closeZoomPage(int index = -1);
  void showCommandPopupMenu();
  void closeCommandPopupMenu(const QString &cmd = "");
  void openFile(const QString &filePath);
  void fullScreen();
  void normalScreen();
  void onAnimationFinished();
  void onSendCommand(const QString &cmd);
  void showHelp(bool slideNext = true);
  void showAboutPage();
  void closeAboutPage();
  void showWarningMessage(const QString &title, const QString &explanation = "");
  void showWaitPage();
  void closeWaitPage();
  void onAppUpAuthCheckError();
  void showPropertiesPage();
  void showDocument();
  void onBack();

private:
  bool eventFilter(QObject *, QEvent *);
  bool showNextPage();
  bool showPrevPage();
  void showPageNumber(int currentPage, int nbPages);
  void setupDocDisplay(unsigned int pageNumber, qreal factor);
  void gotoPage(int pageNb, int numPages);
  void updateView(qreal factor, bool force = false);
  void setScale(qreal factor) {
    if (FIT_WIDTH_ZOOM_FACTOR == factor) { //need to set window width for fit width
      document_->setWinWidth(width());
    }
    document_->setScale(factor);
  }
  QString elapsedTime();
  void saveSettings();
  void setHelpIcon(bool flag, bool flipFlop = true) {
    if (true == flipFlop || (0 != prev_.page)) {
      QObject *pDisp = toolBar_->rootObject();
      if (NULL != pDisp) {
        pDisp->setProperty("hlpBck", flag);
      }
      if (false == flipFlop) {
        prev_.page = 0;
      }
    }
  }
  void addShortcutKeys();
  bool isBackground() {
    return (NULL != fileBrowser_) || (NULL != gotoPage_) ||
      (NULL != zoomPage_) || (NULL != commandPopupMenu_) ||
      (NULL != aboutDialog_) || (NULL != waitDialog_);
  }
  void updateViewForFitWidth(int width) {
    if ((NULL != document_) && (FIT_WIDTH_ZOOM_FACTOR == document_->scale())) {
      document_->setWinWidth(width);
      document_->setScale(FIT_WIDTH_ZOOM_FACTOR);
      updateView(FIT_WIDTH_ZOOM_FACTOR, true);
    }
  }

  SlidingStackedWidget *slidingStacked_;
  DocumentWidget *document_;
  QVector<qreal> scaleFactors_;
  QPoint startPoint_;
  QPoint endPoint_;
  bool animationFinished_;
  //TODO: one declarative view
  QDeclarativeView *toolBar_;
  QDeclarativeView *fileBrowser_;
  QDeclarativeView *gotoPage_;
  QDeclarativeView *zoomPage_;
  QDeclarativeView *commandPopupMenu_;
  QDeclarativeView *aboutDialog_;
  QDeclarativeView *waitDialog_;
  QElapsedTimer pressTimer_;
  Flickable *flickable_;
  FileBrowserModel* fileBrowserModel_;
  QTimer *waitTimer_;
#ifndef NO_QTMOBILITY
  QTM_NAMESPACE::QSystemBatteryInfo *batteryInfo_;
#endif
  int currentPage_;
  QElapsedTimer eTime_;
  const QString helpFile_;
  bool fullScreen_;
  struct {
    QString fileName;
    int page;
  } prev_;//used by showHelp to restore the previous document
};

#endif
