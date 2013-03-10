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

#ifndef DOCUMENTWIDGET_H
#define DOCUMENTWIDGET_H

#include <QLabel>
#include <QRectF>
#include <QScrollArea>
#include <QScrollBar>
#include <QMutex>
#include <QDebug>
#include <QEventLoop>
#include <QDeclarativeImageProvider>
#include "okulardocument.h"

class QDeclarativeContext;

class PageProvider : public QDeclarativeImageProvider
{
public:
  explicit PageProvider(QDeclarativeContext *context);
  ~PageProvider();

  QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};

class Window;

class DocumentWidget
{
public:
  explicit DocumentWidget(Window *parent = 0);
  ~DocumentWidget();
  bool setDocument(const QString &filePath);
  void setPage(int page = -1);
  void setScale(qreal scale) {
    scaleFactor_ = scale;
  }
  qreal scale() const {
    return scaleFactor_;
  }
  int count() const {
    return (NULL != doc_) ? doc_->numPages() : 0;
  }

    /*if(0 > page || maxNumPages_ <= page) {
      qDebug() << "DocumentWidget::invalidatePageCache: nothing to do";
      return false;//operation failed
    }
    doc_->deletePixmap(pageCache_[page % CACHE_SIZE]->pPixmap);
    pageCache_[page % CACHE_SIZE]->pPixmap = NULL;
    pageCache_[page % CACHE_SIZE]->status = PAGE_CACHE_INVALID;
    return true;//operation successful
  }*/
  void sendPageRequest(int page) {
    if (true == invalidatePageCache(page)) {
      pageCache_[page % CACHE_SIZE]->status = PAGE_CACHE_PENDING;
      //emit pageRequest(page, scaleFactor_);
    }
  }
  void pixmapReady(int page, const QPixmap *pix);

  const QString& filePath() const {
    return filePath_;
  }
  void setWinWidth(int width) {
    if (NULL != doc_) {
      doc_->setWinWidth(width);
    }
  }
  const QStringList& supportedFilePatterns() const {
    return doc_->supportedFilePatterns();
  }

  enum {CACHE_SIZE = 3};

private:
  void setPage(int page, bool force = false);
  void setDataModel();
  void setNextPage();
  void setPrevPage();
  bool invalidatePageCache(int page) {
    qDebug() << "PageProvider::invalidatePageCache" << page;

    bool out = false;
    if((0 <= page) && (numPages_ > page)) {
      doc_->deletePixmap(pageCache_[page % CACHE_SIZE]->pPixmap);
      pageCache_[page % CACHE_SIZE]->pPixmap = NULL;
      pageCache_[page % CACHE_SIZE]->status = PAGE_CACHE_INVALID;
      out = true;
    }
    else {
      qDebug() << "invalid page number";
    }
    return out;
  }

  QDeclarativeContext *context_;
  OkularDocument *doc_;
  int currentPage_;
  int numPages_;
  qreal scaleFactor_;
  enum PageCacheStatus {PAGE_CACHE_INVALID, PAGE_CACHE_VALID, PAGE_CACHE_PENDING};
  struct PageCache {
    const QPixmap *pPixmap;
    PageCacheStatus status;
  };
  QList<PageCache*> pageCache_;
  QScrollArea *currentScrollArea_;
  QString filePath_;
  QEventLoop evtLoop_;
  int evtPage_;
};

#endif
