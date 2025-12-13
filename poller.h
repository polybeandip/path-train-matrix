#ifndef POLLER_H
#define POLLER_H

#include <array>
#include <mutex>
#include <string>
#include <memory>
#include <optional>

//#include "train.h"

class Poller {
  public:
    static Poller& obtain();

    Poller(const Poller&)    = delete;
    void operator=(const Poller&) = delete;

    void poll();

    volatile bool interrupt = false;

  private:
    Poller();
    ~Poller();
};

#endif // POLLER_H
