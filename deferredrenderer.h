#ifndef DEFERREDRENDERER_H_
#define DEFERREDRENDERER_H_

#include"globals.h"

// Create g-buffer and all sub-buffers.
void wsDefRenInit();
// Perform geometry pass using g-buffer.
void wsDefRenGeometryPass(mat4 *model, mat4 *view, mat4 *projection, unsigned int cameraID);
// Perform lighting pass and render all information to a quad which fills the screen.
void wsDefRenLightPass(unsigned int cameraID);
// For when the window is resized.
void wsDefRenResize();
// Destroy buffers and such.  Don't forget to call this!!
void wsDefRenTerminate();

#endif