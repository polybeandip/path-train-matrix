#include <led-matrix.h>

#include <unistd.h>
#include <signal.h>
#include <memory>
#include <functional>
#include <thread>

#include "drawutils.h"
#include "flipframecanvas.h"
#include "poller.h"

volatile bool interrupt = false;
static void InterruptHandler(int) {
  interrupt = true;
}

using namespace rgb_matrix;

int main() {
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  RGBMatrix::Options opts;
  RuntimeOptions runtime_opts;
  opts.hardware_mapping = "adafruit-hat";  
  opts.rows = 32;
  opts.cols = 64;
  opts.chain_length = 2;

  std::unique_ptr<RGBMatrix> matrix(RGBMatrix::CreateFromOptions(opts, runtime_opts));
  if (!matrix) return 1;

  std::unique_ptr<FlipFrameCanvas> flip = std::make_unique<FlipFrameCanvas>(matrix->CreateFrameCanvas(), false, true);
  
  int r = 7;
  Color red = Color(255, 0, 0);
  DrawCircleFill(flip.get(), r, r, r, red);                        // top
  DrawCircleFill(flip.get(), r, (flip->height() - 1) - r, r, red); // bot
  matrix->SwapOnVSync(flip->frame);

  Poller& poller = Poller::obtain();

  std::thread t(&Poller::poll, &poller);

  while (!interrupt) usleep(1 * 1000);

  poller.interrupt = true;
  t.join();

  return 0;
}
