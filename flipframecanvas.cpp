#include "flipframecanvas.h"

FlipFrameCanvas::FlipFrameCanvas(rgb_matrix::FrameCanvas* frame, 
                                 bool flipx, bool flipy)
  : frame {frame}
  , flipx {flipx}
  , flipy {flipy}
{}

FlipFrameCanvas::~FlipFrameCanvas() {
  frame->Clear();
}

int FlipFrameCanvas::width() const {
  return frame->width();
}

int FlipFrameCanvas::height() const {
  return frame->height();
}

void FlipFrameCanvas::SetPixel(int x, int y,
                               uint8_t red,
                               uint8_t green,
                               uint8_t blue)
{
  if (flipx)
    x = frame->width() - x - 1;
  if (flipy)
    y = frame->height() - y - 1;

  frame->SetPixel(x, y, red, green, blue);
}

void FlipFrameCanvas::Fill(uint8_t red,
                           uint8_t green,
                           uint8_t blue)
{
  frame->Fill(red, green, blue);
}

void FlipFrameCanvas::Clear() {
  frame->Clear();
}
