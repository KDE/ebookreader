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
#include <QPixmap>
#include <QPainter>
#include <QtCore/qmath.h>
#include <core/generator.h>
#include <core/settings_core.h>
#include <core/page.h>
#include "okulardocument.h"
#include "pageprovider.h"
#include "screen_size.h"

//main entry point into okular core libray
class PagePainter
{
public:
  explicit PagePainter(Okular::Document *doc):
    doc_(doc)
  {}
  ~PagePainter() {
  }
  //send request for page pixmap
  void sendRequest(const Okular::Page *page, int width, int height)
  {
    qDebug() << "PagePainter::sendRequest";
    if(false == page->hasPixmap(OkularDocument::OKULAR_OBSERVER_ID)) {
      qDebug() << "making pixmap request";
    Okular::PixmapRequest *pixmapRequest = new Okular::PixmapRequest(OkularDocument::OKULAR_OBSERVER_ID, page->number(), width, height, 0, true);//asynchronous request (the request is deleted by okular core library)
      req_.clear();
      req_.push_back(pixmapRequest);
      doc_->requestPixmaps(req_);
    }
  }
  //get page pixmap when notifyPageChanged() is called
  const QPixmap* getPagePixmap(int page) const 
  {
    qDebug() << "PagePainter::getPagePixmap";
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
  QLinkedList<Okular::PixmapRequest*> req_;
};

OkularDocument::OkularDocument(PageProvider *provider) :
  provider_(provider),
  doc_(NULL),
  painter_(NULL)
{
  Okular::SettingsCore::instance("");//need to call this before creating the documnent
  doc_ = new Okular::Document(NULL);
  painter_ = new PagePainter(doc_);
  if(NULL != doc_) {
    doc_->addObserver(this);
  }
}

OkularDocument::~OkularDocument()
{
  delete doc_;
  delete painter_;
}

bool OkularDocument::load(const QString &fileName)
{
  bool res = false;
  mimeType_ = KMimeType::findByPath(fileName);
  if (NULL != doc_) {
    doc_->closeDocument();//close previous document if any
    res = doc_->openDocument(fileName, KUrl::fromPath(fileName), mimeType_);
  }
  return res;
}

void OkularDocument::adjustSize(int &width, int &height)
{
  if((long)width*(long)height > 20000000L) {
    qDebug() << "adjust width and height in order to stay below threshold";
    //adjust height in order to stay below this threshold (used by okular core library)
    qreal factor2 = qreal(20000000L)/qreal((long)width*(long)height);
    height = int(height*factor2);
  }
}

const QPixmap* OkularDocument::setWhiteBackground(const QPixmap *pixmap)
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

//send Pixmap request to okular core library
void OkularDocument::pageRequest(int page, qreal factor)
{
  qDebug() << "OkularDocument::pageRequest: pageNb" << page << ", scaleFactor" << factor;

  if((NULL == doc_) || (NULL == painter_)) {
    return;
  }

  const Okular::Page *p = doc_->page(page);
  if(NULL != p) {
    if (-1 == factor) {
      factor = winWidth_/p->width();//adjust scale factor to occupy the entire window width
    }
    int width = int(factor*(p->width()));
    int height = int(factor*(p->height()));
    adjustSize(width, height);
    painter_->sendRequest(p, width, height);
  }
}

void OkularDocument::notifyPageChanged(int page, int flags)
{
  if(flags & DocumentObserver::Pixmap) {
    qDebug() << "DocumentObserver::Pixmap" << page;
    const QPixmap *pix = painter_->getPagePixmap(page);
    if (NULL != pix) {
      pix = setWhiteBackground(pix);//TODO: remove unneeded duplication ?
      //delete immediatelly internal pixmap
      Okular::Page *p = const_cast<Okular::Page*>(doc_->page(page));
      if (NULL != p) {
        p->deletePixmap(OKULAR_OBSERVER_ID);
      }
      provider_->pixmapReady(page, pix);
    }
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
