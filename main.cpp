#include <led-matrix.h>

#include <signal.h>
#include <memory>
#include <functional>
#include <thread>
#include <chrono>
#include <optional>

// temp
#include <iostream> 

#include "utils.h"
#include "flipframecanvas.h"
#include "poller.h"

volatile bool interrupt = false;
static void InterruptHandler(int) {
  interrupt = true;
}

using namespace rgb_matrix;

int main() {
  using namespace std::chrono_literals;

  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  RGBMatrix::Options opts;
  RuntimeOptions runtime_opts;
  opts.hardware_mapping = "adafruit-hat";  
  opts.rows = 32;
  opts.cols = 64;
  opts.chain_length = 3;

  std::unique_ptr<RGBMatrix> matrix(RGBMatrix::CreateFromOptions(opts, runtime_opts));
  if (!matrix) return 1;
  
  FrameCanvas* offscreen = matrix->CreateFrameCanvas();

  Poller& poller = Poller::obtain();

  std::thread t(&Poller::poll, &poller);
  
  std::optional<Train> display[2];
  while (!interrupt) {
    // get two most recent departures
    std::vector<Train> trains = poller.getTrains();
    for (unsigned int i = 0; i < 2; i++) {
      if (trains.size() > i)
        display[i] = trains[i];
      else
        display[i] = {};
    }

    std::this_thread::sleep_for(500ms);
  }

  poller.interrupt = true;
  t.join();

  return 0;
}
