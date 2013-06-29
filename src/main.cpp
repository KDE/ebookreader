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

#include <QApplication>
#include <QTranslator>
#include "window.h"
#include "logger.h"

#if defined(WIN32) || defined(WIN64)
#define _UNICODE
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
void stopKde4WinDaemons();
#endif

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

#ifdef QT_DEBUG_ENABLE_LOG
  //in release mode the log file is not created
  Logger::instance("tabletReader.log");
  //TODO: should reinstall log and provide an option to use either the file or the default logger
#endif
  //translation object
  QTranslator translator;
  if(false == translator.load(":/translations/tabletReader_" + QLocale::system().name().left(2))) {
    qDebug() << "cannot load translation file" << QLocale::system().name().left(2);
  }
  else {
    qDebug() << "loaded translation file" << QLocale::system().name().left(2);
  }
  app.installTranslator(&translator);
  //main window
  Window wnd;
  wnd.show();
  int out = app.exec();
#if defined(WIN32) || defined(WIN64)
  stopKde4WinDaemons();
#endif
  return out;
}

#if defined(WIN32) || defined(WIN64)
void stopKde4WinDaemons()
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry))
    {
        while (Process32Next(snapshot, &entry))
        {
            if ((0 == _tcsicmp(entry.szExeFile, _T("dbus-daemon.exe"))) ||
              (0 == _tcsicmp(entry.szExeFile, _T("klauncher.exe"))))
            {  
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
                if (NULL != hProcess)
                {
                  TerminateProcess(hProcess, 0);
                  CloseHandle(hProcess);
                }
            }
        }
    }

    CloseHandle(snapshot);
}
#endif
