#ifndef CAMERA_H_
#define CAMERA_H_

/* Object-oriented version of camera system.  Don't use this. */

#include"globals.h"

typedef struct wsCamera {
	vec3 position;
	vec3 rotation;
	vec3 right;
	vec3 up;
	vec3 euler;
	float fov;
} wsCamera;

enum EULER_ANGLES { PITCH, YAW, ROLL };
enum RELATIVE_DIRECTIONS { FORWARD, RIGHT, UP, BACKWARD, LEFT, DOWN };

void wsCameraInit(wsCamera *camera, vec3 position, vec3 rotation, float fov);
void wsCameraSyncRotation(wsCamera *camera);
void wsCameraGenViewMatrix(wsCamera *camera, mat4 *view_dest);
void wsCameraMakeFPS(wsCamera *camera, mat4 *view_dest, float speed, float pitch_constraint);
void wsCameraFPSMove(wsCamera *camera, vec3 move_array, float speed);
void wsCameraMouseMove(wsCamera *camera, float offsetx, float offsety, float pitch_constraint);
void wsCameraMouseMoveDamp(wsCamera *camera, float offsetx, float offsety, float pitch_constraint, float damp);

#endif