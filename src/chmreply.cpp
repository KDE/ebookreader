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
** Reference: qindle project (http://code.google.com/p/qindle/)
**            kchmviewer (http://sourceforge.net/projects/kchmviewer/)
**
****************************************************************************/

#include <QDebug>
#include "chmreply.h"
#include "libchmfile.h"
#include "libchmurlfactory.h"

CHMReply::CHMReply(QObject *parent, const QNetworkRequest &req, const QUrl &url, LCHMFile *doc) :
  QNetworkReply(parent),
  doc_(doc)
{
  setRequest(req);
  setOpenMode(QIODevice::ReadOnly);

  data_ = loadResource(url);
  length_ = data_.length();

  setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data_.length()));
  QMetaObject::invokeMethod(this, "metaDataChanged", Qt::QueuedConnection);

  if(length_) {
    QMetaObject::invokeMethod(this, "readyRead", Qt::QueuedConnection);
  }

  QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
}

qint64 CHMReply::bytesAvailable() const
{
  return data_.length() + QNetworkReply::bytesAvailable();
}

void CHMReply::abort()
{
  QNetworkReply::close();
}

qint64 CHMReply::readData(char* data, qint64 maxlen)
{
  qint64 len = qMin(qint64(data_.length()), maxlen);

  if(len) {
    qMemCopy(data, data_.constData(), len);
    data_.remove(0, len);
  }
  return len;
}

QByteArray CHMReply::loadResource(const QUrl &url)
{
  QString data, path = url.toString(QUrl::StripTrailingSlash);

  // Retreive the data from chm file
  if(NULL == doc_) {
    return QByteArray();
  }
  // Does the file have a file name, or just a path with prefix?
  if(!path.contains("::")) {
    // Just the prefix, so strip it
    path.remove(0, 7);
  }
  int pos = path.indexOf('#');
  if(pos != -1) {
    path = path.left(pos);
  }

  // To handle a single-image pages, we need to generate the HTML page to show
  // this image. We did it in KCHMViewWindow::handleStartPageAsImage; now we need
  // to generate the HTML page, and set it.
  if(LCHMUrlFactory::handleFileType(path, data)) {
    return qPrintable(data);
  }

  QByteArray buf;

  if(path.endsWith(".html", Qt::CaseInsensitive)
      || path.endsWith(".htm", Qt::CaseInsensitive)) {
    // encoding autodetection is disabled
    if(!doc_->getFileContentAsBinary(&buf, path)) {
      qWarning("Could not resolve file %s\n", qPrintable(path));
    }

    setHeader(QNetworkRequest::ContentTypeHeader, "text/html");
  }
  else {
    QString fpath = decodeUrl(path);

    if(!doc_->getFileContentAsBinary(&buf, fpath))
      qWarning("Could not resolve file %s\n", qPrintable(path));

    setHeader(QNetworkRequest::ContentTypeHeader, "binary/octet");
  }

  return buf;
}

// Shamelessly stolen from Qt
QString CHMReply::decodeUrl(const QString &input)
{
  QString temp;

  int i = 0;
  int len = input.length();
  int a, b;
  QChar c;
  while(i < len) {
    c = input[i];
    if(c == '%' && i + 2 < len) {
      a = input[++i].unicode();
      b = input[++i].unicode();

      if(a >= '0' && a <= '9')
        a -= '0';
      else if(a >= 'a' && a <= 'f')
        a = a - 'a' + 10;
      else if(a >= 'A' && a <= 'F')
        a = a - 'A' + 10;

      if(b >= '0' && b <= '9')
        b -= '0';
      else if(b >= 'a' && b <= 'f')
        b  = b - 'a' + 10;
      else if(b >= 'A' && b <= 'F')
        b  = b - 'A' + 10;

      temp.append((QChar)((a << 4) | b));
    }
    else {
      temp.append(c);
    }

    ++i;
  }

  return temp;
}
