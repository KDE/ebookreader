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
#ifndef NO_QTMOBILITY
#include <QtSystemInfo/QSystemDeviceInfo>
#include <QtSystemInfo/QSystemBatteryInfo>
#endif
#include "window.h"
#include "SlidingStackedWidget.h"
#include "filebrowsermodel.h"
#include "flickable.h"

#define ORGANIZATION "Bogdan Cristea"
#define APPLICATION "tabletReader"
#define KEY_PAGE "current_page"
#define KEY_FILE_PATH "current_file_path"
#define KEY_ZOOM_LEVEL "current_zoom_level"
#if defined(WIN32) || defined(WIN64)
#define HELP_FILE "/tabletReaderHelp.pdf"
#else
#define HELP_FILE "/../share/doc/tabletReaderHelp.pdf"
#endif
#ifndef NO_QTMOBILITY
QTM_USE_NAMESPACE
#endif

Window::Window(QWidget *parent)
  : QMainWindow(parent),
    slidingStacked_(NULL),
    document_(NULL),
    animationFinished_(true),
    toolBar_(NULL),
    fileBrowser_(NULL),
    gotoPage_(NULL),
    zoomPage_(NULL),
    commandPopupMenu_(NULL),
    aboutDialog_(NULL),
    waitDialog_(NULL),
    flickable_(NULL),
    fileBrowserModel_(NULL),
    waitTimer_(NULL),
#ifndef NO_QTMOBILITY
    batteryInfo_(NULL),
#endif
    helpFile_(QCoreApplication::applicationDirPath()+QString(HELP_FILE)),
    fullScreen_(false)
{
  prev_.fileName = "";
  prev_.page = 0;

  eTime_.start();//used to measure the elapsed time since the app is started
  QApplication::setOverrideCursor(Qt::WaitCursor);

  //main window
  QWidget *centralWidget = new QWidget(this);
  QGridLayout *gridLayout = new QGridLayout(centralWidget);
  setCentralWidget(centralWidget);
  setWindowTitle(tr(APPLICATION));
  setStyleSheet("background-color: black");

  //zoom scale factors
  //-1 used for fit width scaling factor
  scaleFactors_ << -1 << 0.25 << 0.5 << 0.75 << 1.
                << 1.25 << 1.5 << 2. << 3. << 4.;

  //create main document
  document_ = new DocumentWidget(this);
  //create file browser (uses supported file types given by OkularDocument)
  fileBrowserModel_ = new FileBrowserModel(this, document_->supportedFilePatterns());

  //create sliding animation
  slidingStacked_ = new SlidingStackedWidget(this);

  //create flickable object
  flickable_ = new Flickable(this);

  //init document pages and the sliding animation
  QScrollArea *scroll = NULL;
  QLabel *label = NULL;
  register int n = 0;
  for(n = 0; n < DocumentWidget::CACHE_SIZE; ++n) {
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
  slidingStacked_->setSpeed(HORIZONTAL_SLIDE_SPEED_MS);
  slidingStacked_->setWrap(true);
  slidingStacked_->setVerticalMode(false);
  slidingStacked_->setStyleSheet("background:black");
  slidingStacked_->setAttribute(Qt::WA_DeleteOnClose);
  gridLayout->addWidget(slidingStacked_, 1, 0, 1, 1);

  connect(slidingStacked_, SIGNAL(animationFinished()),
          this, SLOT(onAnimationFinished()));

  statusBar()->hide();

  //wait timer initialisation (used to handle long actions: document openings, page changes)
  waitTimer_ = new QTimer(this);
  waitTimer_->setInterval(WAIT_TIMER_INTERVAL_MS);
  connect(waitTimer_, SIGNAL(timeout()), this, SLOT(showWaitPage()));

  normalScreen();

  //main toolbar
  if(NULL != (toolBar_ = new QDeclarativeView(this))) {
    toolBar_->setSource(QUrl("qrc:/qml/qml/maintoolbar.qml"));
    toolBar_->setFocusPolicy(Qt::NoFocus);
    QObject *pDisp = toolBar_->rootObject();
    if(NULL != pDisp) {
      pDisp->setProperty("width", width());
      QObject *pToolbar = pDisp->findChild<QObject*>("toolbar");
      if(NULL != pToolbar) {
        connect(pToolbar, SIGNAL(sendCommand(QString)), this, SLOT(onSendCommand(QString)));
      }
      else {
        qDebug() << "cannot find toolbar object";
      }
    }
    gridLayout->addWidget(toolBar_, 0, 0, 1, 1);
  }

  addShortcutKeys();

#ifndef NO_QTMOBILITY
  //battery status
  batteryInfo_ = new QSystemBatteryInfo(this);
#endif

  Qt::WindowFlags winFlags = Qt::WindowMinimizeButtonHint;
#ifndef DESKTOP_APP
  winFlags |= Qt::X11BypassWindowManagerHint;
#endif
  setWindowFlags(winFlags);

  QTimer::singleShot(0, this, SLOT(showDocument()));
}

void Window::showDocument()
{
  qDebug() << "Window::showDocument";

  //set document if one has been previously open
  QSettings settings(ORGANIZATION, APPLICATION);
  QString filePath;
  waitTimer_->start();
  if ((NULL != (filePath = settings.value(KEY_FILE_PATH).toString())) &&
    (true == document_->setDocument(filePath))) {
      qDebug() << "Found document " << filePath;
      setupDocDisplay(settings.value(KEY_PAGE, 0).toInt() + 1, 
          settings.value(KEY_ZOOM_LEVEL, 1.0).toFloat());
      fileBrowserModel_->setCurrentDir(filePath);
  }
  else {
    qDebug() << "no document found";
    setScale(FIT_WIDTH_ZOOM_FACTOR);//use default fit width scale factor
    showHelp(false);
  }
  onAnimationFinished();//simulate an onAnimationFinished
  QApplication::restoreOverrideCursor();
}

Window::~Window()
{
  delete fileBrowserModel_;
  fileBrowserModel_ = NULL;
}

void Window::onSendCommand(const QString &cmd)
{
  qDebug() << "Window::onSendCommand" << cmd;
  if(tr("Open") == cmd) {
    showFileBrowser();
  }
  else if(tr("Full Screen") == cmd) {
    fullScreen();
  }
  else if(tr("Go To Page") == cmd) {
    showGotoPage();
  }
  else if(tr("Zoom") == cmd) {
    showZoomPage();
  }
  else if(tr("Properties") == cmd) {
    showPropertiesPage();
  }
  else if((tr("Help") == cmd) || (tr("Back") == cmd)) {
    if ((tr("Help") == cmd) && (helpFile_ == document_->filePath())) {
      return;//nothing to do
    }
    showHelp();
  }
  else if(tr("About") == cmd) {
    showAboutPage();
  }
  else if(tr("Quit") == cmd) {
    close();
  }
  else {
    qDebug() << "unknown command" << cmd;
  }
}

void Window::showFileBrowser()
{
  qDebug() << "Window::showFileBrowser";

  if (false == animationFinished_) {
    qDebug() << "unfinished animation";
    return;
  }

  if(NULL == fileBrowser_) {
    if(NULL == (fileBrowser_ = new QDeclarativeView(this))) {
      showWarningMessage(tr("Cannot create fileBrowser object"),
                         tr("not enough memory"));
      return;
    }
    if(NULL == fileBrowserModel_) {
      showWarningMessage(tr("fileBrowserObject is NULL"));
      return;
    }
    fileBrowserModel_->searchSupportedFiles();
    fileBrowser_->engine()->rootContext()->setContextProperty("pdfPreviewModel", fileBrowserModel_);
    fileBrowser_->setSource(QUrl("qrc:/qml/qml/filebrowser.qml"));
    fileBrowser_->setStyleSheet("background:transparent");
    fileBrowser_->setAttribute(Qt::WA_TranslucentBackground);
    fileBrowser_->setAttribute(Qt::WA_DeleteOnClose);
    fileBrowser_->setWindowFlags(Qt::FramelessWindowHint);
	fileBrowser_->setFocusPolicy(Qt::NoFocus);
    fileBrowser_->move(0, 0);
    QObject *pDisp = fileBrowser_->rootObject();
    if(NULL != pDisp) {
      if(false == pDisp->setProperty("width", width())) {
        qDebug() << "cannot set width";
      }
      if(false == pDisp->setProperty("height", height())) {
        qDebug() << "cannot set height";
      }
      connect(pDisp, SIGNAL(changeDirectory(int)), fileBrowserModel_, SLOT(changeCurrentDir(int)));
      connect(pDisp, SIGNAL(showDocument(QString)), this, SLOT(closeFileBrowser(QString)));
      fileBrowser_->show();
    }
    else {
      qDebug() << "cannot get root object";
      delete fileBrowser_;
      fileBrowser_ = NULL;
    }
  }
}

void Window::closeFileBrowser(const QString &doc)
{
  qDebug() << "Window::closeFileBrowser" << doc;

  if((NULL != fileBrowser_) && (true == fileBrowser_->close())) {
    qDebug() << "widget closed";
    fileBrowser_ = NULL;
    if(FileBrowserModel::closeFileBrowserText() != doc) {
      openFile(doc);
    }
  }
}

void Window::showGotoPage()
{
  qDebug() << "Window::showGotoPage";
  if(NULL == gotoPage_) {
    gotoPage_ = new QDeclarativeView(this);
    gotoPage_->setSource(QUrl("qrc:/qml/qml/gotopage.qml"));
    gotoPage_->setStyleSheet("background:transparent");
    gotoPage_->setAttribute(Qt::WA_TranslucentBackground);
    gotoPage_->setAttribute(Qt::WA_DeleteOnClose);
    gotoPage_->setWindowFlags(Qt::FramelessWindowHint);
    gotoPage_->setFocusPolicy(Qt::NoFocus);
    QObject *pRoot = gotoPage_->rootObject();
    if(NULL != pRoot) {
      pRoot->setProperty("width", width());
      pRoot->setProperty("height", height());
      connect(gotoPage_->engine(), SIGNAL(quit()), this, SLOT(closeGotoPage()));
      QObject *pDisp = pRoot->findChild<QObject*>("disp");
      if(NULL != pDisp) {
        connect(pDisp, SIGNAL(setPage(QString)), this, SLOT(closeGotoPage(QString)));
        pDisp->setProperty("nbPages", document_->numPages());
        gotoPage_->show();
      }
      else {
        qDebug() << "cannot get disp object";
        delete gotoPage_;
        gotoPage_ = NULL;
      }
    }
    else {
      qDebug() << "cannot get root object";
      delete gotoPage_;
      gotoPage_ = NULL;
    }
  }
}

void Window::closeGotoPage(const QString &pageNb)
{
  qDebug() << "Window::closeGotoPage: " << pageNb;

  if((NULL != gotoPage_) && (true == gotoPage_->close()) && (true == animationFinished_)) {
    qDebug() << "widget closed";
    gotoPage_ = NULL;
    //set current page
    bool ok = false;
    int newPageNb = pageNb.toInt(&ok);
    if(true == ok) {
      int currentPage = document_->currentPage() + 1;
      int numPages = document_->numPages();
      if((newPageNb != currentPage) && (0 != newPageNb) && (newPageNb <= numPages)) {
        //start timer
        waitTimer_->start();
        //change page
        animationFinished_ = false;
        gotoPage(newPageNb, numPages);
        if(currentPage < newPageNb) {
          document_->showCurrentPageUpper();
          slidingStacked_->slideInNext();
        }
        else {
          document_->showCurrentPageLower();
          slidingStacked_->slideInPrev();
        }
      }
    }
  }
}

void Window::showZoomPage()
{
  qDebug() << "Window::showZoomPage";
  if(NULL == zoomPage_) {
    zoomPage_ = new QDeclarativeView(this);
    zoomPage_->setSource(QUrl("qrc:/qml/qml/zoompage.qml"));
    zoomPage_->setStyleSheet("background:transparent");
    zoomPage_->setAttribute(Qt::WA_TranslucentBackground);
    zoomPage_->setAttribute(Qt::WA_DeleteOnClose);
    zoomPage_->setWindowFlags(Qt::FramelessWindowHint);
    zoomPage_->setFocusPolicy(Qt::NoFocus);
    connect(zoomPage_->engine(), SIGNAL(quit()), this, SLOT(closeZoomPage()));
    QObject *pRoot = zoomPage_->rootObject();
    if(NULL != pRoot) {
      pRoot->setProperty("height", height());
      pRoot->setProperty("width", width());
      QObject *pZoomReel = pRoot->findChild<QObject*>("zoomreel");
      if(NULL != pZoomReel) {
        if(false == pZoomReel->setProperty("zoomIndex", scaleFactors_.indexOf(document_->scale()))) {
          qDebug() << "cannot set property";
        }
        connect(pZoomReel, SIGNAL(setZoomFactor(int)), this, SLOT(closeZoomPage(int)));
        zoomPage_->show();
      }
      else {
        qDebug() << "cannot get disp object";
        delete zoomPage_;
        zoomPage_ = NULL;
      }
    }
    else {
      qDebug() << "cannot get root object";
      delete zoomPage_;
      zoomPage_ = NULL;
    }
  }
}

void Window::closeZoomPage(int index)
{
  qDebug() << "Window::closeZoomPage";
  if((NULL != zoomPage_) && (true == zoomPage_->close())) {
    qDebug() << "widget closed";
    zoomPage_ = NULL;
    if((0 <= index) && (index < scaleFactors_.size())) {
      updateView(scaleFactors_[index]);
    }
  }
}

void Window::showCommandPopupMenu()
{
  qDebug() << "Window::showCommandPopupMenu";
  if(false == toolBar_->isHidden()) {
    return;//show command popup menu only when the toolbar of hidden
  }
  if(NULL == commandPopupMenu_) {
    commandPopupMenu_ = new QDeclarativeView(this);
    commandPopupMenu_->setSource(QUrl("qrc:/qml/qml/popupmenu.qml"));
    commandPopupMenu_->setStyleSheet("background:transparent");
    commandPopupMenu_->setAttribute(Qt::WA_TranslucentBackground);
    commandPopupMenu_->setAttribute(Qt::WA_DeleteOnClose);
    commandPopupMenu_->setWindowFlags(Qt::FramelessWindowHint);
    connect(commandPopupMenu_->engine(), SIGNAL(quit()), this, SLOT(closeCommandPopupMenu()));
    QObject *pRoot = commandPopupMenu_->rootObject();
    if(NULL != pRoot) {
      pRoot->setProperty("height", height());
      pRoot->setProperty("width", width());
      QObject *pDisp = pRoot->findChild<QObject*>("popuplist");
      if(NULL != pDisp) {
        connect(pDisp, SIGNAL(itemClicked(QString)), this, SLOT(closeCommandPopupMenu(QString)));
        commandPopupMenu_->show();
      }
      else {
        qDebug() << "cannot get popuplist object";
        delete commandPopupMenu_;
        commandPopupMenu_ = NULL;
      }
    }
    else {
      qDebug() << "cannot get root object";
      delete commandPopupMenu_;
      commandPopupMenu_ = NULL;
    }
  }
}

void Window::closeCommandPopupMenu(const QString &cmd)
{
  qDebug() << "Window::closeCommandPopupMenu" << cmd;
  if((NULL != commandPopupMenu_) && (true == commandPopupMenu_->close())) {
    qDebug() << "widget closed";
    commandPopupMenu_ = NULL;
    if(tr("Open") == cmd) {
      showFileBrowser();
    }
    else if(tr("Go To Page") == cmd) {
      showGotoPage();
    }
    else if(tr("Properties") == cmd) {
      showPropertiesPage();
    }
    else if(tr("Zoom") == cmd) {
      showZoomPage();
    }
    else if(tr("Quit") == cmd) {
      close();
    }
    else if(tr("Normal Screen") == cmd) {
      normalScreen();
    }
    else {
      qDebug() << "unknown command" << cmd;
    }
  }
}

void Window::openFile(const QString &filePath)
{
  qDebug() << "Window::openFile";

  if (false == animationFinished_) {
    qDebug() << "unfinished animation";
    return;
  }

  //open document
  waitTimer_->start();
  if(document_->setDocument(filePath)) {
    //load document
    animationFinished_ = false;
    setupDocDisplay(1, document_->scale());
    slidingStacked_->slideInNext();
    setHelpIcon(true, false);
  }
  else {
    closeWaitPage();
    showWarningMessage(QString(APPLICATION " - ") + tr("Failed to open file"),
                       tr("%1 cannot be opened").arg(filePath));
  }
}

void Window::fullScreen()
{
  qDebug() << "Window::fullScreen";

  if (true == fullScreen_) {
    return;
  }
  normScrGeometry_ = geometry();//store current geometry

  toolBar_->hide();
  showFullScreen();
  if(true == hasTouchScreen()) {
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
  }
  //work around since the window width is not yet available
  updateViewForFitWidth(QApplication::desktop()->width());
  fullScreen_ = true;
}

void Window::normalScreen()
{
  qDebug() << "Window::normalScreen";

  fullScreen_ = false;
  if (NULL != toolBar_) {
    toolBar_->show();
  }

  int width = 0;
  int height = 0;
  if (false == normScrGeometry_.isValid()) {
    width = QApplication::desktop()->width();
    height = QApplication::desktop()->height();
    if((MIN_SCREEN_WIDTH >= width) && (MIN_SCREEN_HEIGHT >= height)) {
      qDebug() << "using full screen mode with toolbar";
      setFixedSize(width, height);
      showFullScreen();
      if(true == hasTouchScreen()) {
        QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
      }
    }
    else {
      qDebug() << "using normal mode";
      width = (MIN_SCREEN_WIDTH < width) ? MIN_SCREEN_WIDTH : width;
      height = (MIN_SCREEN_HEIGHT < height) ? MIN_SCREEN_HEIGHT : height;
      resize(width, height);
      showNormal();
      if(true == hasTouchScreen()) {
        QApplication::restoreOverrideCursor();
      }
    }
  }
  else {
    setGeometry(normScrGeometry_);
    width = normScrGeometry_.width();
  }
  //work around since the window width is not yet available
  updateViewForFitWidth(width);
}

bool Window::eventFilter(QObject *, QEvent *event)
{
  bool out = false;

  if(QEvent::Wheel == event->type()) {
    if (false == isBackground()) {
      // * handle mouse wheel events
      QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
      if(0 > wheelEvent->delta()) {
        showNextPage();
        return true;//stop further processing
      }
      if(0 < wheelEvent->delta()) {
        showPrevPage();
        return true;
      }
    }
  }
  else if(QEvent::MouseButtonPress == event->type()) {
    // * handle mouse events
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    startPoint_ = mouseEvent->pos();
  }
  else if(QEvent::MouseButtonRelease == event->type()) {
    // * handle mouse events - cont
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    endPoint_ = mouseEvent->pos();

    //process distance and direction
    int xDiff = qAbs(startPoint_.x() - endPoint_.x());
    int yDiff = qAbs(startPoint_.y() - endPoint_.y());
    if((xDiff > 2 * SWIPE_THRESHOLD) && (xDiff > yDiff)) {
      // horizontal swipe detected, now find direction
      if(startPoint_.x() > endPoint_.x()) {
        //left swipe
        showNextPage();
      }
      else if(startPoint_.x() < endPoint_.x()) {
        //right swipe
        showPrevPage();
      }
    }
    // vertical swipe is handled by Flickable class
  }
  else if (QEvent::ContextMenu == event->type()) {
    // * handle right click event
    showCommandPopupMenu();
  }
  else if(QEvent::KeyPress == event->type()) {
    // * handle key events
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    switch(keyEvent->key()) {
      case Qt::Key_Escape:
        if (isFullScreen() && (false == isBackground())) {
          //must be the first check
          normalScreen();
        }
        if (NULL != fileBrowser_) {
          fileBrowser_->close();
          fileBrowser_ = NULL;
        }
        if (NULL != gotoPage_) {
          gotoPage_->close();
          gotoPage_ = NULL;
        }
        if (NULL != zoomPage_) {
          zoomPage_->close();
          zoomPage_ = NULL;
        }
        if (NULL != commandPopupMenu_) {
          commandPopupMenu_->close();
          commandPopupMenu_ = NULL;
        }
        if (NULL != aboutDialog_) {
          aboutDialog_->close();
          aboutDialog_ = NULL;
        }
        break;
      case Qt::Key_Right:
        if (false == isBackground()) {
          showNextPage();
        }
        out = true;//don't propagate this event
        break;
      case Qt::Key_Left:
        if (false == isBackground()) {
          showPrevPage();
        }
        out = true;//don't propagate this event
        break;
      case Qt::Key_Up:
      case Qt::Key_Down:
        out = isBackground();
        break;
      case Qt::Key_Home:
        if((0 != document_->currentPage()) && (true == animationFinished_) &&
            (false == isBackground())) {
          //not at the beginning of the document
          animationFinished_ = false;
          gotoPage(1, document_->numPages());
          slidingStacked_->slideInPrev();
        }
        break;
      case Qt::Key_End:
        {
          int numPages = document_->numPages();
          if(((numPages - 1) != document_->currentPage()) && (true == animationFinished_) &&
              (false == isBackground())) {
            //not at the end of the document
            animationFinished_ = false;
            gotoPage(numPages, numPages);
            slidingStacked_->slideInNext();
          }
        }
        break;
    }
  }
  return out;
}

bool Window::showNextPage()
{
  qDebug() << "Window::showNextPage";

  bool out = false;

  if(true == document_->isLoaded() && true == animationFinished_) {
    currentPage_ = document_->currentPage() + 2;
    if(currentPage_ <= document_->numPages()) {
      //start timer
      waitTimer_->start();
      //load a new page
      animationFinished_ = false;
      document_->setPage(currentPage_);
      document_->showCurrentPageUpper();
      //update the cache after the page has been displayed
      document_->sendPageRequest(currentPage_);
      //make sure that the next page is ready
      slidingStacked_->slideInNext();
      out = true;
    }
  }

  return out;
}

bool Window::showPrevPage()
{
  qDebug() << "Window::showPrevPage";

  bool out = false;

  if(true == document_->isLoaded() && true == animationFinished_) {
    currentPage_ = document_->currentPage();
    if(0 < currentPage_) {
      //start timer
      waitTimer_->start();
      //load a new page
      animationFinished_ = false;
      document_->setPage(currentPage_);
      document_->showCurrentPageLower();
      //update the cache after the page has been displayed
      document_->sendPageRequest(currentPage_ - 2);
      //make sure that the prev page is ready
      slidingStacked_->slideInPrev();
      out = true;
    }
  }

  return out;
}

void Window::closeEvent(QCloseEvent *evt)
{
  qDebug() << "Window::closeEvent";

  saveSettings();

  QWidget::closeEvent(evt);
}

void Window::onAnimationFinished()
{
  qDebug() << "Window::onAnimationFinished";
  closeWaitPage();
  animationFinished_ = true;//must be the last statement
}

void Window::setupDocDisplay(unsigned int pageNumber, qreal factor)
{
  qDebug() << "Window::setupDocDisplay" << pageNumber;
  //set document zoom factor
  setScale(factor);
  //set current page
  gotoPage(pageNumber, document_->numPages());
}

void Window::gotoPage(int pageNb, int numPages)
{
  qDebug() << "Window::gotoPage: page nb" << pageNb << ", numPages" << numPages;

  //set current page
  if(true == document_->invalidatePageCache(pageNb - 1)) {
    document_->setPage(pageNb);
  }
  //preload next page
  if((numPages - pageNb) > 0) {
    qDebug() << "next page";
    document_->sendPageRequest(pageNb);//next page (index starts from 0)
  }
  //preload previous page
  if(pageNb > 1) {
    qDebug() << "previous page";
    document_->sendPageRequest(pageNb - 2); //previous page (index starts from 0)
  }
}

void Window::updateView(qreal factor, bool force)
{
  qDebug() << "Window::updateView";

  if (false == animationFinished_) {
    qDebug() << "unfinished animation";
    return;
  }

  //set zoom factor
  if (false == force) {
    if (document_->scale() == factor) {
      return;
    }
    setScale(factor);
  }

  //update all pages from circular buffer
  animationFinished_ = false;
  gotoPage(document_->currentPage()+1, document_->numPages());

  //update view
  document_->showCurrentPageUpper();
  slidingStacked_->slideInNext();
}

void Window::showHelp(bool slideNext)
{
  qDebug() << "Window::showHelp";

  if (false == animationFinished_) {
    qDebug() << "unfinished animation";
    return;
  }

  const QString *curFileName = &helpFile_;
  int curPage = 1;

  if (0 == prev_.page) {
    //store the current file name and page number
    prev_.fileName = document_->filePath();
    prev_.page = document_->currentPage()+1;
  }
  else {
    //restore previous file name and page number
    curFileName = &(prev_.fileName);
    curPage = prev_.page;
    prev_.page = 0;
  }

  if(document_->setDocument(*curFileName)) {
    animationFinished_ = false;
    waitTimer_->start();
    setupDocDisplay(curPage, document_->scale());
    document_->showCurrentPageUpper();
    if(true == slideNext) {
      slidingStacked_->slideInNext();
    }
    bool showHelp = (helpFile_ != *curFileName) || (0 == prev_.page);
    setHelpIcon(showHelp);
  }
  else {
    qDebug() << "cannot open help file";
    prev_.page = 0;
    showWarningMessage(tr("Cannot open help file"));
  }
}

void Window::showAboutPage()
{
  qDebug() << "Window::showAboutPage";
  if(NULL == aboutDialog_) {
    aboutDialog_ = new QDeclarativeView(this);
    aboutDialog_->setSource(QUrl("qrc:/qml/qml/aboutdialog.qml"));
    aboutDialog_->setStyleSheet("background:transparent");
    aboutDialog_->setAttribute(Qt::WA_TranslucentBackground);
    aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);
    aboutDialog_->setWindowFlags(Qt::FramelessWindowHint);
    connect(aboutDialog_->engine(), SIGNAL(quit()), this, SLOT(closeAboutPage()));
    QObject *pAbout = aboutDialog_->rootObject();
    if(NULL != pAbout) {
      pAbout->setProperty("height", height());
      pAbout->setProperty("width", width());
      QObject *pAboutDlg = pAbout->findChild<QObject*>("aboutDialog");
      if(NULL != pAboutDlg) {
        pAboutDlg->setProperty("text", tr("<H2>tabletReader v%1</H2>"
                                          "<H3>e-book reader for touch-enabled devices</H3>"
                                          "<H4>Supported formats:</H4>"
                                          "<H4>%2</H4>"
                                          "<br>Copyright (C) 2012, Bogdan Cristea. All rights reserved.<br>"
                                          "<i>e-mail: cristeab@gmail.com</i><br><br>"
                                          "This program is distributed in the hope that it will be useful, "
                                          "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                                          "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                                          "GNU General Public License for more details.<br><br>").arg(TR_VERSION).
            arg(document_->supportedFilePatterns().join(", ")));
      }
      else {
        qDebug() << "cannot get aboutDialog object";
      }
      aboutDialog_->show();
    }
    else {
      qDebug() << "cannot get aboutDialog object";
      delete aboutDialog_;
      aboutDialog_ = NULL;
    }
  }
}

void Window::closeAboutPage()
{
  qDebug() << "Window::closeAboutPage";
  if((NULL != aboutDialog_) && (true == aboutDialog_->close())) {
    qDebug() << "widget closed";
    aboutDialog_ = NULL;
  }
}

void Window::showWarningMessage(const QString &title, const QString &explanation)
{
  qDebug() << "Window::showWarningMessage";
  if(NULL == aboutDialog_) {
    aboutDialog_ = new QDeclarativeView(this);
    aboutDialog_->setSource(QUrl("qrc:/qml/qml/aboutdialog.qml"));
    aboutDialog_->setStyleSheet("background:transparent");
    aboutDialog_->setAttribute(Qt::WA_TranslucentBackground);
    aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);
    aboutDialog_->setWindowFlags(Qt::FramelessWindowHint);
    connect(aboutDialog_->engine(), SIGNAL(quit()), this, SLOT(closeAboutPage()));
    QObject *pRoot = aboutDialog_->rootObject();
    if(NULL != pRoot) {
      if (false  == pRoot->setProperty("height", height())) {
        qDebug() << "cannot set height";
      }
      if (false == pRoot->setProperty("width", width())) {
        qDebug() << "cannot set width";
      }
      QObject *pAbout = pRoot->findChild<QObject*>("aboutDialog");
      if(NULL != pAbout) {
        pAbout->setProperty("text", "<H3 style=\"color:red\">" + title +
                            "</H3><br>" + explanation + "<br>");
      }
      else {
        qDebug() << "cannot get aboutDialog object";
      }
      aboutDialog_->show();
    }
    else {
      qDebug() << "cannot get aboutDialog object";
      delete aboutDialog_;
      aboutDialog_ = NULL;
    }
  }
}

void Window::showWaitPage()
{
  qDebug() << "Window::showWaitPage";
  waitTimer_->stop();
  if(NULL == waitDialog_) {
    waitDialog_ = new QDeclarativeView(this);
    waitDialog_->setSource(QUrl("qrc:/qml/qml/waitdialog.qml"));
    waitDialog_->setStyleSheet("background:transparent");
    waitDialog_->setAttribute(Qt::WA_TranslucentBackground);
    waitDialog_->setAttribute(Qt::WA_DeleteOnClose);
    waitDialog_->setWindowFlags(Qt::FramelessWindowHint);
    QObject *pRoot = waitDialog_->rootObject();
    if(NULL != pRoot) {
      pRoot->setProperty("height", height());
      pRoot->setProperty("width", width());
      waitDialog_->show();
    }
    else {
      qDebug() << "cannot get waitDialog object";
      delete waitDialog_;
      waitDialog_ = NULL;
    }
  }
}

void Window::closeWaitPage()
{
  qDebug() << "Window::closeWaitPage";
  waitTimer_->stop();
  if((NULL != waitDialog_) && (true == waitDialog_->close())) {
    qDebug() << "widget closed";
    waitDialog_ = NULL;
  }
  else {
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

void Window::showPropertiesPage()
{
  qDebug() << "Window::showPropertiesPage";
  if(NULL == aboutDialog_) {
    aboutDialog_ = new QDeclarativeView(this);
    aboutDialog_->setSource(QUrl("qrc:/qml/qml/aboutdialog.qml"));
    aboutDialog_->setStyleSheet("background:transparent");
    aboutDialog_->setAttribute(Qt::WA_TranslucentBackground);
    aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);
    aboutDialog_->setWindowFlags(Qt::FramelessWindowHint);
    connect(aboutDialog_->engine(), SIGNAL(quit()), this, SLOT(closeAboutPage()));
    QObject *pAbout = aboutDialog_->rootObject();
    if(NULL != pAbout) {
      pAbout->setProperty("height", height());
      pAbout->setProperty("width", width());
      QObject *pAboutDlg = pAbout->findChild<QObject*>("aboutDialog");
      if(NULL != pAboutDlg) {
        //document path
        QString msg = tr("<H3>Document path:<br><i>%1</i></H3>").arg(document_->filePath());
        //current page / page number
        msg += tr("<H3>Current page / Number of pages:<br><b>%1 / %2</b></H3>").arg(document_->currentPage() + 1).arg(document_->numPages());
        //time since the application was started
        msg += tr("<H3>Elapsed time:<br>%1</H3>").arg(elapsedTime());
        //battery state
#ifndef NO_QTMOBILITY
        msg += tr("<H3>Battery status:<br>%1</H3>").arg(batteryStatus());
#endif
        //set message
        pAboutDlg->setProperty("text", msg);
      }
      else {
        qDebug() << "cannot get aboutDialog object";
      }
      aboutDialog_->show();
    }
    else {
      qDebug() << "cannot get aboutDialog object";
      delete aboutDialog_;
      aboutDialog_ = NULL;
    }
  }
}

bool Window::hasTouchScreen()
{
#ifndef NO_QTMOBILITY
  QSystemDeviceInfo systemInfo;
  QSystemDeviceInfo::InputMethodFlags flags = systemInfo.inputMethodType();
  return ((flags & (QSystemDeviceInfo::SingleTouch |
                    QSystemDeviceInfo::MultiTouch)) != 0) ? true : false;
#else
  return false;
#endif
}

QString Window::batteryStatus()
{
  QString msg("N/A");
#ifndef NO_QTMOBILITY
  switch(batteryInfo_->chargerType()) {
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
  if(-1 != remCap) {
    msg += tr(", %1% remaining capacity").arg(remCap);
  }
  else {
    msg += tr(", unknown remaining capacity");
  }
#endif
  return msg;
}

QString Window::elapsedTime()
{
  QString msg = "";
  qint64 eTimeMs = eTime_.elapsed();
  qint64 eTimeHrs = eTimeMs / (1000 * 3600);
  if(0 != eTimeHrs) {
    if(1 == eTimeHrs) {
      msg += tr("1 hour");
    }
    else {
      msg += tr("%1 hours").arg(eTimeHrs);
    }
  }
  qint64 eTimeMin = (eTimeMs - eTimeHrs * 1000 * 3600) / (1000 * 60);
  if(0 != eTimeMin) {
    if(0 != eTimeHrs) {
      msg += ", ";
    }
    if(1 == eTimeMin) {
      msg += tr("1 minute");
    }
    else {
      msg += tr("%1 minutes").arg(eTimeMin);
    }
  }
  qint64 eTimeSec = (eTimeMs - eTimeHrs * 1000 * 3600 - eTimeMin * 1000 * 60) / 1000;
  if(0 != eTimeSec) {
    if((0 != eTimeMin) || (0 != eTimeHrs)) {
      msg += ", ";
    }
    if(1 == eTimeSec) {
      msg += tr("1 second");
    }
    else {
      msg += tr("%1 seconds").arg(eTimeSec);
    }
  }
  return msg;
}

void Window::saveSettings()
{
  if((NULL != document_) && (true == document_->isLoaded())) {
    const QString &fileName = document_->filePath();
    if (fileName != helpFile_) {
      QSettings settings(ORGANIZATION, APPLICATION);
      settings.setValue(KEY_PAGE, document_->currentPage());
      settings.setValue(KEY_FILE_PATH, fileName);
      settings.setValue(KEY_ZOOM_LEVEL, document_->scale());
    }
  }
}

void Window::addShortcutKeys()
{
  //open file
  QAction *openAction = new QAction(this);
  openAction->setShortcut(Qt::CTRL+Qt::Key_O);
  addAction(openAction);
  connect(openAction, SIGNAL(triggered()), this, SLOT(showFileBrowser()));

  //full screen
  QAction *fullScreenAction = new QAction(this);
  fullScreenAction->setShortcut(Qt::CTRL+Qt::Key_U);
  addAction(fullScreenAction);
  connect(fullScreenAction, SIGNAL(triggered()), this, SLOT(fullScreen()));

  //go to page
  QAction *gotoAction = new QAction(this);
  gotoAction->setShortcut(Qt::CTRL+Qt::Key_G);
  addAction(gotoAction);
  connect(gotoAction, SIGNAL(triggered()), this, SLOT(showGotoPage()));

  //zoom
  QAction *zoomAction = new QAction(this);
  zoomAction->setShortcut(Qt::CTRL+Qt::Key_Z);
  addAction(zoomAction);
  connect(zoomAction, SIGNAL(triggered()), this, SLOT(showZoomPage()));

  //properties
  QAction *propertiesAction = new QAction(this);
  propertiesAction->setShortcut(Qt::CTRL+Qt::Key_P);
  addAction(propertiesAction);
  connect(propertiesAction, SIGNAL(triggered()), this, SLOT(showPropertiesPage()));

  //help
  QAction *helpAction = new QAction(this);
  helpAction->setShortcut(Qt::Key_F1);//TODO: use also Ctrl+H
  addAction(helpAction);
  connect(helpAction, SIGNAL(triggered()), this, SLOT(showHelp()));

  //back
  QAction *backAction = new QAction(this);
  backAction->setShortcut(Qt::ALT+Qt::Key_Left);
  addAction(backAction);
  connect(backAction, SIGNAL(triggered()), this, SLOT(onBack()));

  //about
  QAction *aboutAction = new QAction(this);
  aboutAction->setShortcut(Qt::CTRL+Qt::Key_A);
  addAction(aboutAction);
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAboutPage()));

  //exit
  QAction *exitAction = new QAction(this);
  exitAction->setShortcut(Qt::CTRL+Qt::Key_Q);
  addAction(exitAction);
  connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void Window::onBack()
{
  qDebug() << "onBack";

  if (0 != prev_.page) {
    showHelp();
  }
}
