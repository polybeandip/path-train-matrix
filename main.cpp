#include <led-matrix.h>
#include <unistd.h>
#include <signal.h>

#include "xflipcanvas.h"
#include "drawutils.h"

volatile bool interrupt_received = false;
static void InterruptHandler(int) {
  interrupt_received = true;
}

using namespace rgb_matrix;

int main(int argc, char *argv[]) {
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  RGBMatrix::Options defaults;
  defaults.hardware_mapping = "adafruit-hat";  
  defaults.rows = 32;
  defaults.cols = 64;
  defaults.chain_length = 1;
  defaults.parallel = 1;
  defaults.show_refresh_rate = true;

  Canvas* canvas = RGBMatrix::CreateFromFlags(&argc, &argv, &defaults);
  if (canvas == NULL) return 1;
  XFlipCanvas xflip(canvas);
  
  int r = 6;
  Color red = Color(255, 0, 0);
  drawCircleFill(&xflip, 2 + r, 2 + r, r, red);
  drawCircleFill(&xflip, 2 + r, (xflip.height() - 1) - (2 + r), r, red);

  while (!interrupt_received) usleep(1 * 1000);

  return 0;
}
