#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include"globals.h"
#include"camera.h"

// Initializes models, cameras, lighting, UI.
unsigned short wsGraphicsInit();

// Call all render functions in Graphics.
void wsGraphicsRender();
void wsGraphicsWorldRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho);
void wsGraphicsUIRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho);

// Free stuff.
void wsGraphicsTerminate();

// Load a texture using the stbi lib.
void wsGraphicsLoadTexture(const char *path, unsigned int *dest_texture, unsigned int gl_texture_unit, unsigned int wrap_style, unsigned int filter_style);

#endif