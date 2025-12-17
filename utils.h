#ifndef UTILS_H
#define UTILS_H

#include <canvas.h>
#include <graphics.h>

#include <string>

void DrawCircleFill(rgb_matrix::Canvas* canvas,
                    int cx, int cy, int r, 
                    rgb_matrix::Color color);

int StringWidth(const rgb_matrix::Font& font, const std::string& str);

#endif // UTILS_H
