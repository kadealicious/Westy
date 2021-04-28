#ifndef FONT_H_
#define FONT_H_

#include<stdbool.h>
#include"globals.h"

typedef struct Char {
	unsigned int textureID;
	unsigned int advance;
	vec2 size, bearing;
} Char;

bool wsTextInit(unsigned int shaderID, mat4 *projection);
void wsTextRender(unsigned int shaderID, const char* text, float x, float y, float scale, vec3 color);
void wsTextTerminate();

#endif