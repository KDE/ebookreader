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
#include <qmobilityglobal.h>
#include "documentwidget.h"
#include "worker.h"

class QScrollArea;
class QSpinBox;
class QComboBox;
class SlidingStackedWidget;
class FileBrowserModel;
class QDeclarativeView;
class Flickable;
class QTimer;
QTM_BEGIN_NAMESPACE
class QSystemBatteryInfo;
QTM_END_NAMESPACE

class Window : public QMainWindow
{
    Q_OBJECT

    friend class Worker;

public:
    Window(QWidget *parent = NULL);
    ~Window();
    bool hasTouchScreen();
    QString batteryStatus();
    void setSingleThreaded(bool value = true)
    {
        isSingleThreaded_ = value;
    }
    enum {TOOLTIP_VISIBLE_TIME_MS = 1500,
         HORIZONTAL_SLIDE_SPEED_MS = 500,
         SWIPE_THRESHOLD = 5,
         LONG_PRESS_TIMEOUT_MS = 1000,
         WAIT_TIMER_INTERVAL_MS = 1000};

protected:
    void closeEvent(QCloseEvent *);

signals:
    void updateCache(int);

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
    void increaseScale();
    void decreaseScale();
    void onAnimationFinished();
    void onSendCommand(const QString &cmd);
    void showHelp(bool slideNext = true);
    void showAboutDialog();
    void closeAboutDialog();
    void showWarningMessage(const QString &title, const QString &explanation = "");
    void showWaitDialog();
    void closeWaitDialog();
    void onAppUpAuthCheckError();
    void showPropertiesDialog();

private:
    bool eventFilter(QObject *, QEvent *);
    bool showNextPage();
    bool showPrevPage();
    void showPageNumber(int currentPage, int nbPages);
    void setupDocDisplay(unsigned int pageNumber, const QString &filePath);
    void gotoPage(int pageNb, int numPages);
    void setZoomFactor(int index);
    QString elapsedTime();
    void saveSettings();
    void preloadPage(int page)
    {
        if (false == isSingleThreaded_)
        {            
            if (true == document_->invalidatePageCache(page))
            {
                qDebug() << "Window::preloadPageMultiThreaded";
                emit updateCache(page);
            }
        }
        else
        {
            //the single threaded version of this function is called from onAnimationFinished slot
            pageToLoadNo_.enqueue(page);//put into queue the page number
        }
    }
    //should be called only from onAnimationFinished slot
    void preloadPageSingleThreaded()
    {        
        if (true == isSingleThreaded_)
        {
            while (false == pageToLoadNo_.isEmpty())
            {
                int page = pageToLoadNo_.dequeue();
                if (true == document_->invalidatePageCache(page))
                {
                    qDebug() << "Window::preloadPageSingleThreaded";
                    worker_->onUpdateCache(page);
                }
            }
        }
    }

    SlidingStackedWidget *slidingStacked_;
    DocumentWidget *document_;
    QDeclarativeView *toolBar_;
    QString lastFilePath_;
    QVector<qreal> scaleFactors_;
    int currentZoomIndex_;
    QPoint startPoint_;
    QPoint endPoint_;
    bool animationFinished_;
    QDeclarativeView *fileBrowser_;
    QDeclarativeView *gotoPage_;
    QDeclarativeView *zoomPage_;
    QDeclarativeView *commandPopupMenu_;
    QDeclarativeView *aboutDialog_;
    QElapsedTimer pressTimer_;
    Worker *worker_;
    QThread *thread_;
    Flickable *flickable_;
    FileBrowserModel* fileBrowserModel_;
    QTimer *waitTimer_;
    QDeclarativeView *waitDialog_;
    QTM_NAMESPACE::QSystemBatteryInfo *batteryInfo_;
    int currentPage_;
    QElapsedTimer eTime_;    
    bool isSingleThreaded_;
    QQueue<int> pageToLoadNo_;
};

#endif
