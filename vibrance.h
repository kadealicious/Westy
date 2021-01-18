#ifndef SHADERS_H_
#define SHADERS_H_

#include<stdbool.h>

#include"globals.h"

unsigned int vibrance_LoadShader(const char*, const char*);
void vibrance_UseShader(unsigned int id);
void vibrance_SetBool(unsigned int, const char*, bool);
void vibrance_SetInt(unsigned int, const char*, int);
void vibrance_SetFloat(unsigned int, const char*, float);

#endif