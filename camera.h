#ifndef CAMERA_H_
#define CAMERA_H_

#include"globals.h"

#define WS_NUM_CAMERAS 10
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

typedef struct wsCameras {
	vec3 position[WS_NUM_CAMERAS];
	vec3 rotation[WS_NUM_CAMERAS];
	vec3 right[WS_NUM_CAMERAS];
	vec3 up[WS_NUM_CAMERAS];
	vec3 euler[WS_NUM_CAMERAS];
	float fov[WS_NUM_CAMERAS];
	
	vec3 euler_interp;
} wsCamera;

enum EULER_ANGLES { PITCH, YAW, ROLL };
enum RELATIVE_DIRECTIONS { FORWARD, RIGHT, UP, BACKWARD, LEFT, DOWN };

struct wsCameras cameras;

void wsCameraInit(unsigned int cameraID, vec3 position, vec3 rotation, float fov);

void wsCameraGenViewMatrix(unsigned int cameraID, mat4 *view_dest);
void wsCameraMakeFPS(unsigned int cameraID, mat4 *view_dest, float speed, float pitch_constraint);
void wsCameraFPSMove(unsigned int cameraID, vec3 move_array, float speed);
void wsCameraMouseMove(unsigned int cameraID, float offsetx, float offsety, float pitch_constraint);
void wsCameraMouseMoveDamp(unsigned int cameraID, float offsetx, float offsety, float pitch_constraint, float damp);

#endif