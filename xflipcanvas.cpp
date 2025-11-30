#include "xflipcanvas.h"

XFlipCanvas::XFlipCanvas(rgb_matrix::Canvas* canvas) :
  canvas {canvas}
{}

XFlipCanvas::~XFlipCanvas() {
  canvas->Clear();
  delete canvas;
}

int XFlipCanvas::width() const {
  return canvas->width();
}

int XFlipCanvas::height() const {
  return canvas->height();
}

void XFlipCanvas::SetPixel(int x, int y,
                           uint8_t red,
                           uint8_t green,
                           uint8_t blue) {
  canvas->SetPixel(canvas->width() - x - 1, y, 
                   red, 
                   green, 
                   blue);
}

void XFlipCanvas::Fill(uint8_t red,
                       uint8_t green,
                       uint8_t blue) {
  canvas->Fill(red, green, blue);
}

void XFlipCanvas::Clear() {
  canvas->Clear();
}
