/****************************************************************************
**
** Copyright (C) 2011, Bogdan Cristea. All rights reserved.
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
#include <QtSystemInfo/QSystemBatteryInfo>

class QProgressDialog;
class DocumentWidget;
class Window;

QTM_USE_NAMESPACE

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(DocumentWidget *doc = NULL, Window *win = NULL);

signals:
    void appUpAuthCheckError();

public slots:
    void onUpdateCache(int page);
    void onCheckAppUpAuthCode();
    void onBatteryStatusChanged(QSystemBatteryInfo::BatteryStatus status);

private:
    DocumentWidget *doc_;
    Window *win_;
};

#endif // WORKER_H
