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

#ifndef OKULAR_DOCUMENT_H
#define OKULAR_DOCUMENT_H

#include <QPixmap>
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

class OkularDocument : public QObject, public Okular::DocumentObserver
{
  Q_OBJECT

signals:
  void pixmapReady(int page, const QPixmap *pix);

public slots:
  void onPageRequest(int page, qreal factor);

public:
  OkularDocument();
  bool load(const QString &fileName);
  uint numPages() const {
    return (NULL != doc_)?doc_->pages():0;
  }
  void deletePixmap(const QPixmap *p) {
    delete p;//TODO: remove this method
  }
  void setWinWidth(int width) {
    winWidth_ = width;
  }
  ~OkularDocument();
  void notifyPageChanged(int page, int flags);
  const QStringList& supportedFilePatterns();
private:
  void preProcessPage(int &width, int &height, const Okular::Page *page);
  const QPixmap* postProcessPage(const QPixmap *pixmap);
  Okular::Document *doc_;
  PagePainter *painter_;
  int winWidth_;
  qreal zoomFactor_;
  KMimeType::Ptr mimeType_;
  QStringList supportedFilePatterns_;
};

#endif // OKULAR_DOCUMENT_H
