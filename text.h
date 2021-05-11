#ifndef FONT_H_
#define FONT_H_

#include<stdbool.h>
#include"globals.h"
#include"camera.h"

typedef struct Char {
	unsigned int textureID;
	unsigned int advance;
	vec2 size, bearing;
} Char;

// Initialize text renderer.
void wsTextInit();
// Load font face.
bool wsTextLoadFace(const char *face_path, unsigned int face_size, unsigned int gl_texture_unit);
// Render text in 2d screenspace.
void wsTextRender(unsigned int shaderID, const char* text, vec2 position, float scale, vec3 color, mat4 *matrix_ortho);
// Render billboard text in 3d worldspace.
void wsTextBillboardRender(unsigned int shaderID, const char* text, vec3 position, float scale, vec3 color,  unsigned int cameraID, mat4 *matrix_view, mat4 *matrix_perspective);
// Free memory associated with text renderer.
void wsTextTerminate();

#endif