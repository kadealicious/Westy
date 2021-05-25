#ifndef DEFERREDRENDERER_H_
#define DEFERREDRENDERER_H_

#include"globals.h"

/* Create g-buffer and all sub-buffers.  When rendering, call wsDefRenGeometryPass() before 
	you call any external rendering functions.  DO NOT change the active shader!!  After all 
	desired targets for deferred shading have been rendered, call wsDefRenLightPass(). */
void wsDefRenInit();
// Perform geometry pass using g-buffer.
void wsDefRenGeometryPass(mat4 *model, mat4 *view, mat4 *projection, unsigned int cameraID);
// Perform lighting pass and render all information to a quad which fills the screen.
void wsDefRenLightPass(unsigned int cameraID);
// Destroy buffers and such.  Don't forget to call this!!
void wsDefRenTerminate();

#endif