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

#include "document.h"

namaspace Okular
{
	class Document;
	class Page;
}
class OkularObserver;
class PagePainter;

class OkularDocument : public Document
{
public:
    OkularDocument();
    virtual int load(const QString &fileName);
    virtual const QPixmap* getPixmap(int page, qreal xres, qreal yres);
    virtual void deletePixmap(const QPixmap *pixmap);
    virtual ~OkularDocument();
private:
    Okular::Document *doc_;
    OkularObserver *obs_;
    PagePainter *painter_;
    QMap<const QPixmap*, const Okular::Page*> pages_;
};

#endif // OKULAR_DOCUMENT_H
