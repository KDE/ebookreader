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

#include <QDebug>
#include <core/page.h>
#include <QPixmap>
#include <QPainter>
#include <core/generator.h>
#include "okulardocument.h"
#include "screen_size.h"

//main entry point into okular core libray
class PagePainter
{
public:
  explicit PagePainter(Okular::Document *doc):
    doc_(doc)
  {}
  const QPixmap* getPagePixmap(const Okular::Page *page, int width, int height) const {
    if(false == page->hasPixmap(OkularDocument::OKULAR_OBSERVER_ID)) {
      qDebug() << "making pixmap request";
      Okular::PixmapRequest *pr = new Okular::PixmapRequest(OkularDocument::OKULAR_OBSERVER_ID, page->number(), width, height, 0, true);
      QLinkedList<Okular::PixmapRequest*> req;
      req.push_back(pr);
      doc_->requestPixmaps(req);
    }
    //get pixmap in notifyPageChanged
    return NULL;
  }
  const QPixmap* getPagePixmap(int page) {
    const QPixmap *pix = NULL;
    if (NULL != doc_) {
      const Okular::Page *p = doc_->page(page);
      if (NULL != p) {
        pix = p->_o_nearestPixmap(OkularDocument::OKULAR_OBSERVER_ID, -1, -1);
      }
    }
    return pix;
  }
private:
  Okular::Document *doc_;
};

OkularDocument::OkularDocument() :
  doc_(new Okular::Document(NULL)),
  painter_(new PagePainter(doc_))
{
  if(NULL != doc_) {
    doc_->addObserver(this);
  }
}

OkularDocument::~OkularDocument()
{
  delete doc_;
  delete painter_;
}

int OkularDocument::load(const QString &fileName)
{
  int res = EXIT_FAILURE;
  mimeType_ = KMimeType::findByPath(fileName);
  if((NULL != doc_) && (true == doc_->openDocument(fileName, KUrl::fromPath(fileName), mimeType_))) {
    res = EXIT_SUCCESS;
  }
  return res;
}

void OkularDocument::adjustSize(int &width, int &height)
{
  if(mimeType_->is("image/vnd.djvu")) {
    //ajust page size for djvu documents if needed
    if(width > MIN_SCREEN_WIDTH) {
      int old_width = width;
      width = int(MIN_SCREEN_WIDTH * 0.9);
      height = int(height * double(width) / old_width);
    }
  }
}

QPixmap* OkularDocument::setWhiteBackground(const QPixmap *pixmap)
{
  QPixmap *out = NULL;
  if(mimeType_->is("application/epub+zip")) {
    out = new QPixmap(pixmap->width(), pixmap->height());
    if(NULL != out) {
      out->fill(Qt::white);
      QPainter p;
      p.begin(out);
      p.setBackgroundMode(Qt::OpaqueMode);
      p.drawPixmap(0, 0, *pixmap);
      p.end();
    }
  }
  else {
    //already has white background
    out = new QPixmap(*pixmap);
  }
  return out;
}

//get Pixmap from okular core library and immediately create a copy of it
//TODO: remove unneeded copy
const QPixmap* OkularDocument::getPixmap(int pageNb, qreal scaleFactor)
{
  if((NULL == doc_) || (NULL == painter_) || (0 >= scaleFactor)) {
    return NULL;
  }
  qDebug() << "OkularDocument::getPixmap: pageNb" << pageNb << ", scaleFactor" << scaleFactor;
  QPixmap *out = NULL;
  Okular::Page *page = const_cast<Okular::Page*>(doc_->page(pageNb));
  if(NULL != page) {

    qDebug() << "width" << page->width() << ", height" << page->height();
    int width = int(scaleFactor * page->width());
    int height = int(scaleFactor * page->height());
    adjustSize(width, height);
    const QPixmap *pixmap = painter_->getPagePixmap(page, width, height);
    if(NULL != pixmap) {
      out = setWhiteBackground(pixmap);
      //delete immediatelly internal pixmap
      page->deletePixmap(OKULAR_OBSERVER_ID);
    }
  }
  return out;
}

void OkularDocument::notifyPageChanged(int page, int flags)
{
  if(flags & DocumentObserver::Pixmap) {
    qDebug() << "DocumentObserver::Pixmap" << page;
    const QPixmap *pix = painter_->getPagePixmap(page);
  }
  else if(flags & DocumentObserver::Bookmark) {
    qDebug() << "DocumentObserver::Bookmark" << page;
  }
  else if(flags & DocumentObserver::Highlights) {
    qDebug() << "DocumentObserver::Highlights" << page;
  }
  else if(flags & DocumentObserver::TextSelection) {
    qDebug() << "DocumentObserver::TextSelection" << page;
  }
  else if(flags & DocumentObserver::Annotations) {
    qDebug() << "DocumentObserver::Annotations" << page;
  }
  else if(flags & DocumentObserver::BoundingBox) {
    qDebug() << "DocumentObserver::BoundingBox" << page;
  }
  else if(flags & DocumentObserver::NeedSaveAs) {
    qDebug() << "DocumentObserver::NeedSaveAs" << page;
  }
  else {
    qDebug() << "Unknown notification" << flags << " for page" << page;
  }
}
