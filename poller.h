#ifndef POLLER_H
#define POLLER_H

#include "train.h"

#include <vector>
#include <mutex>
#include <string>

class Poller {
  public:
    static Poller& obtain();

    Poller(const Poller&)    = delete;
    void operator=(const Poller&) = delete;

    void poll();
    std::vector<Train> getTrains();

    volatile bool interrupt = false;

  private:
    Poller();
    ~Poller();

    std::vector<Train> trains;
    std::mutex mtx;
};

#endif // POLLER_H
