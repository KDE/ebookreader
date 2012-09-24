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

#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#ifndef NO_MOBILITY
#include <QtSystemInfo/QSystemBatteryInfo>
#endif

class DocumentWidget;
class Window;

#ifndef NO_MOBILITY
QTM_USE_NAMESPACE
#endif

class Worker : public QObject
{
  Q_OBJECT
public:
  Worker(DocumentWidget *doc = NULL, Window *win = NULL);

#ifndef NO_MOBILITY
public slots:
  void onBatteryStatusChanged(QSystemBatteryInfo::BatteryStatus status);
#endif

private:
  DocumentWidget *doc_;
  Window *win_;
};

#endif // WORKER_H
