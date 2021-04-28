#include<stdio.h>

#include"globals.h"
#include"input.h"

float mouse_sensitivity;

void InputInit(float sens) {
    mouse_sensitivity = sens;

    printf("Input initialized\n");
}

void InputProcess() {

}