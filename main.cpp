#include <led-matrix.h>
#include <unistd.h>
#include <signal.h>
#include <memory>

#include "drawutils.h"

volatile bool interrupt_received = false;
static void InterruptHandler(int) {
  interrupt_received = true;
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

  FrameCanvas* offscreen = matrix->CreateFrameCanvas();
  
  int r = 7;
  Color red   = Color(255, 0, 0);
  DrawCircleFill(offscreen, r, r, r, red);                             // top
  DrawCircleFill(offscreen, r, (offscreen->height() - 1) - r, r, red); // bot
  matrix->SwapOnVSync(offscreen);

  while (!interrupt_received) usleep(1 * 1000);
  
  return 0;
}
