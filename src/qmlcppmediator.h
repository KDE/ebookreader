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

#ifndef QML_CPP_MEDIATOR_H_
#define QML_CPP_MEDIATOR_H_

#include <QObject>
#include <QDebug>
#include "pageprovider.h"
#include "filebrowsermodel.h"

class QmlCppMediator: public QObject {
  Q_OBJECT

  Q_PROPERTY (bool hlpBck READ isHlpBck WRITE setHlpBck NOTIFY hlpBckChanged);
  Q_PROPERTY (bool fullScr READ isFullScr WRITE setFullScr NOTIFY fullScrChanged);

  Q_PROPERTY (int winWidth READ isWinWidth WRITE setWinWidth NOTIFY winWidthChanged);
  Q_PROPERTY (int winHeight READ isWinHeight WRITE setWinHeight NOTIFY winHeightChanged);

public:
  Q_INVOKABLE bool cacheReady() const {
    bool out = (NULL != provider_)?provider_->cacheReady():false;
    return out;
  }
  Q_INVOKABLE void setFavorites(bool f) {
    if (NULL != fbrowser_) {
      fbrowser_->setFavorites(f);
    }
  }

  QmlCppMediator(const PageProvider *provider, FileBrowserModel *fbrowser) :
  QObject(NULL), hlpBck(true), fullScr(true), provider_(provider), fbrowser_(fbrowser),
  winWidth(1024), winHeight(768)
  {}
  void setHlpBck(bool value) {
    hlpBck = value;
    emit hlpBckChanged();
  }
  bool isHlpBck() {
    return hlpBck;
  }
  void setFullScr(bool value) {
    fullScr = value;
    emit fullScrChanged();
  }
  bool isFullScr() {
    return fullScr;
  }

  void setWinWidth(int w) {
    winWidth = w;
    emit winWidthChanged();
  }
  int isWinWidth() {
    return winWidth;
  }
  void setWinHeight(int h) {
    winHeight = h;
    emit winHeightChanged();
  }
  int isWinHeight() {
    return winHeight;
  }

signals:
  void hlpBckChanged();
  void fullScrChanged();
  void winWidthChanged();
  void winHeightChanged();

private:
  bool hlpBck;
  bool fullScr;
  const PageProvider *provider_;
  FileBrowserModel *fbrowser_;
  int winWidth;
  int winHeight;
};

#endif

