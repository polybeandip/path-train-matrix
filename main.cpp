#include <led-matrix.h>

#include <signal.h>
#include <memory>
#include <functional>
#include <thread>
#include <chrono>
#include <optional>
#include <map>
#include <sstream>
#include <boost/log/trivial.hpp>

#include "train.h"
#include "utils.h"
#include "pathpoller.h"

#define LAYER_VERT      15
#define TOP_BOT_OFF     (LAYER_VERT + 2)
#define X_TRAIN_POS     15
#define Y_TRAIN_POS_TOP 14

enum Level {TOP, BOT};
enum Station {WTC, ThirtyThird, ThirtyThirdViaHob, IDK};

const std::map<std::string, Station> string_to_station = {
  {"World Trade Center", Station::WTC},
  {"33rd Street", Station::ThirtyThird},
  {"33rd Street via Hoboken", Station::ThirtyThirdViaHob}
};

static std::string fontpath = "roboto-tweaked.bdf";

static int large_radius = 7;
static int small_radius = 5;

using namespace rgb_matrix;

static const Color red    = Color(255, 0, 0);
static const Color green  = Color(0, 255, 0);
static const Color blue   = Color(0, 0, 255);
static const Color yellow = Color(255, 255, 0);
static const Color white  = Color(255, 255, 255);

volatile bool interrupt = false;
static void InterruptHandler(int) {
  interrupt = true;
}

void drawcircle(Canvas* canvas, const std::string& head_sign, int y_offset) {
  Station station = Station::IDK;
  if ( auto it = string_to_station.find(head_sign);
       it != string_to_station.end() ) {
    station = it->second;
  }

  switch (station) {
    case Station::WTC:
      DrawCircleFill(canvas,
                     large_radius, large_radius + y_offset,
                     large_radius, red);
      break;
    case Station::ThirtyThird:
      DrawCircleFill(canvas,
                     large_radius, large_radius + y_offset,
                     large_radius, yellow);
      break;
    case Station::ThirtyThirdViaHob:
      DrawCircleFill(canvas,
                     small_radius, small_radius + y_offset,
                     small_radius, yellow);
      DrawCircleFill(canvas,
                     small_radius + 4, small_radius + y_offset + 4,
                     small_radius, blue);
      break;
    default:
      DrawCircleFill(canvas,
                     large_radius, large_radius + y_offset,
                     large_radius, white);
  }
}

void drawtext(Canvas* canvas, const Font& font,
              const std::string& head_sign, const std::string& arrival_msg,
              int y_offset) {
  DrawText(canvas,
           font,
           X_TRAIN_POS,
           Y_TRAIN_POS_TOP + y_offset,
           white,
           NULL,
           head_sign.c_str());

  int arrival_off = StringWidth(font, arrival_msg);
  DrawText(canvas,
           font,
           canvas->width() - arrival_off + 1, // right align
           Y_TRAIN_POS_TOP + y_offset,
           arrival_msg == "Late " ? red : green,
           NULL,
           arrival_msg.c_str());
}

void substitute(Train& train) {
  if (train.head_sign == "33rd Street via Hoboken")
    train.head_sign = "33rd Street via HOB";
  if (train.arrival_msg == "0 min")
    train.arrival_msg = "Due ";
  if (train.arrival_msg == "Delayed")
    train.arrival_msg = "Late ";
}

void draw(Canvas* canvas, const Font& font, Train train, Level level)
{
  int y_offset = level == Level::TOP ? 0 : TOP_BOT_OFF;

  drawcircle(canvas, train.head_sign, y_offset);
  substitute(train);
  drawtext(canvas, font, train.head_sign, train.arrival_msg, y_offset);
}

int main() {
  using namespace std::chrono_literals;

  BOOST_LOG_TRIVIAL(info) << "Choo Choo!";

  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  RGBMatrix::Options opts;
  RuntimeOptions runtime_opts;
  opts.hardware_mapping = "adafruit-hat";  
  opts.rows = 32;
  opts.cols = 64;
  opts.chain_length = 3;

  std::unique_ptr<RGBMatrix> matrix(RGBMatrix::CreateFromOptions(opts, runtime_opts));
  if (!matrix) {
    BOOST_LOG_TRIVIAL(fatal) << "main(): could not init RGBMatrix";
    return 1;
  }
  
  FrameCanvas* offscreen = matrix->CreateFrameCanvas();

  Font font;
  if (!font.LoadFont(fontpath.c_str())) {
    BOOST_LOG_TRIVIAL(fatal) << "main(): could not load font "
                             << fontpath;
    return 1;
  }
  
  PathPoller& poller = PathPoller::obtain();

  std::thread t(&PathPoller::poll, &poller);
  
  std::optional<Train> display[2];
  while (!interrupt) {
    // get two most recent departures
    std::vector<Train> trains = poller.getTrains();
    bool is_new = false;
    for (unsigned int i = 0; i < 2; i++) {
      if (trains.size() > i) {
        is_new = is_new || !(display[i].has_value())
                        || *(display[i]) != trains[i];
        display[i] = trains[i];
      }
      else {
        is_new = is_new || display[i].has_value();
        display[i] = {};
      }
    }

    // draw
    if (is_new) {
      std::ostringstream dss;
      dss << "[";
      if (display[0].has_value())
        dss << *(display[0]);
      else
        dss << "None";
      dss << ", ";
      if (display[1].has_value())
        dss << *(display[1]);
      else
        dss << "None";
      dss << "]";
      BOOST_LOG_TRIVIAL(info) << "main(): display=" << dss.str();

      offscreen->Clear();
      if (display[0].has_value())
        draw(offscreen, font, *(display[0]), Level::TOP);
      if (display[1].has_value())
        draw(offscreen, font, *(display[1]), Level::BOT);
      matrix->SwapOnVSync(offscreen);
    }

    std::this_thread::sleep_for(500ms);
  }

  poller.interrupt = true;
  t.join();

  return 0;
}
