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

#include <QtGui>
#include <QScrollArea>
#include <QtDeclarative>
#include <QtSystemInfo/QSystemDeviceInfo>
#include <QtSystemInfo/QSystemBatteryInfo>
#include "window.h"
#include "SlidingStackedWidget.h"
#include "filebrowsermodel.h"
#include "worker.h"
#include "flickable.h"

#define ORGANIZATION "Bogdan Cristea"
#define APPLICATION "tabletReader"
#define KEY_PAGE "current_page"
#define KEY_FILE_PATH "current_file_path"
#define KEY_ZOOM_LEVEL "current_zoom_level"
#define HELP_FILE ":/help/help/tabletReader.pdf"

QTM_USE_NAMESPACE

Window::Window(QWidget *parent)
    : QMainWindow(parent),
      slidingStacked_(NULL),
      document_(NULL),
      toolBar_(NULL),
      fileBrowser_(NULL),
      gotoPage_(NULL),
      zoomPage_(NULL),
      commandPopupMenu_(NULL),
      aboutDialog_(NULL),
      worker_(NULL),
      thread_(NULL),
      flickable_(NULL),
      fileBrowserModel_(new FileBrowserModel(this)),
      waitTimer_(NULL),
      waitDialog_(NULL),
      batteryInfo_(NULL),
      isSingleThreaded_(false),
      pageToLoadNo_(QQueue<int>())
{
    eTime_.start();//used to measure the elapsed time since the app is started

    //main window
    QWidget *centralWidget = new QWidget(this);
    QGridLayout *gridLayout = new QGridLayout(centralWidget);
    setCentralWidget(centralWidget);
    setWindowTitle(tr(APPLICATION));
    setStyleSheet("background-color: black");

    //actions for zoom in/out
    QAction *increaseScaleAction = new QAction(this);
    increaseScaleAction->setShortcut(tr("Ctrl++"));
    QAction *decreaseScaleAction = new QAction(this);
    decreaseScaleAction->setShortcut(tr("Ctrl+-"));
    addAction(increaseScaleAction);
    addAction(decreaseScaleAction);

    //zoom scale factors
    scaleFactors_ << 0.25 << 0.5 << 0.75 << 1.
                  << 1.25 << 1.5 << 2. << 3. << 4.;
    currentZoomIndex_ = 3;//zoom 100%

    //create main document
    document_ = new DocumentWidget(this);

    //worker thread
    worker_ = new Worker(document_, this);

    //create sliding animation
    slidingStacked_ = new SlidingStackedWidget(this);

    //create flickable object
    flickable_ = new Flickable(this);

    //init document pages and the sliding animation
    QScrollArea *scroll = NULL;
    QLabel *label = NULL;
    register int n = 0;
    for (n = 0; n < DocumentWidget::CACHE_SIZE; ++n)
    {
        //scroll areas (one for each page)
        scroll = new QScrollArea(centralWidget);
	scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidgetResizable(true);
        scroll->setAlignment(Qt::AlignCenter);
        label = new QLabel();//QLabel is used to display a page
        label->setAlignment(Qt::AlignCenter);
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        scroll->setWidget(label);
        scroll->installEventFilter(this);
        slidingStacked_->addWidget(scroll);//scroll areas are switched by the stacked widget
        flickable_->activateOn(scroll);
    }
    document_->setStackedWidget(slidingStacked_);
    document_->setPhysicalDpi(label->physicalDpiX(), label->physicalDpiY());
    slidingStacked_->setSpeed(HORIZONTAL_SLIDE_SPEED_MS);
    slidingStacked_->setWrap(true);
    slidingStacked_->setVerticalMode(false);
    slidingStacked_->setStyleSheet("background:black");
    slidingStacked_->setAttribute(Qt::WA_DeleteOnClose);
    gridLayout->addWidget(slidingStacked_, 1, 0, 1, 1);

    connect(slidingStacked_, SIGNAL(animationFinished()),
            this, SLOT(onAnimationFinished()));
    connect(increaseScaleAction, SIGNAL(triggered()), this, SLOT(increaseScale()));
    connect(decreaseScaleAction, SIGNAL(triggered()), this, SLOT(decreaseScale()));
    connect(this, SIGNAL(updateCache(int)), worker_, SLOT(onUpdateCache(int)));

    statusBar()->hide();

    //start worker thread
    thread_ = new QThread(this);
    worker_->moveToThread(thread_);
    thread_->start();
    if (true == thread_->isRunning())
    {
        qDebug() << "worker thread is running";
        thread_->setPriority(QThread::IdlePriority);
    } else
    {
        qDebug() << "worker thread is NOT running";
    }

    //set document if one has been previously open
    QSettings settings(ORGANIZATION, APPLICATION);
    QString filePath;
    if (NULL != (filePath = settings.value(KEY_FILE_PATH).toString()))
    {
        if (document_->setDocument(filePath))
        {
            currentZoomIndex_ = settings.value(KEY_ZOOM_LEVEL, 3).toInt();
            setupDocDisplay(settings.value(KEY_PAGE, 0).toInt()+1, filePath);
            fileBrowserModel_->setCurrentDir(filePath);
        }        
    } else
    {
        qDebug() << "no document found";
        showHelp(false);
    }
    animationFinished_ = true;

    //wait timer initialisation (used to handle too long actions: document openings, page changes)
    waitTimer_ = new QTimer(this);
    waitTimer_->setInterval(WAIT_TIMER_INTERVAL_MS);
    connect(waitTimer_, SIGNAL(timeout()), this, SLOT(showWaitDialog()));

    normalScreen();

    //main toolbar
    if (NULL != (toolBar_ = new QDeclarativeView(this)))
    {
        toolBar_->setSource(QUrl("qrc:/qml/qml/maintoolbar.qml"));
        QObject *pDisp = toolBar_->rootObject();
        if (NULL != pDisp)
        {
            pDisp->setProperty("width", width());
            QObject *pToolbar = pDisp->findChild<QObject*>("toolbar");
            if (NULL != pToolbar)
            {
                connect(pToolbar, SIGNAL(sendCommand(QString)), this, SLOT(onSendCommand(QString)));
            } else
            {
                qDebug() << "cannot find toolbar object";
            }
        }
        gridLayout->addWidget(toolBar_, 0, 0, 1, 1);
    }

    //battery status
    batteryInfo_ = new QSystemBatteryInfo(this);
    connect(batteryInfo_, SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),
            worker_, SLOT(onBatteryStatusChanged(QSystemBatteryInfo::BatteryStatus)));

#ifndef NO_APPUP_AUTH_CODE
    showWaitDialog();//prepare to check appup code
    appupApp_ = NULL;
    connect(worker_, SIGNAL(appUpAuthCheckError()), this,
            SLOT(onAppUpAuthCheckError()));
    QTimer::singleShot(0, worker_, SLOT(onCheckAppUpAuthCode()));
#endif
#ifndef DESKTOP_APP
    setWindowFlags(Qt::X11BypassWindowManagerHint);
#endif
}

Window::~Window()
{
    delete fileBrowserModel_;
    fileBrowserModel_ = NULL;

    //Cleanup code for Intel AppUp(TM) software
#ifndef NO_APPUP_AUTH_CODE
    delete appupApp_;
    appupApp_ = NULL;
#endif
}

void Window::onSendCommand(const QString &cmd)
{
    qDebug() << "Window::onSendCommand" << cmd;
    if (tr("Open") == cmd)
    {
        showFileBrowser();
    } else if (tr("Full Screen") == cmd)
    {
        fullScreen();
    } else if (tr("Go To Page") == cmd)
    {
        showGotoPage();
    } else if (tr("Zoom") == cmd)
    {
        showZoomPage();
    } else if (tr("Properties") == cmd)
    {
        showPropertiesDialog();
    } else if (tr("Help") == cmd)
    {
        showHelp();
    } else if (tr("About") == cmd)
    {
        showAboutDialog();
    } else if (tr("Exit") == cmd)
    {
        close();
    } else
    {
        qDebug() << "unknown command" << cmd;
    }
}

void Window::showFileBrowser()
{
    qDebug() << "Window::showFileBrowser";
    if (NULL == fileBrowser_)
    {
        if (NULL == (fileBrowser_ = new QDeclarativeView(this)))
        {
            showWarningMessage(tr("Cannot create fileBrowser object"),
                               tr("not enough memory"));
            return;
        }
        if (NULL == fileBrowserModel_)
        {
            showWarningMessage(tr("fileBrowserObject is NULL"));
            return;
        }
        fileBrowserModel_->searchPdfFiles();
        fileBrowser_->engine()->rootContext()->setContextProperty("pdfPreviewModel", fileBrowserModel_);
        fileBrowser_->setSource(QUrl("qrc:/qml/qml/filebrowser.qml"));
        fileBrowser_->setStyleSheet("background:transparent");
        fileBrowser_->setAttribute(Qt::WA_TranslucentBackground);
        fileBrowser_->setAttribute(Qt::WA_DeleteOnClose);
        fileBrowser_->setWindowFlags(Qt::FramelessWindowHint);
        fileBrowser_->move(0, 0);
        QObject *pDisp = fileBrowser_->rootObject();
        if (NULL != pDisp)
        {
            if (false == pDisp->setProperty("width", width()))
            {
                qDebug() << "cannot set width";
            }
            if (false == pDisp->setProperty("height", height()))
            {
                qDebug() << "cannot set height";
            }
            connect(pDisp, SIGNAL(changeDirectory(int)), fileBrowserModel_, SLOT(changeCurrentDir(int)));
            connect(pDisp, SIGNAL(showDocument(QString)), this, SLOT(closeFileBrowser(QString)));
            fileBrowser_->show();
        } else {
            qDebug() << "cannot get root object";
            delete fileBrowser_;
            fileBrowser_ = NULL;
        }
    }
}

void Window::closeFileBrowser(const QString &doc)
{
    qDebug() << "Window::closeFileBrowser" << doc;
    if ((NULL != fileBrowser_) && (true == fileBrowser_->close()))
    {
        qDebug() << "widget closed";
        fileBrowser_ = NULL;
        if (FileBrowserModel::closeFileBrowserText() != doc)
        {
            openFile(doc);
        } else
        {
            qDebug() << "closing file browser";
        }
    }
}

void Window::showGotoPage()
{
    qDebug() << "Window::showGotoPage";
    if (NULL == gotoPage_)
    {
        gotoPage_ = new QDeclarativeView(this);
        gotoPage_->setSource(QUrl("qrc:/qml/qml/gotopage.qml"));
        gotoPage_->setStyleSheet("background:transparent");
        gotoPage_->setAttribute(Qt::WA_TranslucentBackground);
        gotoPage_->setAttribute(Qt::WA_DeleteOnClose);
        gotoPage_->setWindowFlags(Qt::FramelessWindowHint);
        QObject *pRoot = gotoPage_->rootObject();
        if (NULL != pRoot)
        {
            pRoot->setProperty("width", width());
            pRoot->setProperty("height", height());
            connect(gotoPage_->engine(), SIGNAL(quit()), this, SLOT(closeGotoPage()));
            QObject *pDisp = pRoot->findChild<QObject*>("disp");
            if (NULL != pDisp)
            {
                connect(pDisp, SIGNAL(setPage(QString)), this, SLOT(closeGotoPage(QString)));
                gotoPage_->show();
            } else {
                qDebug() << "cannot get disp object";
                delete gotoPage_;
                gotoPage_ = NULL;
            }
        } else
        {
            qDebug() << "cannot get root object";
            delete gotoPage_;
            gotoPage_ = NULL;
        }
    }
}

void Window::closeGotoPage(const QString &pageNb)
{
    qDebug() << "Window::closeGotoPage: " << pageNb;
    if ((NULL != gotoPage_) && (true == gotoPage_->close()))
    {
        qDebug() << "widget closed";
        gotoPage_ = NULL;
        //set current page
        bool ok = false;
        int newPageNb = pageNb.toInt(&ok);
        if (true == ok)
        {
            int currentPage = document_->currentPage()+1;
            int numPages = document_->numPages();
            if ((newPageNb != currentPage) && (0 != newPageNb) && (newPageNb <= numPages))
            {
                //start timer
                waitTimer_->start();
                //change page
                gotoPage(newPageNb, numPages);
                if (currentPage < newPageNb)
                {
                    document_->showCurrentPageUpper();
                    slidingStacked_->slideInNext();
                } else {
                    document_->showCurrentPageLower();
                    slidingStacked_->slideInPrev();
                }
            } else {
                qDebug() << "nothing to do";
            }
        } else
        {
            qDebug() << "cannot convert input or empty input";
        }
    }
}

void Window::showZoomPage()
{
    qDebug() << "Window::showZoomPage";
    if (NULL == zoomPage_)
    {
        zoomPage_ = new QDeclarativeView(this);
        zoomPage_->setSource(QUrl("qrc:/qml/qml/zoompage.qml"));
        zoomPage_->setStyleSheet("background:transparent");
        zoomPage_->setAttribute(Qt::WA_TranslucentBackground);
        zoomPage_->setAttribute(Qt::WA_DeleteOnClose);
        zoomPage_->setWindowFlags(Qt::FramelessWindowHint);
        connect(zoomPage_->engine(), SIGNAL(quit()), this, SLOT(closeZoomPage()));
        QObject *pRoot = zoomPage_->rootObject();
        if (NULL != pRoot)
        {
            pRoot->setProperty("height", height());
            pRoot->setProperty("width", width());
            QObject *pZoomReel = pRoot->findChild<QObject*>("zoomreel");
            if (NULL != pZoomReel)
            {
                if (false == pZoomReel->setProperty("zoomIndex", currentZoomIndex_))
                {
                    qDebug() << "cannot set property";
                }
                connect(pZoomReel, SIGNAL(setZoomFactor(int)), this, SLOT(closeZoomPage(int)));
                zoomPage_->show();
            } else {
                qDebug() << "cannot get disp object";
                delete zoomPage_;
                zoomPage_ = NULL;
            }
        } else
        {
            qDebug() << "cannot get root object";
            delete zoomPage_;
            zoomPage_ = NULL;
        }
    }
}

void Window::closeZoomPage(int index)
{
    qDebug() << "Window::closeZoomPage";
    if ((NULL != zoomPage_) && (true == zoomPage_->close()))
    {
        qDebug() << "widget closed";
        zoomPage_ = NULL;
        setZoomFactor(index);
    }
}

void Window::showCommandPopupMenu()
{
    qDebug() << "Window::showCommandPopupMenu";
    if (false == toolBar_->isHidden())
    {
        return;//show command popup menu only when the toolbar of hidden
    }
    if (NULL == commandPopupMenu_)
    {
        commandPopupMenu_ = new QDeclarativeView(this);
        commandPopupMenu_->setSource(QUrl("qrc:/qml/qml/popupmenu.qml"));
        commandPopupMenu_->setStyleSheet("background:transparent");
        commandPopupMenu_->setAttribute(Qt::WA_TranslucentBackground);
        commandPopupMenu_->setAttribute(Qt::WA_DeleteOnClose);
        commandPopupMenu_->setWindowFlags(Qt::FramelessWindowHint);
        connect(commandPopupMenu_->engine(), SIGNAL(quit()), this, SLOT(closeCommandPopupMenu()));
        QObject *pRoot = commandPopupMenu_->rootObject();
        if (NULL != pRoot)
        {
            pRoot->setProperty("height", height());
            pRoot->setProperty("width", width());
            QObject *pDisp = pRoot->findChild<QObject*>("popuplist");
            if (NULL != pDisp)
            {
                connect(pDisp, SIGNAL(itemClicked(QString)), this, SLOT(closeCommandPopupMenu(QString)));
                commandPopupMenu_->show();
            } else {
                qDebug() << "cannot get popuplist object";
                delete commandPopupMenu_;
                commandPopupMenu_ = NULL;
            }
        } else
        {
            qDebug() << "cannot get root object";
            delete commandPopupMenu_;
            commandPopupMenu_ = NULL;
        }
    }
}

void Window::closeCommandPopupMenu(const QString &cmd)
{
    qDebug() << "Window::closeCommandPopupMenu" << cmd;
    if ((NULL != commandPopupMenu_) && (true == commandPopupMenu_->close()))
    {
        qDebug() << "widget closed";
        commandPopupMenu_ = NULL;
        if (tr("Open") == cmd)
        {
            showFileBrowser();
        } else if (tr("Go To Page") == cmd)
        {
            showGotoPage();
        } else if (tr("Properties") == cmd)
        {
            showPropertiesDialog();
        } else if (tr("Zoom") == cmd)
        {
            showZoomPage();
        } else if (tr("Exit") == cmd)
        {
            close();
        } else if (tr("Normal Screen") == cmd)
        {
            normalScreen();
        } else
        {
            qDebug() << "unknown command" << cmd;
        }
    }
}

void Window::openFile(const QString &filePath)
{
    qDebug() << "Window::openFile";
    //open document
    if (document_->setDocument(filePath))
    {
        //show wait dialog
        showWaitDialog();
        //load document
        setupDocDisplay(1, filePath);
        slidingStacked_->slideInNext();
    } else
    {
        showWarningMessage(QString(APPLICATION " - ")+tr("Failed to open file"),
                           tr("%1 cannot be opened").arg(filePath));
    }
}

void Window::fullScreen()
{
    qDebug() << "Window::fullScreen";
    toolBar_->hide();
    showFullScreen();
    if (true == hasTouchScreen())
    {
        QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
    }
}

void Window::normalScreen()
{
    qDebug() << "Window::normalScreen";

    QDesktopWidget *pDesktop = QApplication::desktop();
    if (NULL != pDesktop)
    {
        int width = pDesktop->width();
        int height = pDesktop->height();
        if ((FULL_SCREEN_WIDTH >= width) || (FULL_SCREEN_HEIGHT >= height))
        {
            qDebug() << "using full screen mode with toolbar";
            setFixedSize(width, height);
            showFullScreen();
            if (true == hasTouchScreen())
            {
                QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
            }
        } else
        {
            qDebug() << "using normal mode";
            showNormal();
            resize((MIN_SCREEN_WIDTH<width)?MIN_SCREEN_WIDTH:width,
                   (MIN_SCREEN_HEIGHT<height)?MIN_SCREEN_HEIGHT:height);
            if (true == hasTouchScreen())
            {
                QApplication::restoreOverrideCursor();
            }
        }
    }

    if (NULL != toolBar_)
    {
        toolBar_->show();
    }
}

void Window::increaseScale()
{
    qDebug() << "Window::increaseScale";
}

void Window::decreaseScale()
{
    qDebug() << "Window::decreaseScale";
}

bool Window::eventFilter(QObject *, QEvent *event)
{
    if (QEvent::Wheel == event->type())
    {
        // * handle mouse wheel events
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        if (0 > wheelEvent->delta())
        {
            showNextPage();
            return true;//stop further processing
        }
        if (0 < wheelEvent->delta())
        {
            showPrevPage();
            return true;
        }
    } else if (QEvent::MouseButtonPress == event->type()) {
        // * handle mouse events
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*> (event);
        startPoint_ = mouseEvent->pos();
        pressTimer_.start();
    } else if (QEvent::MouseButtonRelease == event->type()) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*> (event);
        endPoint_ = mouseEvent->pos();

        //process distance and direction
        int xDiff = qAbs(startPoint_.x() - endPoint_.x());
        int yDiff = qAbs(startPoint_.y() - endPoint_.y());
        if (xDiff <= 2*SWIPE_THRESHOLD && yDiff <= SWIPE_THRESHOLD) {
            if (pressTimer_.isValid() && pressTimer_.elapsed() >= LONG_PRESS_TIMEOUT_MS) {
                pressTimer_.invalidate();
                showCommandPopupMenu();
            }
        } else if( xDiff > yDiff )
        {
            // horizontal swipe detected, now find direction
            if( startPoint_.x() > endPoint_.x() )
            {
                //left swipe
                showNextPage();
            } else if ( startPoint_.x() < endPoint_.x() )
            {
                //right swipe
                showPrevPage();
            }
        }
        // vertical swipe is handled by Flickable class
    } else if (QEvent::KeyPress == event->type())
    {
        // * handle key events
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (Qt::Key_Escape == keyEvent->key() && isFullScreen())
        {
            normalScreen();
        }
        if (Qt::Key_Right == keyEvent->key())
        {
            showNextPage();
        }
        if (Qt::Key_Left == keyEvent->key())
        {
            showPrevPage();
        }
        if (Qt::Key_Home == keyEvent->key())
        {
            if (0 != document_->currentPage())
            {
                //not at the beginning of the document
                gotoPage(1, document_->numPages());
                slidingStacked_->slideInPrev();
            }
        }
        if (Qt::Key_End == keyEvent->key())
        {
            int numPages = document_->numPages();
            if ((numPages-1) != document_->currentPage())
            {
                //not at the end of the document
                gotoPage(numPages, numPages);
                slidingStacked_->slideInNext();
            }
        }
    }

    return false;
}

bool Window::showNextPage()
{
    qDebug() << "Window::showNextPage";
    if (false == document_->isLoaded() || false == animationFinished_)
    {
        return false;
    }

    currentPage_ = document_->currentPage()+2;
    int nbPages = document_->numPages();
    if (currentPage_ <= nbPages)
    {
        //start timer
        waitTimer_->start();
        //load a new page
        document_->setPage(currentPage_);
        document_->showCurrentPageUpper();        
        //emit signal to update the cache after the page has been displayed
        preloadPage(currentPage_);
        //make sure that the next page is ready
        animationFinished_ = false;
        slidingStacked_->slideInNext();
        return true;
    }

    return false;
}

bool Window::showPrevPage()
{
    qDebug() << "Window::showPrevPage";
    if (false == document_->isLoaded() || false == animationFinished_)
    {
        return false;
    }

    currentPage_ = document_->currentPage();
    if (0 < currentPage_)
    {
        //start timer
        waitTimer_->start();
        //load a new page
        document_->setPage(currentPage_);
        document_->showCurrentPageLower();        
        //emit signal to update the cache after the page has been displayed
        preloadPage(currentPage_-2);
        //make sure that the prev page is ready
        animationFinished_ = false;
        slidingStacked_->slideInPrev();
        return true;
    }

    return false;
}

void Window::closeEvent(QCloseEvent *evt)
{    
    qDebug() << "Window::closeEvent" << lastFilePath_ << document_->currentPage();

    saveSettings();
    //terminate worker thread
    if (NULL != thread_)
    {
        thread_->quit();
        while (false == thread_->isFinished());//wait for the thread to finish
        qDebug() << "worker finished";
        delete worker_;//delete worker object
        worker_ = NULL;
    }

    QWidget::closeEvent(evt);
}

void Window::onAnimationFinished()
{
    qDebug() << "Window::onAnimationFinished";        
    closeWaitDialog();
    //preload page
    preloadPageSingleThreaded();
    animationFinished_ = true;//must be the last statement
}

void Window::setupDocDisplay(unsigned int pageNumber, const QString &filePath)
{
    qDebug() << "Window::setupDocDisplay" << pageNumber;
    lastFilePath_ = filePath;    
    //set document zoom factor
    document_->setScale(scaleFactors_[currentZoomIndex_]);
    //set current page
    gotoPage(pageNumber, document_->numPages());
}

void Window::gotoPage(int pageNb, int numPages)
{
    qDebug() << "Window::gotoPage";
    //set current page
    if (true == document_->invalidatePageCache(pageNb-1))
    {
        document_->setPage(pageNb);
    }
    //preload next page
    if ((numPages-pageNb) > 0)
    {
        qDebug() << "Window::gotoPage: preload next page";
        preloadPage(pageNb);//next page (index starts from 0)
    }
    //preload previous page
    if (pageNb > 1)
    {
        qDebug() << "Window::gotoPage: preload previous page";
        preloadPage(pageNb-2);//previous page (index starts from 0)
    }
}

void Window::setZoomFactor(int index)
{
    qDebug() << "Window::setZoomFactor " << index;
    //set zoom factor
    if ((currentZoomIndex_ == index) || ((0 > index) || (scaleFactors_.count() <= index)))
    {
        qDebug() << "nothing to do";
        return;//nothing to do
    }
    currentZoomIndex_ = index;
    document_->setScale(scaleFactors_[currentZoomIndex_]);
    //update all pages from circular buffer
    int pageNb = document_->currentPage()+1;
    int numPages = document_->numPages();
    if (true == document_->invalidatePageCache(pageNb-1))
    {
        document_->showPage(pageNb);
    }
    //update view
    document_->showCurrentPageUpper();
    slidingStacked_->slideInNext();
    //preload next page
    if ((numPages-pageNb) > 0)
    {
        qDebug() << "Window::gotoPage: preload next page";
        preloadPage(pageNb);//next page (index starts from 0)
    }
    //preload previous page
    if (pageNb > 1)
    {
        qDebug() << "Window::gotoPage: preload previous page";
        preloadPage(pageNb-2);//previous page (index starts from 0)
    }    
}

void Window::showHelp(bool slideNext)
{
    qDebug() << "Window::showHelp";
    QFile file(HELP_FILE);
    if (true == file.open(QIODevice::ReadOnly))
    {
        if (document_->loadFromData(file.readAll()))
        {
            setupDocDisplay(1, HELP_FILE);
            document_->showCurrentPageUpper();
            if (true == slideNext)
            {
                slidingStacked_->slideInNext();
            }            
        } else
        {
            qDebug() << "cannot load from data";
        }
        file.close();
    } else
    {
        qDebug() << "cannot open help file";
        showWarningMessage(tr("Cannot open help file"));
    }
}

void Window::showAboutDialog()
{
    qDebug() << "Window::showAboutDialog";
    if (NULL == aboutDialog_)
    {
        aboutDialog_ = new QDeclarativeView(this);
        aboutDialog_->setSource(QUrl("qrc:/qml/qml/aboutdialog.qml"));
        aboutDialog_->setStyleSheet("background:transparent");
        aboutDialog_->setAttribute(Qt::WA_TranslucentBackground);
        aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);
        aboutDialog_->setWindowFlags(Qt::FramelessWindowHint);
        connect(aboutDialog_->engine(), SIGNAL(quit()), this, SLOT(closeAboutDialog()));
        QObject *pAbout = aboutDialog_->rootObject();
        if (NULL != pAbout)
        {
            pAbout->setProperty("height", height());
            pAbout->setProperty("width", width());
            QObject *pAboutDlg = pAbout->findChild<QObject*>("aboutDialog");
            if (NULL != pAboutDlg)
            {
                pAboutDlg->setProperty("text", tr("<H2>tabletReader v2.0</H2>"
                                                  "<H3>e-book reader for touch-enabled devices</H3>"
                                                  "<H4>Supported formats: PDF, DJVU and CHM.</H4><br>"
                                                  "Copyright (C) 2012, Bogdan Cristea. All rights reserved.<br>"
                                                  "<i>e-mail: cristeab@gmail.com</i><br><br>"
                                                  "This program is distributed in the hope that it will be useful, "
                                                  "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                                                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                                                  "GNU General Public License for more details.<br><br>"));
            } else
            {
                qDebug() << "cannot get aboutDialog object";
            }
            aboutDialog_->show();
        } else {
            qDebug() << "cannot get aboutDialog object";
            delete aboutDialog_;
            aboutDialog_ = NULL;
        }
    }
}

void Window::closeAboutDialog()
{
    qDebug() << "Window::closeAboutDialog";
    if ((NULL != aboutDialog_) && (true == aboutDialog_->close()))
    {
        qDebug() << "widget closed";
        aboutDialog_ = NULL;
    }
}

void Window::showWarningMessage(const QString &title, const QString &explanation)
{
    qDebug() << "Window::showWarningMessage";
    if (NULL == aboutDialog_)
    {
        aboutDialog_ = new QDeclarativeView(this);
        aboutDialog_->setSource(QUrl("qrc:/qml/qml/aboutdialog.qml"));
        aboutDialog_->setStyleSheet("background:transparent");
        aboutDialog_->setAttribute(Qt::WA_TranslucentBackground);
        aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);
        aboutDialog_->setWindowFlags(Qt::FramelessWindowHint);
        connect(aboutDialog_->engine(), SIGNAL(quit()), this, SLOT(closeAboutDialog()));
        QObject *pRoot = aboutDialog_->rootObject();
        if (NULL != pRoot)
        {
            pRoot->setProperty("height", height());
            pRoot->setProperty("width", width());
            QObject *pAbout = pRoot->findChild<QObject*>("aboutDialog");
            if (NULL != pAbout)
            {
                pAbout->setProperty("text", "<H3 style=\"color:red\">"+title+
                                    "</H3><br>"+explanation+"<br>");
            } else
            {
                qDebug() << "cannot get aboutDialog object";
            }
            aboutDialog_->show();
        } else
        {
            qDebug() << "cannot get aboutDialog object";
            delete aboutDialog_;
            aboutDialog_ = NULL;
        }
    }
}

void Window::showWaitDialog()
{
    qDebug() << "Window::showWaitDialog";
    waitTimer_->stop();
    if (NULL == waitDialog_)
    {
        waitDialog_ = new QDeclarativeView(this);
        waitDialog_->setSource(QUrl("qrc:/qml/qml/waitdialog.qml"));
        waitDialog_->setStyleSheet("background:transparent");
        waitDialog_->setAttribute(Qt::WA_TranslucentBackground);
        waitDialog_->setAttribute(Qt::WA_DeleteOnClose);
        waitDialog_->setWindowFlags(Qt::FramelessWindowHint);
        QObject *pRoot = waitDialog_->rootObject();
        if (NULL != pRoot)
        {
            pRoot->setProperty("height", height());
            pRoot->setProperty("width", width());
            waitDialog_->show();
        } else
        {
            qDebug() << "cannot get waitDialog object";
            delete waitDialog_;
            waitDialog_ = NULL;
        }
    }
}

void Window::closeWaitDialog()
{
    qDebug() << "Window::closeWaitDialog";
    waitTimer_->stop();
    if ((NULL != waitDialog_) && (true == waitDialog_->close()))
    {
        qDebug() << "widget closed";
        waitDialog_ = NULL;
    } else
    {
        qDebug() << "nothing to do";
    }
}

void Window::onAppUpAuthCheckError()
{
    showWarningMessage(tr("Cannot get authorization code for Intel AppUp(TM) software"),
    tr("You can use tabletReader, but it is highly recommended to connect to Intel AppUp center"));
    //aplication will exit
    //connect(aboutDialog_->engine(), SIGNAL(quit()), this, SLOT(close()));
}

void Window::showPropertiesDialog()
{
    qDebug() << "Window::showPropertiesDialog";
    if (NULL == aboutDialog_)
    {
        aboutDialog_ = new QDeclarativeView(this);
        aboutDialog_->setSource(QUrl("qrc:/qml/qml/aboutdialog.qml"));
        aboutDialog_->setStyleSheet("background:transparent");
        aboutDialog_->setAttribute(Qt::WA_TranslucentBackground);
        aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);
        aboutDialog_->setWindowFlags(Qt::FramelessWindowHint);
        connect(aboutDialog_->engine(), SIGNAL(quit()), this, SLOT(closeAboutDialog()));
        QObject *pAbout = aboutDialog_->rootObject();
        if (NULL != pAbout)
        {
            pAbout->setProperty("height", height());
            pAbout->setProperty("width", width());
            QObject *pAboutDlg = pAbout->findChild<QObject*>("aboutDialog");
            if (NULL != pAboutDlg)
            {
                //document path
                QString msg = tr("<H3>Document path:<br><i>%1</i></H3>").arg(lastFilePath_);
                //current page / page number
                msg += tr("<H3>Current page / Number of pages:<br><b>%1 / %2</b></H3>").arg(document_->currentPage()+1).arg(document_->numPages());
                //time since the application was started
                msg += tr("<H3>Elapsed time:<br>%1</H3>").arg(elapsedTime());
                //battery state
                msg += tr("<H3>Battery status:<br>%1</H3>").arg(batteryStatus());
                //set message
                pAboutDlg->setProperty("text", msg);
            } else
            {
                qDebug() << "cannot get aboutDialog object";
            }
            aboutDialog_->show();
        } else {
            qDebug() << "cannot get aboutDialog object";
            delete aboutDialog_;
            aboutDialog_ = NULL;
        }
    }
}

bool Window::hasTouchScreen()
{
    QSystemDeviceInfo systemInfo;
    QSystemDeviceInfo::InputMethodFlags flags = systemInfo.inputMethodType();
    return ((flags & (QSystemDeviceInfo::SingleTouch |
                      QSystemDeviceInfo::MultiTouch)) != 0)?true:false;
}

QString Window::batteryStatus()
{
    QString msg;
    switch(batteryInfo_->chargerType())
    {
    case QSystemBatteryInfo::NoCharger:
        msg = tr("no charger");
        break;
    case QSystemBatteryInfo::WallCharger:
        msg = tr("wall charger");
        break;
    case QSystemBatteryInfo::USBCharger:
    case QSystemBatteryInfo::USB_500mACharger:
    case QSystemBatteryInfo::USB_100mACharger:
    case QSystemBatteryInfo::VariableCurrentCharger:
        msg = tr("charging");
        break;
    case QSystemBatteryInfo::UnknownCharger:
    default:
        msg = tr("unknown");
    }
    int remCap = batteryInfo_->remainingCapacityPercent();
    if (-1 != remCap)
    {
        msg += tr(", %1% remaining capacity").arg(remCap);
    } else{
        msg += tr(", unknown remaining capacity");
    }
    return msg;
}

QString Window::elapsedTime()
{
    QString msg = "";
    qint64 eTimeMs = eTime_.elapsed();
    qint64 eTimeHrs = eTimeMs/(1000*3600);
    if (0 != eTimeHrs)
    {
        if (1 == eTimeHrs)
        {
            msg += tr("1 hour");
        } else
        {
            msg += tr("%1 hours").arg(eTimeHrs);
        }
    }
    qint64 eTimeMin = (eTimeMs-eTimeHrs*1000*3600)/(1000*60);
    if (0 != eTimeMin)
    {
        if (0 != eTimeHrs)
        {
            msg += ", ";
        }
        if (1 == eTimeMin)
        {
            msg += tr("1 minute");
        } else
        {
            msg += tr("%1 minutes").arg(eTimeMin);
        }
    }
    qint64 eTimeSec = (eTimeMs-eTimeHrs*1000*3600-eTimeMin*1000*60)/1000;
    if (0 != eTimeSec)
    {
        if ((0 != eTimeMin) || (0 != eTimeHrs))
        {
            msg += ", ";
        }
        if (1 == eTimeSec)
        {
            msg += tr("1 second");
        } else
        {
            msg += tr("%1 seconds").arg(eTimeSec);
        }
    }
    return msg;
}

void Window::saveSettings()
{
    if ((NULL != document_) && (QString(HELP_FILE) != lastFilePath_))
    {
        //the current settings are not saved if the last file is the help file
        QSettings settings(ORGANIZATION, APPLICATION);
        settings.setValue(KEY_PAGE, document_->currentPage());
        settings.setValue(KEY_FILE_PATH, lastFilePath_);
        settings.setValue(KEY_ZOOM_LEVEL, currentZoomIndex_);
    }
}
