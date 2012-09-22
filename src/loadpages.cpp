#include "loadpages.h"
#include "documentwidget.h"
#include <QDebug>

LoadPages::LoadPages(DocumentWidget* doc) :
  doc_(doc)
{
  start(QThread::IdlePriority);
}

void LoadPages::run()
{
  exec();
}

void LoadPages::loadPages(int currentPage)
{
  static int prevPage = -1;
  if(prevPage < currentPage) {
    //next page
    doc_->loadImage(currentPage + 1, doc_->currentIndex() + 1);
    qDebug() << "LoadPages::loadPages: next " << currentPage + 1;
  }
  else if(prevPage > currentPage) {
    //load previous page
    doc_->loadImage(currentPage - 1, doc_->currentIndex() - 1);
    qDebug() << "LoadPages::loadPages: previous " << currentPage - 1;
  }
  else {
    qDebug() << "LoadPages::loadPages: same " << currentPage;
  }
  prevPage = currentPage;
}

void LoadPages::preloadPage(unsigned int page)
{
  qDebug() << "LoadPages::preloadPage: " << page;
  doc_->loadImage(page, 0);//first page to be displayed
}
