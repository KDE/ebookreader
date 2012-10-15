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
#include "filebrowsermodel.h"
#include "screen_size.h"

#define ORGANIZATION "Bogdan Cristea"
#define APPLICATION "tabletReader"
#define KEY_PAGE "current_page"
#define KEY_FILE_PATH "current_file_path"
#define KEY_ZOOM_LEVEL "current_zoom_level"
#define HELP_FILE "/../share/doc/tabletReaderHelp.pdf"

#ifndef NO_QTMOBILITY
QTM_USE_NAMESPACE
#endif

Window::Window()
  : QDeclarativeView(),
    provider_(NULL),
    fileBrowserModel_(new FileBrowserModel(this)),
    waitTimer_(NULL),
#ifndef NO_QTMOBILITY
    batteryInfo_(NULL),
#endif
    helpFile_(QCoreApplication::applicationDirPath()+QString(HELP_FILE))
{
  prev_.fileName = "";
  prev_.page = 0;

  eTime_.start();//used to measure the elapsed time since the app is started

  //zoom scale factors
  //-1 used for fit width scaling factor
  scaleFactors_ << -1 << 0.25 << 0.5 << 0.75 << 1.
                << 1.25 << 1.5 << 2. << 3. << 4.;

  //wait timer initialisation (used to handle long actions: document openings, page changes)
  waitTimer_ = new QTimer(this);
  waitTimer_->setInterval(WAIT_TIMER_INTERVAL_MS);
  connect(waitTimer_, SIGNAL(timeout()), this, SLOT(showWaitDialog()));

#ifndef NO_QTMOBILITY
  //battery status
  batteryInfo_ = new QSystemBatteryInfo(this);
#endif

#ifndef DESKTOP_APP
  setWindowFlags(Qt::X11BypassWindowManagerHint);
#endif

  showDocument();
}

void Window::showDocument()
{
  qDebug() << "Window::showDocument";

  //create page provider
  provider_ = new PageProvider(this);
  engine()->addImageProvider(QLatin1String("pages"), provider_);

  //set document if one has been previously open
  QSettings settings(ORGANIZATION, APPLICATION);
  QString filePath;
  waitTimer_->start();
  if(NULL != (filePath = settings.value(KEY_FILE_PATH).toString())) {
    filePath = "/home/bogdan/Documents/CV-simple/CV-en-simple_detail.pdf";//TODO: remove this
    qDebug() << "Found document " << filePath;
    if(provider_->setDocument(filePath)) {
      setupDocDisplay(settings.value(KEY_PAGE, 0).toInt(),
          settings.value(KEY_ZOOM_LEVEL, 1.0).toFloat());
      //configure file browser
      fileBrowserModel_->setCurrentDir(filePath);
    }
  }
  else {
    qDebug() << "no document found";
    showHelp();
  }
  setSource(QUrl("qrc:/qml/qml/main.qml"));
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
    showPropertiesDialog();
  }
  else if((tr("Help") == cmd) || (tr("Back") == cmd)) {
    showHelp();
  }
  else if(tr("About") == cmd) {
    showAboutDialog();
  }
  else if(tr("Exit") == cmd) {
    //close();
  }
  else {
    qDebug() << "unknown command" << cmd;
  }
}

void Window::showFileBrowser()
{
  qDebug() << "Window::showFileBrowser";

  /*if(NULL == fileBrowser_) {
    if(NULL == (fileBrowser_ = new QDeclarativeView(this))) {
      showWarningMessage(tr("Cannot create fileBrowser object"),
                         tr("not enough memory"));
      return;
    }
    if(NULL == fileBrowserModel_) {
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
  }*/
}

void Window::closeFileBrowser(const QString &doc)
{
  qDebug() << "Window::closeFileBrowser" << doc;

  /*if((NULL != fileBrowser_) && (true == fileBrowser_->close())) {
    qDebug() << "widget closed";
    fileBrowser_ = NULL;
    if(FileBrowserModel::closeFileBrowserText() != doc) {
      openFile(doc);
    }
  }*/
}

void Window::showGotoPage()
{
  qDebug() << "Window::showGotoPage";
  /*if(NULL == gotoPage_) {
    gotoPage_ = new QDeclarativeView(this);
    gotoPage_->setSource(QUrl("qrc:/qml/qml/gotopage.qml"));
    gotoPage_->setStyleSheet("background:transparent");
    gotoPage_->setAttribute(Qt::WA_TranslucentBackground);
    gotoPage_->setAttribute(Qt::WA_DeleteOnClose);
    gotoPage_->setWindowFlags(Qt::FramelessWindowHint);
    QObject *pRoot = gotoPage_->rootObject();
    if(NULL != pRoot) {
      pRoot->setProperty("width", width());
      pRoot->setProperty("height", height());
      connect(gotoPage_->engine(), SIGNAL(quit()), this, SLOT(closeGotoPage()));
      QObject *pDisp = pRoot->findChild<QObject*>("disp");
      if(NULL != pDisp) {
        connect(pDisp, SIGNAL(setPage(QString)), this, SLOT(closeGotoPage(QString)));
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
  }*/
}

void Window::closeGotoPage(const QString &pageNb)
{
  qDebug() << "Window::closeGotoPage: " << pageNb;

  /*if((NULL != gotoPage_) && (true == gotoPage_->close()) && (true == animationFinished_)) {
    qDebug() << "widget closed";
    gotoPage_ = NULL;
    //set current page
    bool ok = false;
    int newPageNb = pageNb.toInt(&ok);
    if(true == ok) {
      int currentPage = provider_->currentPage() + 1;
      int count = provider_->count();
      if((newPageNb != currentPage) && (0 != newPageNb) && (newPageNb <= count)) {
        //start timer
        waitTimer_->start();
        //change page
        animationFinished_ = false;
        gotoPage(newPageNb, count);
        if(currentPage < newPageNb) {
          provider_->showCurrentPageUpper();
          slidingStacked_->slideInNext();
        }
        else {
          provider_->showCurrentPageLower();
          slidingStacked_->slideInPrev();
        }
      }
    }
  }*/
}

void Window::showZoomPage()
{
  qDebug() << "Window::showZoomPage";
  /*if(NULL == zoomPage_) {
    zoomPage_ = new QDeclarativeView(this);
    zoomPage_->setSource(QUrl("qrc:/qml/qml/zoompage.qml"));
    zoomPage_->setStyleSheet("background:transparent");
    zoomPage_->setAttribute(Qt::WA_TranslucentBackground);
    zoomPage_->setAttribute(Qt::WA_DeleteOnClose);
    zoomPage_->setWindowFlags(Qt::FramelessWindowHint);
    connect(zoomPage_->engine(), SIGNAL(quit()), this, SLOT(closeZoomPage()));
    QObject *pRoot = zoomPage_->rootObject();
    if(NULL != pRoot) {
      pRoot->setProperty("height", height());
      pRoot->setProperty("width", width());
      QObject *pZoomReel = pRoot->findChild<QObject*>("zoomreel");
      if(NULL != pZoomReel) {
        if(false == pZoomReel->setProperty("zoomIndex", scaleFactors_.indexOf(provider_->scale()))) {
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
  }*/
}

void Window::closeZoomPage(int /*index*/)
{
  qDebug() << "Window::closeZoomPage";
  /*if((NULL != zoomPage_) && (true == zoomPage_->close())) {
    qDebug() << "widget closed";
    zoomPage_ = NULL;
    if((0 <= index) && (index < scaleFactors_.size())) {
      updateView(scaleFactors_[index]);
    }
  }*/
}

void Window::showCommandPopupMenu()
{
  qDebug() << "Window::showCommandPopupMenu";
  /*if(false == toolBar_->isHidden()) {
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
  }*/
}

void Window::closeCommandPopupMenu(const QString &cmd)
{
  qDebug() << "Window::closeCommandPopupMenu" << cmd;
  /*if((NULL != commandPopupMenu_) && (true == commandPopupMenu_->close())) {
    qDebug() << "widget closed";
    commandPopupMenu_ = NULL;
    if(tr("Open") == cmd) {
      showFileBrowser();
    }
    else if(tr("Go To Page") == cmd) {
      showGotoPage();
    }
    else if(tr("Properties") == cmd) {
      showPropertiesDialog();
    }
    else if(tr("Zoom") == cmd) {
      showZoomPage();
    }
    else if(tr("Exit") == cmd) {
      close();
    }
    else if(tr("Normal Screen") == cmd) {
      normalScreen();
    }
    else {
      qDebug() << "unknown command" << cmd;
    }
  }*/
}

void Window::openFile(const QString &filePath)
{
  qDebug() << "Window::openFile";

  //open document
  waitTimer_->start();
  if(provider_->setDocument(filePath)) {
    //load document
    setupDocDisplay(1, provider_->scale());
    setHelpIcon(true, false);
  }
  else {
    closeWaitDialog();
    showWarningMessage(QString(APPLICATION " - ") + tr("Failed to open file"),
                       tr("%1 cannot be opened").arg(filePath));
  }
}

void Window::fullScreen()
{
  qDebug() << "Window::fullScreen";
  //toolBar_->hide();
  //showFullScreen();
  if(true == hasTouchScreen()) {
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
  }
}

void Window::normalScreen()
{
  qDebug() << "Window::normalScreen";

  QDesktopWidget *pDesktop = QApplication::desktop();
  if(NULL != pDesktop) {
    int width = pDesktop->width();
    int height = pDesktop->height();
    if((FULL_SCREEN_WIDTH >= width) || (FULL_SCREEN_HEIGHT >= height)) {
      qDebug() << "using full screen mode with toolbar";
      //resize(width, height);
      //showFullScreen();
      if(true == hasTouchScreen()) {
        QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
      }
    }
    else {
      qDebug() << "using normal mode";
      //showNormal();
      width = (MIN_SCREEN_WIDTH < width) ? MIN_SCREEN_WIDTH : width;
      height = (MIN_SCREEN_HEIGHT < height) ? MIN_SCREEN_HEIGHT : height;
      //resize(width, height);
      if(true == hasTouchScreen()) {
        QApplication::restoreOverrideCursor();
      }
    }
  }
}

void Window::closeEvent(QCloseEvent* /*evt*/)
{
  qDebug() << "Window::closeEvent";

  saveSettings();
}

void Window::setupDocDisplay(unsigned int pageNumber, qreal factor)
{
  qDebug() << "Window::setupDocDisplay" << pageNumber;
  //set document zoom factor
  setScale(factor);
  //set current page
  gotoPage(pageNumber, provider_->count());
}

void Window::gotoPage(int pageNb, int count)
{
  qDebug() << "Window::gotoPage: page nb" << pageNb << ", count" << count;

  //set current page
  if(true == provider_->invalidatePageCache(pageNb)) {
    provider_->setPage(pageNb);
  }
  //preload next page
  if(count > (pageNb+1)) {
    qDebug() << "next page";
    provider_->sendPageRequest(pageNb+1);//next page (index starts from 0)
  }
  //preload previous page
  if(pageNb > 0) {
    qDebug() << "previous page";
    provider_->sendPageRequest(pageNb-1); //previous page (index starts from 0)
  }
}

void Window::updateView(qreal factor)
{
  qDebug() << "Window::updateView";

  //set zoom factor
  if (provider_->scale() == factor) {
    return;//nothing to do
  }
  setScale(factor);

  //update all pages from circular buffer
  gotoPage(provider_->currentPage(), provider_->count());
}

void Window::showHelp()
{
  qDebug() << "Window::showHelp";

  const QString *curFileName = &helpFile_;
  int curPage = 0;

  if (-1 == prev_.page) {
    //store the current file name and page number
    prev_.fileName = provider_->filePath();
    prev_.page = provider_->currentPage();
  }
  else {
    //restore previous file name and page number
    curFileName = &(prev_.fileName);
    curPage = prev_.page;
    prev_.page = -1;
  }

  if(provider_->setDocument(*curFileName)) {
    setupDocDisplay(curPage, provider_->scale());
    //setHelpIcon(helpFile_ != *curFileName);
  }
  else {
    qDebug() << "cannot open help file";
    prev_.page = -1;
    showWarningMessage(tr("Cannot open help file"));
  }
}

void Window::showAboutDialog()
{
  qDebug() << "Window::showAboutDialog";
  /*if(NULL == aboutDialog_) {
    aboutDialog_ = new QDeclarativeView(this);
    aboutDialog_->setSource(QUrl("qrc:/qml/qml/aboutdialog.qml"));
    aboutDialog_->setStyleSheet("background:transparent");
    aboutDialog_->setAttribute(Qt::WA_TranslucentBackground);
    aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);
    aboutDialog_->setWindowFlags(Qt::FramelessWindowHint);
    connect(aboutDialog_->engine(), SIGNAL(quit()), this, SLOT(closeAboutDialog()));
    QObject *pAbout = aboutDialog_->rootObject();
    if(NULL != pAbout) {
      pAbout->setProperty("height", height());
      pAbout->setProperty("width", width());
      QObject *pAboutDlg = pAbout->findChild<QObject*>("aboutDialog");
      if(NULL != pAboutDlg) {
        pAboutDlg->setProperty("text", tr("<H2>tabletReader v%1</H2>"
                                          "<H3>e-book reader for touch-enabled devices</H3>"
                                          "<H4>Supported formats: all Okular supported formats.</H4>"
                                          "<H4>(e.g. PDF, CHM, DJVU, EPUB, etc)</H4><br>"
                                          "Copyright (C) 2012, Bogdan Cristea. All rights reserved.<br>"
                                          "<i>e-mail: cristeab@gmail.com</i><br><br>"
                                          "This program is distributed in the hope that it will be useful, "
                                          "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                                          "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                                          "GNU General Public License for more details.<br><br>").arg(TR_VERSION));
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
  }*/
}

void Window::closeAboutDialog()
{
  qDebug() << "Window::closeAboutDialog";
  /*if((NULL != aboutDialog_) && (true == aboutDialog_->close())) {
    qDebug() << "widget closed";
    aboutDialog_ = NULL;
  }*/
}

void Window::showWarningMessage(const QString& /*title*/, const QString& /*explanation*/)
{
  qDebug() << "Window::showWarningMessage";
  /*if(NULL == aboutDialog_) {
    aboutDialog_ = new QDeclarativeView(this);
    aboutDialog_->setSource(QUrl("qrc:/qml/qml/aboutdialog.qml"));
    aboutDialog_->setStyleSheet("background:transparent");
    aboutDialog_->setAttribute(Qt::WA_TranslucentBackground);
    aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);
    aboutDialog_->setWindowFlags(Qt::FramelessWindowHint);
    connect(aboutDialog_->engine(), SIGNAL(quit()), this, SLOT(closeAboutDialog()));
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
  }*/
}

void Window::showWaitDialog()
{
  qDebug() << "Window::showWaitDialog";
  waitTimer_->stop();
  /*if(NULL == waitDialog_) {
    waitDialog_ = new QDeclarativeView(this);
    waitDialog_->setSource(QUrl("qrc:/qml/qml/waitpage.qml"));
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
  }*/
}

void Window::closeWaitDialog()
{
  qDebug() << "Window::closeWaitDialog";
  waitTimer_->stop();
  /*if((NULL != waitDialog_) && (true == waitDialog_->close())) {
    qDebug() << "widget closed";
    waitDialog_ = NULL;
  }
  else {
    qDebug() << "nothing to do";
  }*/
}

void Window::showPropertiesDialog()
{
  qDebug() << "Window::showPropertiesDialog";
  /*if(NULL == aboutDialog_) {
    aboutDialog_ = new QDeclarativeView(this);
    aboutDialog_->setSource(QUrl("qrc:/qml/qml/aboutdialog.qml"));
    aboutDialog_->setStyleSheet("background:transparent");
    aboutDialog_->setAttribute(Qt::WA_TranslucentBackground);
    aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);
    aboutDialog_->setWindowFlags(Qt::FramelessWindowHint);
    connect(aboutDialog_->engine(), SIGNAL(quit()), this, SLOT(closeAboutDialog()));
    QObject *pAbout = aboutDialog_->rootObject();
    if(NULL != pAbout) {
      pAbout->setProperty("height", height());
      pAbout->setProperty("width", width());
      QObject *pAboutDlg = pAbout->findChild<QObject*>("aboutDialog");
      if(NULL != pAboutDlg) {
        //document path
        QString msg = tr("<H3>Document path:<br><i>%1</i></H3>").arg(provider_->filePath());
        //current page / page number
        msg += tr("<H3>Current page / Number of pages:<br><b>%1 / %2</b></H3>").arg(provider_->currentPage() + 1).arg(provider_->count());
        //time since the application was started
        msg += tr("<H3>Elapsed time:<br>%1</H3>").arg(elapsedTime());
        //battery state
        msg += tr("<H3>Battery status:<br>%1</H3>").arg(batteryStatus());
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
  }*/
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
  if((NULL != provider_) && (true == provider_->isLoaded())) {
    const QString &fileName = provider_->filePath();
    if (fileName != helpFile_) {
      QSettings settings(ORGANIZATION, APPLICATION);
      settings.setValue(KEY_PAGE, provider_->currentPage());
      settings.setValue(KEY_FILE_PATH, fileName);
      settings.setValue(KEY_ZOOM_LEVEL, provider_->scale());
    }
  }
}

