#ifndef DRAWUTILS_H
#define DRAWUTILS_H

#include <canvas.h>
#include <graphics.h>

void drawCircleFill(rgb_matrix::Canvas* canvas,
                    int cx, int cy, int r, 
                    rgb_matrix::Color color);

#endif // DRAWUTILS_H
