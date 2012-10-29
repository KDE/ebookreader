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
#include "documentwidget.h"
#include "SlidingStackedWidget.h"
#include "okulardocument.h"
#include "window.h"

DocumentWidget::DocumentWidget(Window *parent)
  : QObject(parent),
    doc_(new OkularDocument()),
    currentPage_(-1),
    currentIndex_(-1),
    maxNumPages_(0),
    scaleFactor_(1.0),
    stackedWidget_(NULL),
    currentScrollArea_(NULL),
    evtPage_(-1)
{
  for(int n = 0; n < CACHE_SIZE; ++n) {
    pageCache_.append(new PageCache);
    pageCache_[n]->pPixmap = NULL;
    pageCache_[n]->status = PAGE_CACHE_INVALID;
  }
  connect(this, SIGNAL(pageRequest(int, qreal)), doc_, SLOT(onPageRequest(int, qreal)));
  connect(doc_, SIGNAL(pixmapReady(int, const QPixmap*)), this, SLOT(onPixmapReady(int, const QPixmap*)));
}

DocumentWidget::~DocumentWidget()
{
  for(int n = 0; n < pageCache_.size(); ++n) {
    delete pageCache_[n];
  }
  delete doc_;
}

void DocumentWidget::onPixmapReady(int page, const QPixmap *pix)
{
  qDebug() << "DocumentWidget::onPixmapReady";

  pageCache_[page % CACHE_SIZE]->pPixmap = pix;
  pageCache_[page % CACHE_SIZE]->status = PAGE_CACHE_VALID;
  if (page == evtPage_) {
    if (true == evtLoop_.isRunning()) {//racing conditions should be managed by the upper layers
      evtLoop_.quit();//used for synchronous page requests
    }
    evtPage_ = -1;
  }
}

void DocumentWidget::setPage(int page)
{
  qDebug() << "DocumentWidget::setPage" << page;

  if(page != -1) {
    currentIndex_ = stackedWidget_->currentIndex();
    if(NULL != currentScrollArea_) {  //do nothing if no page has been loaded
      if(currentPage_ <= (page - 1)) {
        ++currentIndex_;
        if(CACHE_SIZE == currentIndex_) {
          currentIndex_ = 0;
        }
      }
      else {
        --currentIndex_;
        if(currentIndex_ < 0) {
          currentIndex_ = CACHE_SIZE - 1;
        }
      }
    }
    currentPage_ = page - 1;
  }

  //set image on the scroll area
  currentScrollArea_ = (QScrollArea*)stackedWidget_->widget(currentIndex_);//get next/prev widget
  QLabel *label = (QLabel*)currentScrollArea_->widget();
  const QPixmap *pPix = NULL;
  switch (PageCacheStatus st = pageCache_[currentPage_%CACHE_SIZE]->status) {
  case PAGE_CACHE_INVALID:
  case PAGE_CACHE_PENDING:
    evtPage_ = currentPage_;//prepare to wait synchronously this page
    if (PAGE_CACHE_INVALID == st) {
      qDebug() << "invalid cache";
      emit pageRequest(currentPage_, scaleFactor_);
    }
    else {
      qDebug() << "pending cache";
    }
    if (-1 != evtPage_) {
      evtLoop_.exec();//wait to receive the page pixmap
    }
    //falls through
  case PAGE_CACHE_VALID:
    qDebug() << "valid cache";
    pPix = pageCache_[currentPage_ % CACHE_SIZE]->pPixmap;
    if((NULL != pPix) && (false == pPix->isNull())) {
      qDebug() << "setPixmap" << pPix;
      label->setPixmap(*pPix);
      label->adjustSize();
    }
    break;
  default:
    qDebug() << "unknown page state";
  }
}

bool DocumentWidget::setDocument(const QString &filePath)
{
  qDebug() << "DocumentWidget::setDocument";

  bool out = false;

  if (NULL != doc_) {
    if(true == doc_->load(filePath)) {
      maxNumPages_ = doc_->numPages();
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
