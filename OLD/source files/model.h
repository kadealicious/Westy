#ifndef MODEL_H_
#define MODEL_H_

#include"globals.h"

typedef struct {
	int num_verts;
	float *vertices;
} wsModel;

void wsModelInit(wsModel *model);
void wsModelTerminate(wsModel *model);

#endif