#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include"graphics.h"
#include"deferredrenderer.h"
#include"shader.h"
#include"light.h"
#include"text.h"
#include"input.h"
#include"lobj.h"
#include"texturing.h"
#include"entities.h"
#include"exploration.h"

// To set vertex attribute pointer(s): 
/* First set the index/location of the pointer.  Then set the size, type, and if coordinates are normalized.
	The stride is equal to the total size of all vertex attribute pointers.  Ex. if we have 5 floats in total, 
	we have a stride of 5 * sizeof(float).  finally, set the *pointer parameter to 
	(void*)(END_INDEX_OF_PREV_ATTRIB * sizeof(float)).  Finally, enable the vertex attribute pointer.  You will 
	forget this part. */

unsigned int text_vbo, vbo;
unsigned int text_vao, cube_vao, light_vao;

unsigned int text_shader = WS_NONE, text_billboard_shader = WS_NONE;
unsigned int light_shader = WS_NONE;

unsigned int mesh_cube;
unsigned int mesh_lightball;

int wsGraphicsInit();
void wsGraphicsInitText();
void wsGraphicsInitLighting();

void wsGraphicsRender(bool is_paused);
void wsGraphicsTerminate();

int wsGraphicsInit() {
	// OpenGL stuff.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	wsLOBJInit();
	wsTexturingInit();
	wsDefRenInit();
	wsGraphicsInitText();
	wsGraphicsInitLighting();
	
	mesh_cube = wsMeshLoadOBJ("models/test/cube.obj", WS_FACES | WS_TEX | WS_NORMALS);
	mesh_lightball = wsMeshLoadOBJ("models/test/lightball.obj", WS_FACES);
	
	printf("Graphics initialized\n");
	return WS_OKAY;
}

void wsGraphicsInitText() {
	if(text_shader == WS_NONE)				text_shader = wsShaderLoad("shaders/text.vert", "shaders/text.frag", false, false);
	if(text_billboard_shader == WS_NONE)	text_billboard_shader = wsShaderLoad("shaders/text_billboard.vert", "shaders/text_billboard.frag", false, false);
	
	wsTextInit();
	wsTextLoadFace("fonts/Roboto_Mono/static/RobotoMono-Medium.ttf", 108);
}
void wsGraphicsInitLighting() {
	if(light_shader == WS_NONE)	light_shader = wsShaderLoad("shaders/lightsource.vert", "shaders/lightsource.frag", false, false);
}

void wsGraphicsRenderMesh(mat4 *matrix_model, unsigned int shaderID, unsigned int meshID) {
	wsShaderSetMat4(shaderID, "model", matrix_model);
	wsShaderSetNormalMatrix(shaderID, matrix_model);
	
	glBindVertexArray(wsMeshGetVAO(meshID));
	glDrawArrays(GL_TRIANGLES, 0, wsMeshGetBufferSize(meshID));
}
void wsGraphicsRenderObjects(mat4 *matrix_model, unsigned int shaderID) {
	for(size_t i = 0; i < WS_MAX_OBJS; i++) {
		// If there is no mesh to draw, don't.
		if(wsObjectGetMeshID(i) == WS_NONE || wsObjectGetVisible(i) == false || wsObjectGetDiffuseID(i) == WS_NONE)
			continue;
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, wsTextureGetGL(wsObjectGetDiffuseID(i)));
		wsShaderSetInt(shaderID, "texture_diffuse", 0);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wsTextureGetGL(wsObjectGetSpecularID(i)));
		wsShaderSetInt(shaderID, "texture_specular", 1);
		
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, wsTextureGetGL(wsObjectGetNormalID(i)));
		wsShaderSetInt(shaderID, "texture_normal", 2);
		
		vec3 obj_position = {wsObjectGetPositionX(i), wsObjectGetPositionY(i), wsObjectGetPositionZ(i)};
		vec3 obj_rotation = {wsObjectGetRotationX(i), wsObjectGetRotationY(i), wsObjectGetRotationZ(i)};
		
		glm_mat4_identity(*matrix_model);
		glm_translate(*matrix_model, obj_position);
		glm_rotate(*matrix_model, obj_rotation[PITCH], (float*)WS_WORLD_RIGHT);
		glm_rotate(*matrix_model, obj_rotation[YAW], (float*)WS_WORLD_UP);
		glm_rotate(*matrix_model, obj_rotation[ROLL], (float*)WS_WORLD_FORWARD);
		wsShaderSetMat4(shaderID, "model", matrix_model);
		wsShaderSetNormalMatrix(shaderID, matrix_model);
		
		unsigned int meshID = wsObjectGetMeshID(i);
		glBindVertexArray(wsMeshGetVAO(meshID));
		glDrawArrays(GL_TRIANGLES, 0, wsMeshGetBufferSize(meshID));
	}
}

// The exciting stuff!
void wsGraphicsRender(bool is_paused) {
	mat4 matrix_model;
	mat4 matrix_view;
	mat4 matrix_perspective;
	mat4 matrix_ortho;
	
	// Rendering stuff.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Get perspective and ortho matrices for camera.
	glm_perspective(glm_rad(cameras.fov[wsCameraGetActive()]), (float)screen_width / screen_height, 0.1f, 100.0f, matrix_perspective);
	glm_ortho(0.0f, screen_width, 0.0f, screen_height, 0.0f, 1000.0f, matrix_ortho);
	wsCameraGenViewMatrix(wsCameraGetActive(), &matrix_view);
	
	// Turn off blending for deferred pass, begin deferred rendering.
	glDisable(GL_BLEND);
	wsDefRenGeometryPass(&matrix_model, &matrix_view, &matrix_perspective, wsCameraGetActive());
	wsGraphicsWorldRender(&matrix_model, &matrix_view, &matrix_perspective, &matrix_ortho);
	wsDefRenLightPass(wsCameraGetActive());
	// End deferred rendering.
	
	// Turn on blending for forward pass, set blend function, begin forward rendering.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	wsGraphicsLightsRender(&matrix_model, &matrix_view, &matrix_perspective, &matrix_ortho);
	wsGraphicsUIRender(&matrix_model, &matrix_view, &matrix_perspective, &matrix_ortho);
	if(is_paused)
		wsGraphicsPauseUIRender(&matrix_model, &matrix_view, &matrix_perspective, &matrix_ortho);
	// End forward rendering.
}

// Render world.
void wsGraphicsWorldRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho) {
	unsigned int shader_active = wsShaderGetActive();
	
	wsGraphicsRenderObjects(matrix_model, shader_active);
}

// Render lights.
void wsGraphicsLightsRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho) {
	// Draw pointlight cube.  Must be done in forward render pass.
	for(size_t i = 0; i < WS_MAX_POINTLIGHTS; i++) {
		if(!pointlights.toggle[i])
			continue;
		
		glm_mat4_identity(*matrix_model);
		glm_translate(*matrix_model, pointlights.position[i]);
		glm_scale(*matrix_model, (vec3){0.2f, 0.2f, 0.2f});
		
		wsShaderUse(light_shader);
		wsShaderSetVec3(light_shader, "light_color", pointlights.color[i]);
		wsShaderSetInt(light_shader, "toggle", (int)pointlights.toggle[i]);
		wsShaderSetMVP(light_shader, matrix_model, matrix_view, matrix_perspective);
		
		wsGraphicsRenderMesh(matrix_model, light_shader, mesh_lightball);
	}
}

// Render UI.
void wsGraphicsUIRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho) {
	// Render game UI.
	wsExploreRenderUI(text_shader, matrix_ortho);
}

// Render Pause UI.
void wsGraphicsPauseUIRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho) {
	wsTextRender(text_shader, "The quick brown fox jumps over the lazy dog!", (vec2){910.0f, 1030.0f}, 0.35f, (vec3){1.0f, 1.0f, 0.5f}, matrix_ortho);
	
	wsTextRender(text_shader, "[W][A][S][D] for z/x-axis movement", (vec2){10.0f, 1050.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[CTRL][SPACE] for y-axis movement", (vec2){10.0f, 1025.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[1] to toggle point lights", (vec2){10.0f, 1000.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[2] to toggle spotlights", (vec2){10.0f, 975.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[3] to toggle directional lights", (vec2){10.0f, 950.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[G] to switch between cameras", (vec2){10.0f, 925.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[ESC] to toggle menu", (vec2){10.0f, 875.0f}, 0.2f, (vec3){.8f, 0.2f, 0.4f}, matrix_ortho);
	wsTextRender(text_shader, "[F1] to exit program", (vec2){10.0f, 850.0f}, 0.2f, (vec3){.8f, 0.2f, 0.4f}, matrix_ortho);
	wsTextRender(text_shader, "[F2] to exit program w/ console", (vec2){10.0f, 825.0f}, 0.2f, (vec3){.8f, 0.2f, 0.4f}, matrix_ortho);
	
	wsTextRender(text_shader, "kade samson", (vec2){575.0f, 1050.0f}, 0.1f, (vec3){0.5f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "version 66642069", (vec2){575.0f, 1040.0f}, 0.1f, (vec3){0.5f, 1.0f, 1.0f}, matrix_ortho);
}

void wsGraphicsResize() {
	wsDefRenTerminate();
	wsDefRenInit();
}

void wsGraphicsTerminate() {
	wsTextTerminate();
	wsTexturingTerminate();
	wsLOBJTerminate();
	wsDefRenTerminate();
	
	glDeleteVertexArrays(1, &cube_vao);
	glDeleteVertexArrays(1, &light_vao);
	
	glDeleteBuffers(1, &vbo);
	
	wsShaderDelete(&light_shader);
	wsShaderDelete(&text_shader);
	wsShaderDelete(&text_billboard_shader);
	
	printf("Graphics terminated\n");
}