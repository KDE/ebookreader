#ifndef QML_CPP_MEDIATOR_H_
#define QML_CPP_MEDIATOR_H_

#include <QObject>
#include <QDebug>
#include "pageprovider.h"

class QmlCppMediator: public QObject {
    Q_OBJECT

    Q_PROPERTY (bool hlpBck READ isHlpBck WRITE setHlpBck NOTIFY hlpBckChanged);
    Q_PROPERTY (bool fullScr READ isFullScr WRITE setFullScr NOTIFY fullScrChanged);

public:
    Q_INVOKABLE bool cacheReady() const {
      bool out = (NULL != provider_)?provider_->cacheReady():false;
      qDebug() << "QmlCppMediator::cacheReady" << out;
      return out;
    }
    QmlCppMediator(PageProvider* provider) :
        QObject(NULL),
        hlpBck(true), fullScr(true), provider_(provider)
        {}

    void setHlpBck(bool value) {
        hlpBck = value;
        emit hlpBckChanged();
    }
    bool isHlpBck() {
        return hlpBck;
    }
    void setFullScr(bool value) {
        fullScr = value;
        emit fullScrChanged();
    }
    bool isFullScr() {
        return fullScr;
    }

signals:
    void hlpBckChanged();
    void fullScrChanged();

private:
  bool hlpBck;
  bool fullScr;
  PageProvider *provider_;
};

#endif

