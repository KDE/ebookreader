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
** References: qindle project (http://code.google.com/p/qindle/)
**             kchmviewer (http://sourceforge.net/projects/kchmviewer/)
**
****************************************************************************/

#include <QByteArray>
#include <QStandardItemModel>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QTextCodec>
#include <QThread>
#include <QApplication>
#include <QDebug>
#include "chmdocument.h"
#include "chmreply.h"
#include "screen_size.h"
#include "libchmfileimpl.h" 

CHMDocument::CHMDocument() :
    Document(),
    doc_(new LCHMFile()),
    req_(new RequestHandler(this))
{
}

CHMDocument::~CHMDocument()
{
    delete doc_;
    delete req_;
}

QString CHMDocument::path() const
{
    return (NULL != doc_)?doc_->impl()->m_filename:QString("");
}

int CHMDocument::load(const QString &fileName)
{
    if ((NULL != doc_) && (true == doc_->loadFile(fileName)))
    {
        //get the table of contents
        if (true == doc_->parseTableOfContents(&toc_))
        {
            numPages_ = toc_.size();
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

const QPixmap* CHMDocument::getPixmap(int page, qreal scaleFactor)
{
    if ((NULL == doc_) || (NULL == req_) || (0 == numPages_) || (0 >= scaleFactor))
    {
        return NULL;
    }

    QWebView webView;//we need to recreate the view at each page
    QObject::connect(&webView, SIGNAL(loadFinished(bool)), &eventLoop_, SLOT(quit()));
    webView.page()->setNetworkAccessManager(req_);
    QStringList urls = toc_.at(page).urls;//there must be only one URL for the TOC
    webView.load(QUrl::fromLocalFile(urls.at(0)));
    eventLoop_.exec();//wait for load to complete
    webView.setZoomFactor(scaleFactor);
    QWebFrame *webFrame = webView.page()->mainFrame();
    webFrame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    webFrame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    QSize size = webFrame->contentsSize();
    int preferredWidth = int(MIN_SCREEN_WIDTH*0.9);
    if (size.width() < preferredWidth)
    {
        size = QSize(preferredWidth, size.height());//adjust page width
    }
    webView.setGeometry(QRect(QPoint(0, 0), size));
    return new QPixmap(QPixmap::grabWidget(&webView));
}

//this method is used by QWebView to load an HTML page
QNetworkReply* CHMDocument::RequestHandler::createRequest(Operation op, const QNetworkRequest &req,
                             QIODevice *outgoingData)
{
    if (req.url().scheme()=="file")
    {
        return new CHMReply(this, req, req.url(), chmDoc_->doc_);
    } else
    {
        return QNetworkAccessManager::createRequest(op, req, outgoingData);
    }
}
