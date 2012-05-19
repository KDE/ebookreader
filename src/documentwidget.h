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
#include "document.h"

class SlidingStackedWidget;
class Window;

class DocumentWidget : public QObject
{
    Q_OBJECT

    friend class Worker;

public:
    DocumentWidget(Window *parent = 0);
    ~DocumentWidget();
    qreal scale() const
    {
        return scaleFactor_;
    }
    int currentPage() const
    {
        return currentPage_;
    }
    int currentIndex() const
    {
        return currentPage_%CACHE_SIZE;
    }
    int numPages() const
    {
        return doc_->numPages();
    }
    bool isLoaded()
    {
        return (doc_ != NULL);
    }
    void setStackedWidget(SlidingStackedWidget *stackedWidget)
    {
        stackedWidget_ = stackedWidget;
    }
    QString path() const
    {
	    return doc_->path();
    }

    void showCurrentPageUpper()
    {
        if (NULL != currentScrollArea_)
        {
            currentScrollArea_->verticalScrollBar()->setValue(0);
        }
    }
    void showCurrentPageLower()
    {
        if (NULL != currentScrollArea_)
        {
            QScrollBar *scroll = currentScrollArea_->verticalScrollBar();
            scroll->setValue(scroll->maximum());
        }
    }
    bool invalidatePageCache(int page)
    {
        qDebug() << "DocumentWidget::invalidatePageCache" << page;
        if (0 > page || maxNumPages_ <= page) {
            qDebug() << "DocumentWidget::invalidatePageCache: nothing to do";
            return false;//operation failed
        }
	pageCache_[page%CACHE_SIZE]->pPixmap = NULL;
        pageCache_[page%CACHE_SIZE]->valid = false;
        return true;//operation successful
    }

    void loadImage(int page);

    enum {CACHE_SIZE = 3};

    void showPage(int page = -1);

public slots:
    bool setDocument(const QString &filePath);
    void setPage(int page = -1);
    void setScale(qreal scale)
    {
        scaleFactor_ = scale;
    }

private:
    Document *doc_;
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
};

#endif
