#ifndef GLOBALS_H_
#define GLOBALS_H_

#define GLEW_STATIC
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<CGLM/cglm.h>

// Prefixs: 
// sp_*		- Shader program

#define GlobalGetVariableName(var)	#var

extern GLFWwindow *program_window;
extern vec3 world_up;
extern vec3 world_right;
extern vec3 world_forward;
extern int screen_width;
extern int screen_height;
extern float fov;

#endif