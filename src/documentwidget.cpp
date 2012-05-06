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

#include <QtGui>
#include <kmimetype.h>
#include "documentwidget.h"
#include "SlidingStackedWidget.h"
#include "okulardocument.h"
#include "chmdocument.h"
#include "window.h"

DocumentWidget::DocumentWidget(Window *parent)
    : QObject(parent),
      parent_(parent),
      doc_(NULL),
      currentPage_(-1),
      currentIndex_(-1),
      maxNumPages_(0),
      scaleFactor_(1.0),
      stackedWidget_(NULL),
      currentScrollArea_(NULL)
{
    for (int n = 0; n < CACHE_SIZE; ++n)
    {
        pageCache_.append(new PageCache);
        pageCache_[n]->valid = false;
    }
}

DocumentWidget::~DocumentWidget()
{
    for (int n = 0; n < pageCache_.size(); ++n)
    {
        delete pageCache_[n];
    }
    delete doc_;
}

void DocumentWidget::loadImage(int page)
{
    qDebug() << "DocumentWidget::loadImage begin";
    if ((0 > page) || (maxNumPages_ <= page))
    {
        qDebug() << "DocumentWidget::loadImage: nothing to do";
        return;
    }

    doc_->deletePixmap(pageCache_[page%CACHE_SIZE]->pPixmap);
    pageCache_[page%CACHE_SIZE]->pPixmap = doc_->
                      getPixmap(page, scaleFactor_);
    pageCache_[page%CACHE_SIZE]->valid = true;
    qDebug() << "DocumentWidget::loadImage end" << page%CACHE_SIZE;
}

void DocumentWidget::showPage(int page)
{
    qDebug() << "DocumentWidget::showPage" << page;
    if (page != -1) {//if we show the same page nothing should be done, but this verification is done by the routines above
        currentIndex_ = stackedWidget_->currentIndex();
        if (NULL != currentScrollArea_) { //do nothing if no page has been loaded
            if (currentPage_ <= (page - 1)) {
                ++currentIndex_;
                if (CACHE_SIZE == currentIndex_) {
                    currentIndex_ = 0;
                }
            } else {
                --currentIndex_;
                if (currentIndex_ < 0) {
                    currentIndex_ = CACHE_SIZE-1;
                }
            }
        }
        currentPage_ = page - 1;
    }

    //set image on the scroll area
    currentScrollArea_ = (QScrollArea*)stackedWidget_->widget(currentIndex_);//get next/prev widget
    QLabel *label = (QLabel*)currentScrollArea_->widget();
    cacheMutex_.lock();
    if (false == pageCache_[currentPage_%CACHE_SIZE]->valid) {
        qDebug() << "DocumentWidget::showPage: invalid cache";
        loadImage(currentPage_);//load image into memory
        pageCache_[currentPage_%CACHE_SIZE]->valid = true;
    } else {
        qDebug() << "DocumentWidget::showPage: valid cache" << currentPage_%CACHE_SIZE;
    }
    qDebug() << "DocumentWidget::showPage: begin setPixmap";
    const QPixmap *pPix = pageCache_[currentPage_%CACHE_SIZE]->pPixmap;
    if ((NULL != pPix) && (false == pPix->isNull()))
    {
	qDebug() << "setPixmap" << pPix;
    	label->setPixmap(*pPix);
    	label->adjustSize();
    } else {
	    qDebug() << "pixmap is NULL or empty" << pPix;
    }
    qDebug() << "DocumentWidget::showPage: end setPixmap";
    cacheMutex_.unlock();
}

//factory method
bool DocumentWidget::setDocument(const QString &filePath)
{
    Document *oldDoc = doc_;//keep old document

    KMimeType::Ptr ptr = KMimeType::findByPath(filePath);
    if (ptr->is("application/vnd.ms-htmlhelp"))
    {
	    doc_ = new CHMDocument();
	    parent_->setSingleThreaded(true);
    } else
    {
	    doc_ = new OkularDocument();
	    parent_->setSingleThreaded(true);
    }

    if ((NULL != doc_) && (EXIT_SUCCESS == doc_->load(filePath)))
    {
        maxNumPages_ = doc_->numPages();
        currentPage_ = -1;
        delete oldDoc;//previous Document must be deleted
        return true;
    } else
    {
        //an error occured
        delete doc_;
        //restore old document
        doc_ = oldDoc;
    }
    return false;
}

void DocumentWidget::setPage(int page)
{
    if (page != currentPage_ + 1)
    {
        showPage(page);
    }
}
