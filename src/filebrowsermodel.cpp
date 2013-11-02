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

#include <QMainWindow>
#include <QDir>
#include <QDebug>
#include <kmimetype.h>
#include <QSettings>
#include "filebrowsermodel.h"
#include "pageprovider.h"
#include "settings_keys.h"

FileBrowserModel::FileBrowserModel(QObject *parent, const PageProvider *doc, const QStringList &list) :
  QAbstractListModel(parent),
  parent_(parent),
  supportedFilePatterns_(list),
  favorites_(false),
  doc_(doc)
{
  QHash<int, QByteArray> roles;
  roles[TITLE] = "title";
  roles[PAGE] = "page";
  roles[IMAGE] = "image";
  roles[IS_FILE] = "file";
  roles[PATH] = "path";
  setRoleNames(roles);

  currentDir_ = QDir::homePath();
  searchSupportedFiles();
}

void FileBrowserModel::changeCurrentDir(int index)
{
  if (favorites_) {
    //save the current file into favorites list
    saveFavorites();
    //and refresh the view
    loadFavorites();
    //close file browser
    emit quit();
  }
  else {
    if(index >= dirs_.count()) {
      return;
    }
    if(index == 0) {
      currentDir_ += "/..";
      QDir dir = QDir(currentDir_);
      currentDir_ = dir.absolutePath();
    }
    else {
      currentDir_ += "/" +  dirs_[index];
    }
    searchSupportedFiles();
    reset();
  }
}

void FileBrowserModel::searchSupportedFiles()
{
  files_.clear();
  dirs_.clear();

  QDir directory = QDir(currentDir_, "*.*", QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);

  //fill folder list
  directory.setFilter(QDir::AllDirs);
  foreach(QString file, directory.entryList()) {
    QString dirToAdd;
    QString absPath = directory.absoluteFilePath(file);
    if(file == "." || !QDir(absPath).isReadable())
      continue;
    if(file == "..") {
      QDir dir = QDir(absPath);
      QString currentDirName = QDir(currentDir_).dirName();
      if(!dir.isRoot()) {
        dirToAdd = tr("Go Back to '%1' from '%2'").arg(QDir(dir.canonicalPath()).dirName()).
          arg(currentDirName);
      }
      else {
        dirToAdd = tr("Go Back to '/' from '%1'").arg(currentDirName);
      }
    }
    else {
      dirToAdd = QDir(absPath).dirName();
    }
    dirs_.append(dirToAdd);
  }

  //fill file list
  directory.setFilter(QDir::Files);
  directory.setNameFilters(supportedFilePatterns_);
  foreach(QString file, directory.entryList()) {
    files_.append(directory.absoluteFilePath(file));
  }
  files_.append(closeFileBrowserText());
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
    {
      QString title = QDir(files_[fileRow]).dirName();
      if (favorites_ && (fileRow < (files_.size()-1))) {
        title += tr(" at page %1").arg(filesPage_[fileRow]+1);
      }
      return title;
    }
    case PAGE:
      return favorites_?filesPage_[fileRow]:0;
    case IMAGE:
      if(((fileRow + 1) == files_.count()) &&
          (closeFileBrowserText() == files_[fileRow])) {
        return QString(":/filebrowser/icons/Apps-session-quit-icon.svg");
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
        else if(ptr->is("application/vnd.ms-htmlhelp") ||
          ptr->is("application/x-chm")) {
          iconFileName = QString(":/filebrowser/icons/Chm-document-icon.png");
        }
        else if(ptr->is("application/epub+zip")) {
          iconFileName = QString(":/filebrowser/icons/Epub-document-icon.png");
        }
        else if(ptr->is("application/x-fictionbook+xml")) {
          iconFileName = QString(":/filebrowser/icons/fb2-icon.png");
        }
        else if(ptr->is("application/x-cbr")) {
          iconFileName = QString(":/filebrowser/icons/cbr-icon.png");
        }
        else if(ptr->is("image/bmp")) {
          iconFileName = QString(":/filebrowser/icons/bmp-icon.png");
        }
        else if(ptr->is("application/x-dvi")) {
          iconFileName = QString(":/filebrowser/icons/dvi-icon.png");
        }
        else if(ptr->is("image/x-eps")) {
          iconFileName = QString(":/filebrowser/icons/eps-icon.png");
        }
        else if(ptr->is("image/jpeg")) {
          iconFileName = QString(":/filebrowser/icons/jpg-icon.png");
        }
        else if(ptr->is("image/png")) {
          iconFileName = QString(":/filebrowser/icons/png-icon.png");
        }
        else if(ptr->is("application/vnd.oasis.opendocument.text")) {
          iconFileName = QString(":/filebrowser/icons/odt-icon.png");
        }
        else if(ptr->is("image/gif")) {
          iconFileName = QString(":/filebrowser/icons/gif-icon.png");
        }
        else if(ptr->is("image/vnd.microsoft.icon")) {
          iconFileName = QString(":/filebrowser/icons/ico-icon.png");
        }
        else if(ptr->is("image/tiff")) {
          iconFileName = QString(":/filebrowser/icons/tif-icon.png");
        }
        else if(ptr->is("application/postscript")) {
          iconFileName = QString(":/filebrowser/icons/ps-icon.png");
        }
        else if(ptr->is("image/vnd.adobe.photoshop")) {
          iconFileName = QString(":/filebrowser/icons/psd-icon.png");
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
      if(dirRow == 0) {
        return favorites_?QString(":/filebrowser/icons/Actions-list-add-icon.svg"):QString(":/filebrowser/icons/Button-Upload-icon.svg");
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
  searchSupportedFiles();
}

void FileBrowserModel::setFavorites(bool f)
{
  favorites_ = f;
  if (favorites_) {
    loadFavorites();
  }
  else {
    //reload previous folder and file list
    searchSupportedFiles();
  }
}

void FileBrowserModel::loadFavorites()
{
  //load the list of favorite docs
  files_.clear();
  filesPage_.clear();
  dirs_.clear();
  dirs_.append(tr("Bookmark \"%1\" at page %2").arg(QDir(doc_->filePath()).dirName()).arg(doc_->currentPage()+1));
  QSettings settings(ORGANIZATION, APPLICATION);
  QStringList fav = settings.value(KEY_FAVORITES_LIST).toStringList();
  foreach (QString path, fav) {
    QStringList list = path.split(":");
    if (2 != list.size()) {
      continue;//unlinkely
    }
    files_.append(list.at(0));
    filesPage_.append(list.at(1).toInt());
  }
  files_.append(closeFileBrowserText());
}

void FileBrowserModel::saveFavorites()
{
  if (NULL == doc_) {
    return;
  }
  QSettings settings(ORGANIZATION, APPLICATION);
  QStringList fav = settings.value(KEY_FAVORITES_LIST).toStringList();
  //check for the same file and remove old entries
  QString filePath = doc_->filePath();
  QStringList entries = fav.filter(filePath);
  foreach(QString entry, entries) {
    int i = fav.indexOf(entry);
    fav.removeAt(i);
  }
  //remove oldest entry if the list is full
  if (FAVORITES_LIST_SIZE < fav.size()) {
    fav.pop_back();
  }
  fav.push_front(filePath+":"+QString::number(doc_->currentPage()));
  settings.setValue(KEY_FAVORITES_LIST, fav);
}

