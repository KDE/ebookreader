#ifndef SINGLE_APP_H
#define SINGLE_APP_H

#include <QSharedMemory>

class SingleApp
{
public:
  SingleApp() {
    shm_.setKey("19111977_tabletReader");
  }
  bool isRunning(void) {
    return !shm_.create(1);
  }
private:
  QSharedMemory shm_;
};

#endif

