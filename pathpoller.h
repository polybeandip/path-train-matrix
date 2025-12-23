#ifndef PATHPOLLER_H
#define PATHPOLLER_H

#include "train.h"

#include <vector>
#include <mutex>
#include <string>

class PathPoller {
  public:
    static PathPoller& obtain();

    PathPoller(const PathPoller&)    = delete;
    void operator=(const PathPoller&) = delete;

    void poll();
    std::vector<Train> getTrains();

    volatile bool interrupt = false;

  private:
    PathPoller();
    ~PathPoller();

    void loop();

    std::vector<Train> trains;
    std::mutex mtx;
};

#endif // PATHPOLLER_H
