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

class QDeclarativeView;

class PageProvider : public QDeclarativeImageProvider
{
public:
  explicit PageProvider(QDeclarativeView *view);
  ~PageProvider();
  bool setDocument(const QString &filePath);

  void pixmapReady(int page, const QPixmap *pix);

  void setPage(int page = -1);
  QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

  void setScale(qreal scale) {
    scaleFactor_ = scale;
  }
  qreal scale() const {
    return scaleFactor_;
  }
  int currentPage() const {
    return currentPage_;
  }
  int currentIndex() const {
    return currentPage_ % CACHE_SIZE;
  }
  int count() const {
    return (NULL != doc_) ? doc_->numPages() : 0;
  }

  void sendPageRequest(int page) {
    if ((true == invalidatePageCache(page)) && (NULL != doc_)) {
      doc_->pageRequest(page, scaleFactor_);
    }
  }
  const QString& filePath() const {
    return filePath_;
  }
  void setWinWidth(int width) {
    if (NULL != doc_) {
      doc_->setWinWidth(width);
    }
  }
  bool isLoaded() const {
    return (NULL != doc_);
  }

  enum {CACHE_SIZE = 3};

private:
  void setDataModel();
  void setNextPage();
  void setPrevPage();
  bool invalidatePageCache(int page) {
    qDebug() << "PageProvider::invalidatePageCache" << page;

    bool out = false;
    if((0 <= page) && (numPages_ > page)) {
      doc_->deletePixmap(pageCache_[page % CACHE_SIZE]->pPixmap);
      pageCache_[page % CACHE_SIZE]->pPixmap = NULL;
      pageCache_[page % CACHE_SIZE]->valid = false;
      out = true;
    }
    return out;
  }

  QDeclarativeView *view_;
  OkularDocument *doc_;
  int currentPage_;
  int currentIndex_;
  int numPages_;
  qreal scaleFactor_;
  struct PageCache {
    const QPixmap *pPixmap;
    bool valid;
  };
  QList<PageCache*> pageCache_;
  QScrollArea *currentScrollArea_;
  QString filePath_;
  QEventLoop evtLoop_;
  int evtPage_;
};

#endif
