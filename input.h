#ifndef INPUT_H_
#define INPUT_H_

#include"globals.h"

char key_once[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(window, key)             \
    (glfwGetKey(window, key) ?              \
     (key_once[key] ? false : (key_once[key] = true)) :   \
     (key_once[key] = false))

void wsInputInit(GLFWwindow *window, float sensitivity);

bool wsInputGetHold(int key);
bool wsInputGetPress(int key);
bool wsInputGetRelease(int key);
float wsInputGetMousePosX();
float wsInputGetMousePosY();
float wsInputGetMouseMoveX();
float wsInputGetMouseMoveY();
float wsInputGetMouseScrollY();
void wsInputUpdate();

void wsInputKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void wsInputCursorPosCallback(GLFWwindow *window, double posx, double posy);
void wsInputScrollCallback(GLFWwindow *window, double offsetx, double offsety);

#endif