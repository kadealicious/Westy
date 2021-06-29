#ifndef GLOBALS_H_
#define GLOBALS_H_

#define GLEW_STATIC
#include<GL/glew.h>
#include"GLFW/glfw3.h"
#include<CGLM/cglm.h>

#include<ft2build.h>
#include<freetype/freetype.h>

#define WS_MAX_CAMERAS 10
#define WS_CAMERA_OFF -1
#define WS_CAMERA0 0
#define WS_CAMERA1 1
#define WS_CAMERA2 2
#define WS_CAMERA3 3
#define WS_CAMERA4 4
#define WS_CAMERA5 5
#define WS_CAMERA6 6
#define WS_CAMERA7 7
#define WS_CAMERA8 8
#define WS_CAMERA9 9

#define WS_MAX_POINTLIGHTS 50
#define WS_MAX_SPOTLIGHTS 25
#define WS_MAX_DIRECTIONLIGHTS 5

#define WS_MAX_MESHES 50

// Max index of possible chars.  Using ASCII because fuck it.
#define WS_NUM_CHARS 128

enum WS_STATES { WS_NONE = INT_MIN, WS_OKAY, WS_QUIT_NORMAL, WS_QUIT_NORMAL_CONSOLE, WS_ERROR_GLFW, WS_ERROR_GLEW, 
	WS_ERROR_FREETYPE, WS_ERROR_FILE, WS_ERROR_UNKNOWN };

extern const vec3 WS_WORLD_UP;
extern const vec3 WS_WORLD_RIGHT;
extern const vec3 WS_WORLD_FORWARD;

extern int window_width;
extern int window_height;
extern int screen_width;
extern int screen_height;
extern unsigned int target_fps;
extern float delta_time;

// Will return a float from 0.0f-[range of num0 - num1].
float wsInterpf(float num0, float num1, float interp);
// Will return a double from 0.0-[range of num0 - num1].
double wsInterpd(double num0, double num1, float interp);
// Will return an int from 0-[range of num0 - num1].
int wsInterpi(int num0, int num1, float interp);
// Will return a float from 0.0f-[num0 + num1].
float wsAddDampf(float num0, float num1, float interp);
// Will return a double from 0.0-[num0 + num1].
double wsAddDampd(double num0, double num1, float interp);
// Will return an int from 0-[num0 + num1].
int wsAddDampi(int num0, int num1, float interp);

float wsGetAspectRatio();

void wsPrintMat4(mat4 *mat, const char *tag);
void wsPrintVec2(vec2 *vec, const char *tag);
void wsPrintVec3(vec3 *vec, const char *tag);
void wsPrintVec4(vec4 *vec, const char *tag);

#endif