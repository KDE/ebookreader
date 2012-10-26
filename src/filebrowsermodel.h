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

#ifndef FileBrowserModel_H
#define FileBrowserModel_H

#include <QAbstractListModel>
#include <QVector>

class FileBrowserModel : public QAbstractListModel
{
  Q_OBJECT
public:
  FileBrowserModel(QObject *parent, const QStringList &list);
  QVariant data(const QModelIndex &index, int role) const;
  int rowCount(const QModelIndex &parent) const;
  void addDirToSearch(QString& dir);
  void removeDirToSearch(QString& dir);
  void searchSupportedFiles();
  void setCurrentDir(const QString &filePath);
  static QString closeFileBrowserText() {
    return tr("Close File Browser");
  }

public slots:
  void changeCurrentDir(int index);

private:
  Q_DISABLE_COPY(FileBrowserModel);
  enum PdfPreviewRoles { TITLE, PAGES, IMAGE, IS_FILE, PATH };
  QString currentDir_;
  QVector<QString> files_;
  QVector<QString> dirs_;
  QObject *parent_;
  const QStringList &supportedFilePatterns_;
};

#endif // FileBrowserModel_H
