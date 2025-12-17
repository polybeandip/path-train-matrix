#include <led-matrix.h>

#include <signal.h>
#include <memory>
#include <functional>
#include <thread>
#include <chrono>
#include <optional>
#include <map>

// temp
#include <iostream> 

#include "utils.h"
#include "poller.h"

#define X_TRAIN_POS 15
#define Y_TOP_POS   14
#define Y_BOT_POS   31

using namespace rgb_matrix;

      int   radius = 7;
const Color red    = Color(255, 0, 0);
const Color white  = Color(255, 255, 255);
const Color green  = Color(0, 255, 0);
const Color yellow = Color(255, 255, 0);

const std::map<std::string, Color> linecolor = {
  {"World Trade Center", red}, 
  {"33rd Street", yellow},
  {"33rd Street via Hoboken", yellow}
};

volatile bool interrupt = false;
static void InterruptHandler(int) {
  interrupt = true;
}

void draw(Canvas* canvas, const Font& font, /*const*/ Train& train, bool istop) {
  /*const*/ std::string& head_sign = train.head_sign;
  const std::string& arrival_msg = train.arrival_msg;
  int                arrival_off = StringWidth(font, arrival_msg);
  const Color&       color       = linecolor.at(head_sign);

  // dumb hack
  if (head_sign == "33rd Street via Hoboken") head_sign = "33rd Street";

  if (istop) {
    DrawCircleFill(canvas, radius, radius, radius, color);
    DrawText(canvas, 
             font, 
             X_TRAIN_POS, 
             Y_TOP_POS, 
             white, 
             NULL, 
             head_sign.c_str());
    DrawText(canvas, 
             font, 
             canvas->width() - arrival_off + 1, // right align
             Y_TOP_POS, 
             green, 
             NULL, 
             arrival_msg.c_str());
  }
  else {
    DrawCircleFill(canvas, radius, (canvas->height() - 1) - radius, radius, color);
    DrawText(canvas, 
             font, 
             X_TRAIN_POS, 
             Y_BOT_POS, 
             white, 
             NULL, 
             head_sign.c_str());
    DrawText(canvas, 
             font, 
             canvas->width() - arrival_off + 1, 
             Y_BOT_POS, 
             green, 
             NULL, 
             arrival_msg.c_str());
  }
}

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

  Font font;
  if (!font.LoadFont("roboto-tweaked.bdf")) {
    std::cout << "ERROR LoadFont(): could't load font" << std::endl;
    return 1;
  }
  
  Poller& poller = Poller::obtain();

  std::thread t(&Poller::poll, &poller);
  
  std::optional<Train> display[2];
  while (!interrupt) {
    // get two most recent departures
    std::vector<Train> trains = poller.getTrains();
    bool is_new = false;
    for (unsigned int i = 0; i < 2; i++) {
      is_new = is_new || i >= trains.size()
                      || !(display[i].has_value())
                      || *(display[i]) != trains[i];
      if (trains.size() > i)
        display[i] = trains[i];
      else
        display[i] = {};
    }

    // draw
    if (is_new) {
      offscreen->Clear();
      if (display[0].has_value()) draw(offscreen, font, *(display[0]), true);
      if (display[1].has_value()) draw(offscreen, font, *(display[1]), false);
      matrix->SwapOnVSync(offscreen);
    }

    std::this_thread::sleep_for(500ms);
  }

  poller.interrupt = true;
  t.join();

  return 0;
}
