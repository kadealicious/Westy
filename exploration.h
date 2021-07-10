#ifndef EXPLORATION_H_
#define EXPLORATION_H_

#include"globals.h"

void ExploreRenderUI(unsigned int text_shader, mat4 *matrix_ortho);

void ExploreInit();
void ExploreUpdate(unsigned int delta_time);
void ExploreTerminate();

#endif /* EXPLORATION_H_ */