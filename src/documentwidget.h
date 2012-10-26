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
#include "okulardocument.h"

class SlidingStackedWidget;
class Window;

class DocumentWidget : public QObject
{
  Q_OBJECT

  friend class Worker;

signals:
  void pageRequest(int page, qreal factor);

public slots:
  void onPixmapReady(int page, const QPixmap *pix);

public:
  DocumentWidget(Window *parent = 0);
  ~DocumentWidget();
  bool setDocument(const QString &filePath);
  void setPage(int page = -1);
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
  int numPages() const {
    return (NULL != doc_) ? doc_->numPages() : 0;
  }
  bool isLoaded() const {
    return (doc_ != NULL);
  }
  void setStackedWidget(SlidingStackedWidget *stackedWidget) {
    stackedWidget_ = stackedWidget;
  }

  void showCurrentPageUpper() {
    if(NULL != currentScrollArea_) {
      currentScrollArea_->verticalScrollBar()->setValue(0);
    }
  }
  void showCurrentPageLower() {
    if(NULL != currentScrollArea_) {
      QScrollBar *scroll = currentScrollArea_->verticalScrollBar();
      scroll->setValue(scroll->maximum());
    }
  }
  bool invalidatePageCache(int page) {
    qDebug() << "DocumentWidget::invalidatePageCache" << page;

    if(0 > page || maxNumPages_ <= page) {
      qDebug() << "DocumentWidget::invalidatePageCache: nothing to do";
      return false;//operation failed
    }
    doc_->deletePixmap(pageCache_[page % CACHE_SIZE]->pPixmap);
    pageCache_[page % CACHE_SIZE]->pPixmap = NULL;
    pageCache_[page % CACHE_SIZE]->valid = false;
    return true;//operation successful
  }
  void sendPageRequest(int page) {
    if (true == invalidatePageCache(page)) {
      emit pageRequest(page, scaleFactor_);
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
  const QStringList& supportedFilePatterns() const {
    return doc_->supportedFilePatterns();
  }

  enum {CACHE_SIZE = 3};

private:
  OkularDocument *doc_;
  int currentPage_;
  int currentIndex_;
  int maxNumPages_;
  qreal scaleFactor_;
  struct PageCache {
    const QPixmap *pPixmap;
    bool valid;
  };
  QList<PageCache*> pageCache_;
  SlidingStackedWidget *stackedWidget_;
  QScrollArea *currentScrollArea_;
  QString filePath_;
  QEventLoop evtLoop_;
  int evtPage_;
};

#endif
