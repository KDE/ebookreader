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

#ifndef PAGEPROVIDER_H
#define PAGEPROVIDER_H

#include <QDebug>
#include <QEventLoop>
#include <QDeclarativeImageProvider>
#include "okulardocument.h"

class PageProvider : public QObject, public QDeclarativeImageProvider
{
  Q_OBJECT

signals:
  void pageRequest(int page, qreal factor);

public slots:
  void onPixmapReady(int page, const QPixmap *pix);

public:
  PageProvider();
  ~PageProvider();
  QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
  bool setDocument(const QString &filePath);
  void setScale(qreal scale, int index) {
    scaleFactor_ = scale;
    scaleIndex_ = index;
  }
  qreal scaleFactor() const {
    return scaleFactor_;
  }
  int scaleIndex() const {
    return scaleIndex_;
  }
  int currentPage() const {
    return currentPage_;
  }
  int numPages() const {
    return (NULL != doc_) ? doc_->numPages() : 0;
  }
  const QString& filePath() const {
    return filePath_;
  }
  bool isLoaded() const {
    return (doc_ != NULL);
  }
  bool cacheReady() const {
    if (requestingPixmap_) {
      return false;
    }
    for (int n = 0; n < cacheSize_; ++n) {
      if (PAGE_CACHE_VALID != pageCache_[n]->status) {
        return false;
      }
    }
    return true;
  }
  void setWinWidth(int width) {
    qDebug() << "PageProvider::setWinWidth";
    if (NULL != doc_) {
      qDebug() << "win width" << width;
      doc_->setWinWidth(width);
    } else {
      qDebug() << "cannot set win width";
    }
  }
  const QStringList& supportedFilePatterns() const {
    return doc_->supportedFilePatterns();
  }
  void gotoPage(int page);
private:
  bool invalidatePageCache(int page) {
    qDebug() << "PageProvider::invalidatePageCache" << page;

    if((0 > page) || (numPages() <= page)) {
      qDebug() << "nothing to do";
      return false;//operation failed
    }
    doc_->deletePixmap(pageCache_[page % CACHE_SIZE]->pPixmap);
    pageCache_[page % CACHE_SIZE]->pPixmap = NULL;
    pageCache_[page % CACHE_SIZE]->status = PAGE_CACHE_INVALID;
    return true;//operation successful
  }
  enum {CACHE_SIZE = 3};
  void sendPageRequest(int page) {
    if (true == invalidatePageCache(page)) {
      pageCache_[page % CACHE_SIZE]->status = PAGE_CACHE_PENDING;
      emit pageRequest(page, scaleFactor_);
    }
  }
  void updateCache();
  OkularDocument *doc_;
  int currentPage_;
  int prevPage_;
  qreal scaleFactor_;
  int scaleIndex_;
  enum PageCacheStatus {PAGE_CACHE_INVALID, PAGE_CACHE_VALID, PAGE_CACHE_PENDING};
  struct PageCache {
    const QPixmap *pPixmap;
    PageCacheStatus status;
  };
  QVector<PageCache*> pageCache_;
  QString filePath_;
  bool requestingPixmap_;//needed to block pixmap requests when inside pixmapRequest method
  QEventLoop evt_;//used for synchronous page requests
  int cacheSize_;//depends on the default cache size and the number of pages of the document
};

#endif
