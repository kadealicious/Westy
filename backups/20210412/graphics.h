#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include"globals.h"
#include"camera.h"

void wsGraphicsInit();
void wsGraphicsRender();
void wsGraphicsTerminate();

void wsGraphicsLoadTexture(const char *path, unsigned int *dest_texture, unsigned int gl_texture_unit, unsigned int wrap_style, unsigned int filter_style);

#endif