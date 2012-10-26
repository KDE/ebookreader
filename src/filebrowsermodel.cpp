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

#include <QMainWindow>
#include <QDir>
#include <QDebug>
#include <kmimetype.h>
#include "filebrowsermodel.h"
#include "documentwidget.h"

FileBrowserModel::FileBrowserModel(QObject *parent, const QStringList &list) :
  QAbstractListModel(parent),
  parent_(parent),
  supportedFilePatterns_(list)
{
  QHash<int, QByteArray> roles;
  roles[TITLE] = "title";
  roles[IMAGE] = "image";
  roles[IS_FILE] = "file";
  roles[PATH] = "path";
  setRoleNames(roles);

  currentDir_ = QDir::homePath();
}

void FileBrowserModel::changeCurrentDir(int index)
{
  if(index >= dirs_.count()) {
    return;
  }
  if(index == 0 && dirs_[index].startsWith(tr("Go"))) {
    currentDir_ += "/..";
    QDir dir = QDir(currentDir_);
    currentDir_ = dir.absolutePath();
  }
  else {
    currentDir_ += "/" +  dirs_[index];
  }
  qDebug() << currentDir_;
  searchSupportedFiles();
  reset();
}

void FileBrowserModel::searchSupportedFiles()
{
  files_.clear();
  dirs_.clear();

  QDir directory = QDir(currentDir_, "*.*",
                        QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);
  directory.setNameFilters(supportedFilePatterns_);

  //fill file list
  directory.setFilter(QDir::Files);
  foreach(QString file, directory.entryList()) {
    files_.append(directory.absoluteFilePath(file));
  }
  files_.append(closeFileBrowserText());

  //fill folder list
  directory.setFilter(QDir::AllDirs);
  foreach(QString file, directory.entryList()) {
    QString dirToAdd;
    QString absPath = directory.absoluteFilePath(file);
    if(file == "." || !QDir(absPath).isReadable())
      continue;
    if(file == "..") {
      QDir dir = QDir(absPath);
      if(!dir.isRoot()) {
        dirToAdd =
          tr("Go Back To '") + QDir(dir.canonicalPath()).dirName() + "'";
      }
      else {
        dirToAdd = tr("Go Back To /");
      }
    }
    else {
      dirToAdd = QDir(absPath).dirName();
    }
    dirs_.append(dirToAdd);
  }
}

int FileBrowserModel::rowCount(const QModelIndex&) const
{
  return (dirs_.count() + files_.count());
}

QVariant FileBrowserModel::data(const QModelIndex &index, int role) const
{

  int dirRow =  index.row();

  bool isFile = false;
  if(dirRow >=  dirs_.count()) {
    isFile = true;
  }

  if(isFile) {
    int fileRow = index.row() - dirs_.count();
    switch(role) {
    case TITLE:
      return QDir(files_[fileRow]).dirName();
    case IMAGE:
      if(((fileRow + 1) == files_.count()) &&
          (closeFileBrowserText() == files_[fileRow])) {
        return QString(":/filebrowser/icons/Apps-session-quit-icon.png");
      }
      else {
        QString iconFileName;
        KMimeType::Ptr ptr = KMimeType::findByPath(files_[fileRow]);
        if(ptr->is("application/pdf")) {
          iconFileName = QString(":/filebrowser/icons/Adobe-PDF-Document-icon.png");
        }
        else if(ptr->is("image/vnd.djvu")) {
          iconFileName = QString(":/filebrowser/icons/Djvu-document-icon.png");
        }
        else if(ptr->is("application/vnd.ms-htmlhelp")) {
          iconFileName = QString(":/filebrowser/icons/Chm-document-icon.png");
        }
        else if(ptr->is("application/epub+zip")) {
          iconFileName = QString(":/filebrowser/icons/Epub-document-icon.png");
        }
        else {
          iconFileName = QString(":/filebrowser/icons/Document-icon.png");
        }
        return iconFileName;
      }
    case IS_FILE:
      return 1;
    case PATH:
      return files_[fileRow];
    }
  }
  else {
    switch(role) {
    case TITLE:
      return dirs_[dirRow];
    case IMAGE:
      if(dirRow == 0 && dirs_[dirRow].startsWith(tr("Go"))) {
        return QString(":/filebrowser/icons/Button-Upload-icon.png");
      }
      else {
        return QString(":/filebrowser/icons/My-Ebooks-icon.png");
      }
    case IS_FILE:
      return 0;
    case PATH:
      return dirs_[dirRow];
    }
  }

  return QVariant();
}

void FileBrowserModel::setCurrentDir(const QString &filePath)
{
  currentDir_ = QFileInfo(filePath).dir().absolutePath();
  qDebug() << "setting current dir to" << currentDir_;
}
