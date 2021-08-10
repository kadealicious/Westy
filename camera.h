#ifndef CAMERA_H_
#define CAMERA_H_

#include"globals.h"

typedef struct wsCameras {
	vec3 position[WS_MAX_CAMERAS];
	vec3 rotation[WS_MAX_CAMERAS];
	vec3 right[WS_MAX_CAMERAS];
	vec3 up[WS_MAX_CAMERAS];
	vec3 euler[WS_MAX_CAMERAS];
	float fov[WS_MAX_CAMERAS];
	unsigned int active;
	
	vec3 euler_interp;
} wsCamera;

struct wsCameras cameras;

unsigned int wsCameraInit(vec3 position, vec3 rotation, float fov);

unsigned int wsCameraGetActive();
vec3 *wsCameraGetPosition(unsigned int cameraID);
vec3 *wsCameraGetRotation(unsigned int cameraID);

void wsCameraSetActive(unsigned int cameraID);
void wsCameraGenViewMatrix(unsigned int cameraID, mat4 *view_dest);
void wsCameraMakeFPS(unsigned int cameraID, float speed, float pitch_constraint);
void wsCameraMove(unsigned int cameraID, vec3 move_array, float speed);
void wsCameraMakeFly(unsigned int cameraID, float speed, float pitch_constraint);
void wsCameraFly(unsigned int cameraID, vec3 move_array, float speed);
void wsCameraMouseMove(unsigned int cameraID, float offsetx, float offsety, float pitch_constraint);
void wsCameraMouseMoveDamp(unsigned int cameraID, float offsetx, float offsety, float pitch_constraint, float damp);

#endif