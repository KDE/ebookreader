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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QString>

class QPixmap;

/* Interface for handling electronic documents
 */
class Document
{
public:
    Document() : numPages_(0) {}
    virtual int load(const QString &fileName) = 0;
    virtual const QPixmap* render(int page, qreal xres, qreal yres) = 0;
    virtual void deletePixmap(const QPixmap *pixmap) = 0;
    int numPages() const
    {
        return numPages_;
    }
    virtual ~Document()
    {
        numPages_ = 0;
    }
protected:
    int numPages_;
};

#endif // DOCUMENT_H
