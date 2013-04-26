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

#include <QtGui>
#include <QDebug>
#include <kmimetype.h>
#include "pageprovider.h"
#include "okulardocument.h"
#include "window.h"

PageProvider::PageProvider()
  : QObject(),
    QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap),
    doc_(new OkularDocument()),
    currentPage_(-1),
    prevPage_(-1),
    scaleFactor_(1.0),
    requestingPixmap_(false)
{
  for(int n = 0; n < CACHE_SIZE; ++n) {
    pageCache_.append(new PageCache);
    pageCache_[n]->pPixmap = NULL;
    pageCache_[n]->status = PAGE_CACHE_INVALID;
  }
  connect(this, SIGNAL(pageRequest(int, qreal)), doc_, SLOT(onPageRequest(int, qreal)));
  connect(doc_, SIGNAL(pixmapReady(int, const QPixmap*)), this, SLOT(onPixmapReady(int, const QPixmap*)));

}

PageProvider::~PageProvider()
{
  for(int n = 0; n < pageCache_.size(); ++n) {
    delete pageCache_[n];
  }
  delete doc_;
}

void PageProvider::onPixmapReady(int page, const QPixmap *pix)
{
  qDebug() << "PageProvider::onPixmapReady" << page;

  pageCache_[page % CACHE_SIZE]->pPixmap = pix;
  pageCache_[page % CACHE_SIZE]->status = PAGE_CACHE_VALID;
  if (evt_.isRunning()) {
    evt_.quit();//this is a synchronous request
  }
}

QPixmap PageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
  qDebug() << "PageProvider::requestPixmap" << id;
  requestingPixmap_ = true;//needed to make sure that a second request does not come to quickly

  const QPixmap *pPix = NULL;
  prevPage_ = currentPage_;
  currentPage_ = id.toInt();
  int width = 100;//default size
  int height =100;

  //the document is not loaded or the cache is not valid
  //the GUI should make sure that the cache is valid before issuing a request
  if ((0 == doc_->numPages()) ||
      (PAGE_CACHE_VALID != pageCache_[currentPage_%CACHE_SIZE]->status)) {
    //return a default pixmap
    qDebug() << "no document or invalid cache";
    if (size) {
      *size = QSize(width, height);
    }
    requestingPixmap_ = false;
    return QPixmap(width,height);
  }

  qDebug() << "valid cache" << currentPage_;
  pPix = pageCache_[currentPage_%CACHE_SIZE]->pPixmap;

  bool isValid = ((NULL != pPix) && (false == pPix->isNull()));
  if (true == isValid) {
    width = requestedSize.width() > 0 ? requestedSize.width() : pPix->width();
    height = requestedSize.height() > 0 ? requestedSize.height() : pPix->height();
    qDebug() << "width" << width << ", height" << height;
  }
  if (size) {
    *size = QSize(width, height);
  }

  updateCache();

  requestingPixmap_ = false;
  return (true == isValid)?*pPix:QPixmap(width, height);
}

bool PageProvider::setDocument(const QString &filePath)
{
  qDebug() << "PageProvider::setDocument" << filePath;

  bool out = false;

  if (NULL != doc_) {
    if(true == doc_->load(filePath)) {
      cacheSize_ = qMin(pageCache_.count(), numPages());
      currentPage_ = -1;
      filePath_ = filePath;
      out = true;
    }
    else if (false == filePath_.isEmpty()) {
      //an error occured -> restore previous document
      out = (EXIT_SUCCESS == doc_->load(filePath_));
    }
  }
  return out;
}

void PageProvider::updateCache()
{
  qDebug() << "PageProvider::updateCache: prev" << prevPage_ << ", current" << currentPage_;

  if (prevPage_ == currentPage_) {
    qDebug() << "do nothing";
    return;
  }
  if (1 == (currentPage_-prevPage_)) {
    //update next page
    qDebug() << "next page";
    sendPageRequest(currentPage_+1);
  }
  else if (-1 == (currentPage_-prevPage_)) {
    //update prev page
    qDebug() << "prev page";
    sendPageRequest(currentPage_-1);
  }
  else {
    qDebug() << "should not reach this point";
  }
}

void PageProvider::gotoPage(int page)
{
  qDebug() << "PageProvider::gotoPage";
  //request synchronously the current page
  sendPageRequest(page);
  evt_.exec();
  //asynchronous requests (make sure that the entire cache is marked as filled)
  if (0 <= (page-1)) {
    sendPageRequest(page-1);
  }
  else if ((numPages() > (page+2))) {
    pageCache_[(page+2) % CACHE_SIZE]->status = PAGE_CACHE_VALID;
  }
  if (numPages() > (page+1)) {
    sendPageRequest(page+1);
  }
  else if (0 <= (page-2)) {
    pageCache_[(page-2) % CACHE_SIZE]->status = PAGE_CACHE_VALID;
  }
}

