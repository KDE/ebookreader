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

#ifndef OKULAR_DOCUMENT_H
#define OKULAR_DOCUMENT_H

#include <kmimetype.h>
#include <core/document.h>
#include <core/observer.h>

namespace Okular
{
class Document;
class Page;
class DocumentObserver;
}
class PagePainter;

class OkularDocument : public Okular::DocumentObserver
{
public:
  OkularDocument();
  virtual int load(const QString &fileName);
  virtual const QPixmap* getPixmap(int page, qreal scaleFactor);
  uint numPages() const {
    return (NULL != doc_)?doc_->pages():0;
  }
  void deletePixmap(const QPixmap *p) {
    delete p;//TODO: remove this methis
  }
  virtual ~OkularDocument();
  enum {OKULAR_OBSERVER_ID = 6};
  virtual uint observerId() const {
    return OKULAR_OBSERVER_ID;
  }
  void notifyPageChanged(int page, int flags);
private:
  void adjustSize(int &width, int &height);
  QPixmap* setWhiteBackground(const QPixmap *pixmap);
  Okular::Document *doc_;
  PagePainter *painter_;
  KMimeType::Ptr mimeType_;
};

#endif // OKULAR_DOCUMENT_H
