#include"globals.h"

const vec3 WS_WORLD_UP		= {0.0f, 1.0f, 0.0f};
const vec3 WS_WORLD_RIGHT	= {1.0f, 0.0f, 0.0f};
const vec3 WS_WORLD_FORWARD	= {0.0f, 0.0f, 1.0f};

int window_width	= 640;
int window_height	= 480;
int screen_width	= 640;
int screen_height	= 480;
unsigned int target_fps = 60;
float delta_time = 1.0f;

float wsInterpf(float num0, float num1, float interp)		{ return (num1 - num0) * interp; }
double wsInterpd(double num0, double num1, float interp)	{ return (num1 - num0) * interp; }
int wsInterpi(int num0, int num1, float interp)				{ return (int)((num1 - num0) * interp); }
float wsAddDampf(float num0, float num1, float interp)		{ return (num0 + num1) * interp; }
double wsAddDampd(double num0, double num1, float interp)	{ return (num0 + num1) * interp; }
int wsAddDampi(int num0, int num1, float interp)			{ return (int)((num0 + num1) * interp); }

float wsGetAspectRatio()
	{ return ((float)window_height / (float)window_width); }

void wsPrintMat4(mat4 *mat, const char *tag) {
	printf("'%s' mat4 %p:\n", tag, mat);
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			printf("%.2f ", (*mat)[i][j]);
		}
		printf("\n");
	}
}

void wsPrintVec2(vec2 *vec, const char *tag) { printf("'%s' vec2 @ %p:\t%.2f %.2f\n", tag, vec, (*vec)[0], (*vec)[1]); }
void wsPrintVec3(vec3 *vec, const char *tag) { printf("'%s' vec3 @ %p:\t%.2f %.2f %.2f\n", tag, vec, (*vec)[0], (*vec)[1], (*vec)[2]); }
void wsPrintVec4(vec4 *vec, const char *tag) { printf("'%s' vec4 @ %p:\t%.2f %.2f %.2f %.2f\n", tag, vec, (*vec)[0], (*vec)[1], (*vec)[2]); }