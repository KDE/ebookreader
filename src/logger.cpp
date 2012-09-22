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

#include <QDir>
#include <QDateTime>
#include <QThread>
#include <stdlib.h>
#include <QDebug>
#include <QMutex>
#include <iostream>
#include "logger.h"

QTextStream Logger::ts_;
QFile *Logger::pOutFile_ = NULL;
QMutex *Logger::loggerMutex_ = NULL;
Logger *Logger::instance_ = NULL;

Logger* Logger::instance(const QString &fileLogName)
{
  if(NULL == instance_) {
    instance_ = new Logger(fileLogName);
  }
  return instance_;
}

Logger::Logger(const QString &fileLogName)
{
  pOutFile_ = new QFile(QDir::homePath() + QDir::separator() +
                        QDir::toNativeSeparators(fileLogName));
  if(false == pOutFile_->open(QIODevice::WriteOnly | QIODevice::Append)) {
    std::cerr << "Cannot open log file " << std::endl;
  }
  ts_.setDevice(pOutFile_);
  loggerMutex_ = new QMutex;
  qInstallMsgHandler(debugMessageHandler);
  qDebug() << "\n\nSTART" << QDateTime::currentDateTime().toString(Qt::ISODate)
           << "START\n";
}

Logger::~Logger()
{
  delete pOutFile_;
  pOutFile_ = NULL;
  delete loggerMutex_;
  loggerMutex_ = NULL;
  delete instance_;
  instance_ = NULL;
}

void Logger::debugMessageHandler(QtMsgType type, const char *msg)
{
  loggerMutex_->lock();
  QString txt;
  switch(type) {
  case QtDebugMsg:
    txt = QString("Debug: %1").arg(msg);
    break;
  case QtWarningMsg:
    txt = QString("Warning: %1").arg(msg);
    break;
  case QtCriticalMsg:
    txt = QString("Critical: %1").arg(msg);
    break;
  case QtFatalMsg:
    txt = QString("Fatal: %1").arg(msg);
    abort();
  }
  ts_ << QDateTime::currentDateTime().toMSecsSinceEpoch()
      << "\tTID:" << QThread::currentThreadId()
      << " " << txt << endl;
  loggerMutex_->unlock();
}
