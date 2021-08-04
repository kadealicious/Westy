#ifndef EXPLORATION_H_
#define EXPLORATION_H_

#include"globals.h"

void wsExploreRenderUI(unsigned int text_shader, mat4 *matrix_ortho);

void wsExploreInit();
void wsExploreUpdate(unsigned int delta_time);
void wsExploreTerminate();

#endif /* EXPLORATION_H_ */