#ifndef FLIPFRAMECANVAS_H
#define FLIPFRAMECANVAS_H

#include <led-matrix.h>

class FlipFrameCanvas : public rgb_matrix::Canvas {
  public:
    FlipFrameCanvas(rgb_matrix::FrameCanvas* frame, bool flipx, bool flipy);
    ~FlipFrameCanvas();

    int width()  const override;
    int height() const override;

    void SetPixel(int x, int y,
                  uint8_t red,
                  uint8_t green,
                  uint8_t blue) override;

    void Fill(uint8_t red,
              uint8_t green,
              uint8_t blue) override;

    void Clear() override;

    rgb_matrix::FrameCanvas* frame; // Does not own

  private:
    const bool               flipx;
    const bool               flipy;
};

#endif // FLIPFRAMECANVAS_H
