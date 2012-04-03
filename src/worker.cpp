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

#include <QProgressDialog>
#include <QDebug>
#include "worker.h"
#include "documentwidget.h"
#include "window.h"
#include <QDeclarativeView>
#include <QDeclarativeEngine>

Worker::Worker(DocumentWidget *doc, Window *win) :
    doc_(doc), win_(win)
{      
}

void Worker::onUpdateCache(int page)
{
    qDebug() << "Worker::onUpdateCache begin page" << page;
    doc_->cacheMutex_.lock();
    if (false == doc_->pageCache_[page%DocumentWidget::CACHE_SIZE]->valid) {
        doc_->loadImage(page);
    } else {
        qDebug() << "Worker::onUpdateCache: nothing to do";
    }
    doc_->cacheMutex_.unlock();
    qDebug() << "Worker::onUpdateCache end";
}

void Worker::onCheckAppUpAuthCode()
{
    qDebug() << "Window::checkAppUpAuthCode";
#ifndef NO_APPUP_AUTH_CODE
    if (NULL != win_)
    {
        //Authorization code for Intel AppUp(TM) software
        try {
#ifdef _DEBUG
            win_->appupApp_ = new Application(ApplicationId(ADP_DEBUG_APPLICATIONID));
#else
            win_->appupApp_ = new Application(ApplicationId(0xF95A11A9,0xF079468E,0xABAF2D5C,0x7C56C5F7));
#endif
        } catch (AdpException&) {
            //Display an appropriate error message here
            win_->closeWaitDialog();
            emit appUpAuthCheckError();
            if (win_->appupApp_ != NULL)
            {
                delete win_->appupApp_;
                win_->appupApp_ = NULL;
            }
            return;
        }
        win_->closeWaitDialog();
    }
#endif
}

void Worker::onBatteryStatusChanged(QSystemBatteryInfo::BatteryStatus status)
{
    if ((QSystemBatteryInfo::BatteryEmpty == status) ||
            (QSystemBatteryInfo::BatteryCritical == status))
    {
        win_->saveSettings();
        qDebug() << "battery empty or critical, settings saved";
    }
}
