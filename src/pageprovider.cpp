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
#include <kmimetype.h>
#include "pageprovider.h"
#include "okulardocument.h"
#include "window.h"

PageProvider::PageProvider(QDeclarativeView *view)
  : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap),
    view_(view),
    doc_(new OkularDocument(this)),
    currentPage_(-1),
    currentIndex_(-1),
    numPages_(0),
    scaleFactor_(1.0),
    currentScrollArea_(NULL),
    evtPage_(-1)
{
  for(int n = 0; n < CACHE_SIZE; ++n) {
    pageCache_.append(new PageCache);
    pageCache_[n]->pPixmap = NULL;
    pageCache_[n]->valid = false;
  }
}

PageProvider::~PageProvider()
{
  for(int n = 0; n < pageCache_.size(); ++n) {
    delete pageCache_[n];
  }
  delete doc_;
}

void PageProvider::pixmapReady(int page, const QPixmap *pix)
{
  qDebug() << "PageProvider::pixmapReady";

  pageCache_[page % CACHE_SIZE]->pPixmap = pix;
  pageCache_[page % CACHE_SIZE]->valid = true;
  if (page == evtPage_) {
    if (true == evtLoop_.isRunning()) {//racing conditions should be managed by the upper layers
      evtLoop_.quit();//used for synchronous page requests
    }
    evtPage_ = -1;
  }
}

void PageProvider::setPage(int page, bool force)
{
  qDebug() << "PageProvider::setPage" << page;

  currentPage_ = page;

  //remove previous page from cache if requested
  if ((true == force) && (false == invalidatePageCache(page))) {
    return;
  }

  //update cache if needed
  if(false == pageCache_[currentPage_ % CACHE_SIZE]->valid) {
    qDebug() << "PageProvider::setPage: invalid cache"; 
    evtPage_ = currentPage_;//prepare to wait synchronously this page
    doc_->pageRequest(currentPage_, scaleFactor_);
    if (-1 != evtPage_) {
      evtLoop_.exec();//wait to receive the page pixmap
    }
  }
  else {
    qDebug() << "page" << page << "already in cache";
  }
}

void PageProvider::setNextPage()
{
  qDebug() << "PageProvider::setNextPage";
  if ((currentPage_+1) < numPages_) {
    setPage(currentPage_+1);
    sendPageRequest(currentPage_+1);
  }
}

void PageProvider::setPrevPage()
{
  qDebug() << "PageProvider::setPrevPage";
  if ((currentPage_-1) >= 0) {
    setPage(currentPage_-1);
    sendPageRequest(currentPage_-1);
  }
}

QPixmap PageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
  qDebug() << "PageProvider::requestPixmap" << id;

  Q_UNUSED(size);
  Q_UNUSED(requestedSize);

  (currentPage_ < id.toInt())?setNextPage():setPrevPage();

  return *(pageCache_[currentPage_ % CACHE_SIZE]->pPixmap);
}

void PageProvider::setDataModel()
{
  QStringList pageIDs;
  for (int i = 0; i < numPages_; ++i) {
    pageIDs << "image://pages/"+QString::number(i);
  }
  view_->rootContext()->setContextProperty("dataModel", QVariant::fromValue(pageIDs));
}

bool PageProvider::setDocument(const QString &filePath)
{
  qDebug() << "PageProvider::setDocument";

  bool out = false;

  if (NULL != doc_) {
    if(true == doc_->load(filePath)) {
      numPages_ = doc_->numPages();
      currentPage_ = -1;
      filePath_ = filePath;
      setDataModel();
      out = true;
    }
    else if (false == filePath_.isEmpty()) {
      //an error occured -> restore previous document
      out = (EXIT_SUCCESS == doc_->load(filePath_));
    }
  }
  return out;
}

