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

#include "config.h"
#ifndef NO_QTMOBILITY
#include <QtSystemInfo/QSystemDeviceInfo>
#include <QtSystemInfo/QSystemBatteryInfo>
#endif
#include "window.h"
#include "filebrowsermodel.h"
#include "qmlcppmediator.h"
#include "settings_keys.h"

#define HELP_FILE "/../share/doc/tabletReaderHelp.pdf"

#ifndef NO_QTMOBILITY
QTM_USE_NAMESPACE
#endif

Window::Window()
  : document_(NULL),
    fileBrowserModel_(NULL),
#ifndef NO_QTMOBILITY
    batteryInfo_(NULL),
#endif
    fullScreen_(false),
    helpFile_(QCoreApplication::applicationDirPath()+QString(HELP_FILE)),
    rootObj_(NULL),
    mediator_(NULL)
{
  prev_.fileName = "";
  prev_.page = 0;

  setResizeMode(QDeclarativeView::SizeRootObjectToView);

  eTime_.start();//used to measure the elapsed time since the app was started
  QApplication::setOverrideCursor(Qt::WaitCursor);

#ifndef NO_QTMOBILITY
  //battery status
  batteryInfo_ = new QSystemBatteryInfo(this);
#endif

  //get settings if any
  QString filePath;
  int page = -1;
  qreal scaleFactor = 0;
  int scaleIndex = 0;
  loadSettings(filePath, page, scaleFactor, scaleIndex);
  bool foundDoc = true;
  if (NULL == filePath) {
    filePath = helpFile_;
    foundDoc = false;
  }

  //load previous document if any or the help
  document_ = new PageProvider();//deleted by QDeclarativeView
  bool loadRes = document_->setDocument(filePath);
  document_->setScale(scaleFactor, scaleIndex);

  normalScreen();//need to set early the window width in order to use the fit width zoom factor

  //create file browser (uses supported file types given by OkularDocument)
  const QStringList &formats = document_->supportedFilePatterns();
  fileBrowserModel_ = new FileBrowserModel(this, document_, formats);
  if (foundDoc) {
    fileBrowserModel_->setCurrentDir(filePath);
  }

  //add page provider before setting the source
  QDeclarativeEngine *eng = engine();
  if (NULL != eng) {
    eng->addImageProvider("pageprovider", document_);
    eng->rootContext()->setContextProperty("pdfPreviewModel", fileBrowserModel_);
    connect(eng, SIGNAL(quit()), this, SLOT(onQuit()));
  }
  //set context property
  mediator_ = new QmlCppMediator(document_, fileBrowserModel_);
  setWindowSize(normScrGeometry_.width(), normScrGeometry_.height());
  rootContext()->setContextProperty("mediator", mediator_);

  setSource(QUrl("qrc:/qml/qml/main.qml"));
  rootObj_ = rootObject();

  if (NULL != rootObj_) {
    //setup GUI
    setZoomIndex(scaleIndex);
    //set about props
    if (false == rootObj_->setProperty("version", TR_VERSION)) {
      qDebug() << "cannot set version";
    }
    if (false == rootObj_->setProperty("formats", formats.join(", "))) {
      qDebug() << "cannot set formats";
    }
    //set filebrowser handlers
    connect(rootObj_, SIGNAL(chDir(int)), fileBrowserModel_, SLOT(changeCurrentDir(int)));
    connect(rootObj_, SIGNAL(showDoc(QString,int)), this, SLOT(onShowDocument(QString,int)));
    //set full screen button handlers
    connect(rootObj_, SIGNAL(fullScreen()), this, SLOT(onFullScreen()));
    connect(rootObj_, SIGNAL(normalScreen()), this, SLOT(onNormalScreen()));
    //set goto page handler
    connect(rootObj_, SIGNAL(gotoPage(int)), this, SLOT(onGotoPage(int)));
    //set zoom handler
    connect(rootObj_, SIGNAL(setZoomFactor(int,int)), this, SLOT(onSetZoomFactor(int,int)));
    //set properties handler
    connect(rootObj_, SIGNAL(setProperties()), this, SLOT(onSetProperties()));
    //set help handler
    connect(rootObj_, SIGNAL(showHelp(bool)), this, SLOT(onShowHelp(bool)));
    //set quit handler
    connect(rootObj_, SIGNAL(quit()), this, SLOT(onQuitApp()));
    //set wait handler
    connect(this, SIGNAL(wait()), rootObj_, SLOT(onWait()));
    //set warning handler
    connect(this, SIGNAL(warning(QVariant)), rootObj_, SLOT(onWarning(QVariant)));
  }

  if (true == loadRes) {
    gotoPage(page, document_->numPages());
    onQuit();
    refreshPage();
  }
  else {
    //no document has been found, just show a warning
    emit warning(tr("No document found"));
  }

  Qt::WindowFlags winFlags = Qt::Window | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint;
#ifndef DESKTOP_APP
  winFlags |= Qt::X11BypassWindowManagerHint;
#endif
  setWindowFlags(winFlags);
  setWindowTitle("TabletReader " TR_VERSION);

  QApplication::restoreOverrideCursor();
}

Window::~Window()
{
  delete fileBrowserModel_;
  delete mediator_;
}

void Window::onShowDocument(const QString &doc, int page)
{
  qDebug() << "Window::onShowDocument" << doc << ", page" << page;

  if(fileBrowserModel_->closeFileBrowserText() != doc) {
    openFile(doc, page);
  }
  else {
    onQuit();
  }
}

void Window::onGotoPage(int page)
{
  qDebug() << "Window::onGotoPage: " << page;

  //set current page
  int currentPage = document_->currentPage();
  int numPages = document_->numPages();
  if((page != currentPage) && (0 <= page) && (page < numPages)) {
    //change page
    gotoPage(page, numPages);
    if(currentPage < page) {
      showNextPage();
    }
    else {
      showPrevPage();
    }
  }
  onQuit();
}

void Window::onSetZoomFactor(int value, int index)
{
  qDebug() << "Window::onSetZoomFactor" << value << ", index" << index;

  //convert value to qreal
  qreal scale = value/100.0;//fit width is coded by -100
  if (scale != document_->scaleFactor()) {
    document_->setScale(scale, index);
    setZoomIndex(index);
    gotoPage(document_->currentPage(), document_->numPages());
    refreshPage();
  }
  onQuit();
}

void Window::openFile(const QString &filePath, int page)
{
  qDebug() << "Window::openFile";

  //open document
  if(document_->setDocument(filePath)) {
    //load document
    gotoPage(page, document_->numPages());
    refreshPage();
    //remove wait page if any
    onQuit();
  }
  else {
    emit warning(tr("Failed to open file") + tr("<br>%1 cannot be opened").arg(filePath));
  }
}

void Window::onFullScreen()
{
  qDebug() << "Window::fullScreen";

  if (true == fullScreen_) {
    return;
  }
  normScrGeometry_ = geometry();//store current geometry

  if(true == hasTouchScreen()) {
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
  }
  int width = QApplication::desktop()->width();
  int height = QApplication::desktop()->height();
  qDebug() << "width " << width << ", height " << height;
#ifndef _MSC_VER
  setGeometry(QRect(0, 0, width, height));//TODO: meant only for AP
#endif
  //set QML size
  setWindowSize(width, height);
  //set window width for fit width
  document_->setWinWidth(width);
  //update page width if needed
  updateViewForFitWidth();

  showFullScreen();//seems to work only on desktops

  fullScreen_ = true;
  onQuit();
}

void Window::normalScreen()
{
  qDebug() << "Window::normalScreen";

  fullScreen_ = false;

  if (false == normScrGeometry_.isValid()) {
    //compute a geometry if none available
    int desktopWidth = QApplication::desktop()->width();
    int desktopHeight = QApplication::desktop()->height();
	int width = 0;
	int height = 0;
    if((MIN_SCREEN_WIDTH >= desktopWidth) && (MIN_SCREEN_HEIGHT >= desktopHeight)) {
      qDebug() << "using full screen mode with toolbar";
	  width = desktopWidth;
	  height = desktopHeight;
      showFullScreen();
      if(true == hasTouchScreen()) {
        QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
      }
    }
    else {
      qDebug() << "using normal mode";
      width = (MIN_SCREEN_WIDTH < desktopWidth) ? MIN_SCREEN_WIDTH : desktopWidth;
      height = (MIN_SCREEN_HEIGHT < desktopHeight) ? MIN_SCREEN_HEIGHT : desktopHeight;
      showNormal();
      if(true == hasTouchScreen()) {
        QApplication::restoreOverrideCursor();
      }
	  move((desktopWidth-width)/2, (desktopHeight-height)/2);
    }
    normScrGeometry_.setWidth(width);
    normScrGeometry_.setHeight(height);
  }
  else {
    setGeometry(normScrGeometry_);
    showNormal();
  }

  //set window width for fit width
  document_->setWinWidth(normScrGeometry_.width());
}

void Window::onNormalScreen()
{
  qDebug() << "Window::onNormalScreen";

  normalScreen();
  //set QML size
  setWindowSize(normScrGeometry_.width(), normScrGeometry_.height());
  //update page width if needed
  updateViewForFitWidth();
  onQuit();
}

void Window::showNextPage()
{
  qDebug() << "Window::showNextPage";
  QObject *obj = rootObj_->findChild<QObject*>("view");
  if (NULL != obj) {
    if (false == QMetaObject::invokeMethod(obj, "incrementCurrentIndex")) {
      qDebug() << "cannot invoke method";
    }
  }
  else {
    qDebug() << "cannot find view";
  }
}

void Window::showPrevPage()
{
  qDebug() << "Window::showPrevPage";
  if (false == document_->cacheReady()) {
    qDebug() << "ignore request";
    return;
  }
  QObject *obj = rootObj_->findChild<QObject*>("view");
  if (NULL != obj) {
    if (false == QMetaObject::invokeMethod(obj, "decrementCurrentIndex")) {
      qDebug() << "cannot invoke method";
    }
  }
  else {
    qDebug() << "cannot find view";
  }
}

void Window::closeEvent(QCloseEvent *evt)
{
  qDebug() << "Window::closeEvent";

  saveSettings();

  QWidget::closeEvent(evt);
}

void Window::gotoPage(int pageNb, int numPages)
{
  qDebug() << "Window::gotoPage: page nb" << pageNb << ", numPages" << numPages;

  //show wait page
  emit wait();

  //update the entire cache
  document_->gotoPage(pageNb);

  //set current page and the total number of pages on the GUI
  setCurrentPage(pageNb, numPages);
}

void Window::onShowHelp(bool show)
{
  qDebug() << "Window::onShowHelp" << show;

  QString curFileName = "";
  int curPage = 0;

  if (true == show) {
    //store the current file name and page number
    prev_.fileName = document_->filePath();
    prev_.page = document_->currentPage();
    curFileName = helpFile_;
    curPage = 0;
  }
  else {
    //restore previous file name and page number
    curFileName = prev_.fileName;
    curPage = prev_.page;
  }

  if(document_->setDocument(curFileName)) {
    gotoPage(curPage, document_->numPages());
    refreshPage();
    onQuit();
    //display "Back" icon next time
    if (NULL != mediator_) {
      mediator_->setHlpBck(false);
    }
  }
  else {
    qDebug() << "cannot open file";
    prev_.page = 0;
    emit warning(tr("Cannot open help file"));
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

void Window::loadSettings(QString &filePath, int &page, qreal &scaleFactor, int &scaleIndex)
{
  qDebug() << "Window::loadSettings";

  QSettings settings(ORGANIZATION, APPLICATION);
  filePath = settings.value(KEY_FILE_PATH).toString();
  page = settings.value(KEY_PAGE, 0).toInt();
  if (page < 0) {
    page = 0;
  }
  scaleFactor = settings.value(KEY_ZOOM_LEVEL, FIT_WIDTH_ZOOM_FACTOR).toFloat();
  scaleIndex = settings.value(KEY_ZOOM_INDEX, FIT_WIDTH_ZOOM_INDEX).toInt();
}

void Window::saveSettings()
{
  qDebug() << "PageProvider::saveSettings";
  if(true == document_->isLoaded()) {
    const QString &fileName = document_->filePath();
    if (fileName != helpFile_) {
      QSettings settings(ORGANIZATION, APPLICATION);
      settings.setValue(KEY_PAGE, document_->currentPage());
      settings.setValue(KEY_FILE_PATH, fileName);
      settings.setValue(KEY_ZOOM_LEVEL, document_->scaleFactor());
      settings.setValue(KEY_ZOOM_INDEX, document_->scaleIndex());
    }
  }
}

void Window::onQuit()
{
  qDebug() << "Window::onQuit";
  if (NULL != rootObj_) {
    QObject *obj = rootObj_->findChild<QObject*>("mainLoader");
    if ((NULL != obj) && (false == obj->setProperty("source", ""))) {
      qDebug() << "cannot set source";
    }
    obj = rootObj_->findChild<QObject*>("toolbarmouse");
    if ((NULL != obj) && (false == obj->setProperty("enabled", true))) {
      qDebug() << "cannot set toolbar mouse area";
    }
  }
}

void Window::onQuitApp()
{
  qDebug() << "Window::onQuitApp";
  saveSettings();
  qApp->quit();
}

void Window::onSetProperties()
{
  qDebug() << "Window::onSetProperties";
  if (NULL != rootObj_) {
    if (false == rootObj_->setProperty("filePath", document_->filePath())) {
      qDebug() << "cannot set file path";
    }
    if (false == rootObj_->setProperty("elapsedTime", elapsedTime())) {
      qDebug() << "cannot set elapsed time";
    }
    if (false == rootObj_->setProperty("batteryStatus", batteryStatus())) {
      qDebug() << "cannot set battery status";
    }
  }
}
