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

#include <QPoint>
#include <QElapsedTimer>
#include <QQueue>
#include <QtDeclarative>

#include "config.h"

#ifndef NO_QTMOBILITY
#include <qmobilityglobal.h>
#endif
#include "pageprovider.h"

class QScrollArea;
class FileBrowserModel;
class QDeclarativeView;
class QTimer;
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
  bool hasTouchScreen();
  QString batteryStatus();
  enum {TOOLTIP_VISIBLE_TIME_MS = 1500,
        HORIZONTAL_SLIDE_SPEED_MS = 500,
        SWIPE_THRESHOLD = 5,
        LONG_PRESS_TIMEOUT_MS = 1000,
        WAIT_TIMER_INTERVAL_MS = 1000
       };

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
  void onSendCommand(const QString &cmd);
  void showHelp();
  void showAboutDialog();
  void closeAboutDialog();
  void showWarningMessage(const QString &title, const QString &explanation = "");
  void showWaitDialog();
  void closeWaitDialog();
  void showPropertiesDialog();
  void showDocument();

private:
  void showPageNumber(int currentPage, int nbPages);
  void setupDocDisplay(int pageNumber, qreal factor);
  void gotoPage(int pageNb, int numPages);
  void updateView(qreal factor);
  void setScale(qreal factor) {
    if (-1 == factor) { //need to set window width for fit width
      provider_->setWinWidth(width());
    }
    provider_->setScale(factor);
  }
  QString elapsedTime();
  void saveSettings();
  void setHelpIcon(bool /*flag*/, bool /*flipFlop = true*/) {
    /*if (true == flipFlop || (0 != prev_.page)) {
      QObject *pDisp = toolBar_->rootObject();
      if (NULL != pDisp) {
        pDisp->setProperty("hlpBck", flag);
      }
      if (false == flipFlop) {
        prev_.page = 0;
      }
    }*/
  }

  PageProvider *provider_;
  QVector<qreal> scaleFactors_;
  QPoint startPoint_;
  QPoint endPoint_;
  QElapsedTimer pressTimer_;
  FileBrowserModel* fileBrowserModel_;
  QTimer *waitTimer_;
#ifndef NO_QTMOBILITY
  QTM_NAMESPACE::QSystemBatteryInfo *batteryInfo_;
#endif
  int currentPage_;
  QElapsedTimer eTime_;
  const QString helpFile_;
  struct {
    QString fileName;
    int page;
  } prev_;//used by showHelp to restore the previous document
};

#endif
