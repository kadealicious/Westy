#ifndef GLOBALS_H_
#define GLOBALS_H_

#define GLEW_STATIC
#include<GL/glew.h>
#include"GLFW/glfw3.h"
#include<CGLM/cglm.h>

#include<ft2build.h>
#include<freetype/freetype.h>

char key_once[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(window, key)             \
    (glfwGetKey(window, key) ?              \
     (key_once[key] ? false : (key_once[key] = true)) :   \
     (key_once[key] = false))

extern const vec3 WORLD_UP;

extern int window_width;
extern int window_height;
extern unsigned int target_fps;
extern float delta_time;
extern float field_of_view;

float wsInterpF(float start, float target, float damp);
double wsInterpD(double start, double target, float damp);
int wsInterpI(int start, int target, float damp);

float wsGetAspectRatio();

void wsPrintMat4(mat4 *mat, const char *tag);
void wsPrintVec2(vec2 *vec, const char *tag);
void wsPrintVec3(vec3 *vec, const char *tag);
void wsPrintVec4(vec4 *vec, const char *tag);

#endif