#ifndef FONT_H_
#define FONT_H_

#include<stdbool.h>
#include"globals.h"

typedef struct Char {
	unsigned int textureID;
	unsigned int advance;
	vec2 size, bearing;
} Char;

// Initialize text renderer.
bool wsTextInit(unsigned int gl_texture_unit, mat4 *projection);
// Render text.
void wsTextRender(unsigned int shaderID, const char* text, float x, float y, float scale, vec3 color);
// Free memory associated with text renderer.
void wsTextTerminate();

#endif