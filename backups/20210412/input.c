#include<stdio.h>
#include<string.h>

#include"globals.h"
#include"input.h"

GLFWwindow *window;
vec2 mouse_position;
vec2 mouse_position_prev;
float mouse_sensitivity;
float mouse_scroll = 50.0f;
bool mouse_first_move = true;

void wsInputInit(GLFWwindow *window, float sensitivity) {
	window = window;
    mouse_sensitivity = sensitivity;

    printf("wsInput initialized\n");
}

bool wsInputGetPress(int key)
	{ return (glfwGetKey(window, key) == GLFW_PRESS); }
bool wsInputGetRelease(int key)
	{ return (glfwGetKey(window, key) == GLFW_RELEASE); }

float wsInputGetMousePosX()
	{ return mouse_position[0]; }
float wsInputGetMousePosY()
	{ return mouse_position[1]; }
float wsInputGetMouseMoveX()
	{ return mouse_position[0] - mouse_position_prev[0]; }
float wsInputGetMouseMoveY()
	{ return mouse_position[1] - mouse_position_prev[1]; }
float wsInputGetMouseScrollY()
	{ return mouse_scroll; }
void wsInputUpdate() {
	mouse_position_prev[0] = mouse_position[0];
	mouse_position_prev[1] = mouse_position[1];
}

void wsInputKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	
}

void wsInputCursorPosCallback(GLFWwindow *window, double posx, double posy) {
	static float posx_prev;
	static float posy_prev;
	
	if(mouse_first_move) {
		posx_prev = posx;
		posy_prev = posy;
		mouse_first_move = false;
	}
	
	float offsetx = posx - posx_prev;
	float offsety = posy_prev - posy;
	posx_prev = posx;
	posy_prev = posy;
	offsetx *= mouse_sensitivity;
	offsety *= mouse_sensitivity;
	mouse_position[0] += offsetx;
	mouse_position[1] += offsety;
}

void wsInputScrollCallback(GLFWwindow *window, double offsetx, double offsety) {
	mouse_scroll -= (float)offsety;
	if(mouse_scroll < 0.0f)
		mouse_scroll = 0.0f;
	else if(mouse_scroll > 100.0f)
		mouse_scroll = 100.0f;
}