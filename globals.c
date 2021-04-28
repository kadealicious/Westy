#include"globals.h"

float wsInterpF(float start, float target, float damp)
	{ return (target - start) * damp; }
double wsInterpD(double start, double target, float damp)
	{ return (target - start) * damp; }
int wsInterpI(int start, int target, float damp)
	{ return (int)((target - start) * damp); }

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

void wsPrintVec2(vec2 *vec, const char *tag)
	{ printf("'%s' vec2 @ %p:\t%.2f %.2f\n", tag, vec, (*vec)[0], (*vec)[1]); }
void wsPrintVec3(vec3 *vec, const char *tag)
	{ printf("'%s' vec3 @ %p:\t%.2f %.2f %.2f\n", tag, vec, (*vec)[0], (*vec)[1], (*vec)[2]); }
void wsPrintVec4(vec4 *vec, const char *tag)
	{ printf("'%s' vec4 @ %p:\t%.2f %.2f %.2f %.2f\n", tag, vec, (*vec)[0], (*vec)[1], (*vec)[2]); }