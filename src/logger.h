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

#ifndef LOGGER_H
#define LOGGER_H

#include <QTextStream>

class QFile;
class QMutex;

class Logger
{
public:
    static Logger* instance(const QString &fileLogName);
private:
    Logger(const QString &fileLogName);
    ~Logger();
    Logger(const Logger&);//copy constructor
    const Logger& operator=(const Logger&);//copy assignment operator
    static void debugMessageHandler(QtMsgType type, const char *msg);
    static QTextStream ts_;
    static QFile *pOutFile_;
    static QMutex *loggerMutex_;
    static Logger *instance_;
};

#endif // LOGGER_H
