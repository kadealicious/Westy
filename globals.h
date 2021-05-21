#ifndef GLOBALS_H_
#define GLOBALS_H_

#define GLEW_STATIC
#include<GL/glew.h>
#include"GLFW/glfw3.h"
#include<CGLM/cglm.h>

#include<ft2build.h>
#include<freetype/freetype.h>

enum ERROR_STATES { WS_OKAY, WS_QUIT_NORMAL, WS_QUIT_NORMAL_CONSOLE, WS_ERROR_GLFW, WS_ERROR_GLEW, 
	WS_ERROR_FREETYPE, WS_ERROR_UNKNOWN };

extern const vec3 WS_WORLD_UP;
extern const vec3 WS_WORLD_RIGHT;
extern const vec3 WS_WORLD_FORWARD;

extern int window_width;
extern int window_height;
extern int screen_width;
extern int screen_height;
extern unsigned int target_fps;
extern float delta_time;

float wsInterpf(float num0, float num1, float interp);
double wsInterpd(double num0, double num1, float interp);
int wsInterpi(int num0, int num1, float interp);

float wsGetAspectRatio();

void wsPrintMat4(mat4 *mat, const char *tag);
void wsPrintVec2(vec2 *vec, const char *tag);
void wsPrintVec3(vec3 *vec, const char *tag);
void wsPrintVec4(vec4 *vec, const char *tag);

#endif