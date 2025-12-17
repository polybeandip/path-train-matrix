#include "utils.h"

void DrawCircleFill(rgb_matrix::Canvas* canvas,
                    int cx, int cy, int r, 
                    rgb_matrix::Color color) {
  /* Midpoint Circle Algorithm
   * https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
   *
   * This is a slight variation that fills as well
   */
  for (int x = 0, y = r; x < y; x++) {
    double y_mid = y - 0.5;
    if (x*x + y_mid*y_mid > r*r) y--;
    
    int min_x = -x, max_x = x;
    int min_y = -y, max_y = y;
    if (x < -x) {min_x = x; max_x = -x;}
    if (y < -y) {min_y = y; max_y = -y;}

    for (int i = min_x; i <= max_x; i++) {
      canvas->SetPixel(cx + i, cy + y, color.r, color.g, color.b);
      canvas->SetPixel(cx + i, cy - y, color.r, color.g, color.b);
    }

    for (int i = min_y; i <= max_y; i++) {
      canvas->SetPixel(cx + i, cy + x, color.r, color.g, color.b);
      canvas->SetPixel(cx + i, cy - x, color.r, color.g, color.b);
    }
  }
}

int StringWidth(const rgb_matrix::Font& font, const std::string& str) {
  int total = 0;
  for (const char& c: str) total += font.CharacterWidth(c);
  return total;
}
