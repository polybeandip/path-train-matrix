#ifndef XFLIPCANVAS_H
#define XFLIPCANVAS_H

#include <canvas.h>
#include <graphics.h>

class XFlipCanvas : public rgb_matrix::Canvas {
  public:
    XFlipCanvas(rgb_matrix::Canvas* canvas);
    ~XFlipCanvas();

    int width() const override;
    int height() const override;

    void SetPixel(int x, int y,
                  uint8_t red,
                  uint8_t green,
                  uint8_t blue) override;

    void Fill(uint8_t red,
              uint8_t green,
              uint8_t blue) override;

    void Clear() override;

  private:
    rgb_matrix::Canvas* canvas;
};

#endif // XFLIPCANVAS_H
