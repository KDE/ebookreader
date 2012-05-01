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
#include <okular/core/document.h>
#include <okular/core/page.h>
#include <okular/core/generator.h>
#include <kmimetype.h>
#include "okulardocument.h"

//helper class, not made public by okular core library
namespace Okular
{
	class DocumentObserver
	{
	public:
		DocumentObserver() {}
		virtual ~DocumentObserver() {}
		virtual uint observerId() const = 0;
	 	virtual void notifySetup(const QVector< Okular::Page * > &, int) {}
		virtual void notifyViewportChanged(bool) {}
		virtual void notifyPageChanged(int, int) {}
		virtual void notifyContentsCleared(int) {}
		virtual void notifyVisibleRectsChanged() {}
		virtual void notifyZoom(int) {}
		virtual bool canUnloadPixmap(int) const {return true;}
	};
}
#define OKULAR_OBSERVER_ID 6
class OkularObserver : public Okular::DocumentObserver
{
public:
	virtual uint observerId() const {return OKULAR_OBSERVER_ID;}
};

//main entry point into okular core libray
class PagePainter
{
public:
	explicit PagePainter(Okular::Document *doc):
		doc_(doc)
	{}
	const QPixmap* getPagePixmap(const Okular::Page *page) const
	{
		if(false == page->hasPixmap(OKULAR_OBSERVER_ID, -1, -1))
		{
			Okular::PixmapRequest *pr = new Okular::PixmapRequest(OKULAR_OBSERVER_ID, page->number(), page->width(), page->height(), 0, false);
			QLinkedList<Okular::PixmapRequest*> req;
			req.push_back(pr);
			doc_->requestPixmaps(req);
		}
		return page->_o_nearestPixmap(OKULAR_OBSERVER_ID, -1, -1);
	}
private:
	Okular::Document *doc_;
};

OkularDocument::OkularDocument() :
    Document(),
    doc_(new Okular::Document(NULL)),
    obs_(new OkularObserver()),
    painter_(new PagePainter(doc_))
{
	if (NULL != doc_)
	{
		doc_->addObserver(obs_);
	}
}

OkularDocument::~OkularDocument()
{
    delete doc_;
    delete obs_;
    delete painter_;
    
}

int OkularDocument::load(const QString &fileName)
{
	int res = EXIT_FAILURE;
	if ((NULL != doc_) && (true == doc_->openDocument(fileName, KUrl::fromPath(fileName), KMimeType::findByPath(fileName))))
	{
		numPages_ = doc_->pages();
		res = EXIT_SUCCESS;
	}
	return res;
}

const QPixmap* OkularDocument::getPixmap(int pageNb, qreal, qreal)
{
	if (NULL == doc_ || NULL == painter_)
	{
		return NULL;
	}
	const QPixmap *pixmap = NULL;
	const Okular::Page *page = doc_->page(pageNb);
	if (NULL != page)
	{
		pixmap = painter_->getPagePixmap(page);
		if (NULL != pixmap)
		{
			pages_.insert(pixmap, page);
		}
	}
	return pixmap;
}

void OkularDocument::deletePixmap(const QPixmap *pixmap)
{
	if (NULL != pixmap)
	{
		QMap<const QPixmap*,const Okular::Page*>::iterator it = pages_.find(pixmap);
		if (pages_.end() != it)
		{
			Okular::Page *page = const_cast<Okular::Page*>(it.value());
			if (NULL != page)
			{
				page->deletePixmap(OKULAR_OBSERVER_ID);
				pages_.remove(pixmap);
			}
		}
	}
}

