#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#include"graphics.h"
#include"deferredrenderer.h"
#include"shader.h"
#include"model.h"
#include"material.h"
#include"light.h"
#include"text.h"
#include"input.h"
#include"lobj.h"

// To set vertex attribute pointer(s): 
/* First set the index/location of the pointer.  Then set the size, type, and if coordinates are normalized.
	The stride is equal to the total size of all vertex attribute pointers.  Ex. if we have 5 floats in total, 
	we have a stride of 5 * sizeof(float).  finally, set the *pointer parameter to 
	(void*)(END_INDEX_OF_PREV_ATTRIB * sizeof(float)).  Finally, enable the vertex attribute pointer.  You will 
	forget this part. */

unsigned int text_vbo, vbo;
unsigned int text_vao, cube_vao, light_vao;

short camera_active = WS_CAMERA_OFF;

unsigned int text_shader = WS_NONE, text_billboard_shader = WS_NONE;
unsigned int light_shader = WS_NONE;

unsigned int cube_diffuse_map;
unsigned int cube_specular_map;

unsigned int mesh_teapot;
unsigned int mesh_canyon;
wsModel cube_model;
wsMaterial cube_material;

vec3 cube_positions[] = {
    { 0.0f,  0.0f,  0.0f}, 
    { 2.0f,  5.0f, -15.0f}, 
    {-1.5f, -2.2f, -2.5f},  
    { -3.8f, -2.0f, -12.3f},  
    { 2.4f, -0.4f, -3.5f},  
    { 1.7f,  3.0f, -7.5f},  
    { 1.5f,  2.0f, -2.5f}, 
    { 1.3f, -2.0f, -2.5f},  
    { 1.5f,  0.2f, -1.5f}, 
    {-1.3f,  1.0f, -1.5f}  
};

vec3 pointlight_positions[WS_MAX_POINTLIGHTS];

int wsGraphicsInit();
void wsGraphicsInitText();
void wsGraphicsInitLighting();
void wsGraphicsInitTestCube();

void wsGraphicsRender();
void wsGraphicsTerminate();

void wsGraphicsLoadTexture(const char *path, unsigned int *dest_texture, unsigned int wrap_style, unsigned int filter_style);

int wsGraphicsInit() {
	stbi_set_flip_vertically_on_load(true);
	
	// OpenGL stuff.
	// glClearColor(0.025f, 0.025f, 0.025f, 1.0f);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_FRAMEBUFFER_SRGB);
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);
	
	wsDefRenInit();
	wsGraphicsInitText();
	wsGraphicsInitLighting();
	wsGraphicsInitTestCube();
	
	camera_active = wsCameraInit((vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, -90.0f, 0.0f}, 90.0f);
	wsCameraInit((vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, -90.0f, 0.0f}, 90.0f);
	
	ojInit();
	mesh_teapot = ojLoadOBJ("models/test/teapot.obj");
	
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
	int spread = 15;
	for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++) {
		pointlight_positions[i][0] = (rand() % spread) - (spread * 0.5f);
		pointlight_positions[i][1] = (rand() % spread) - (spread * 0.5f);
		pointlight_positions[i][2] = (rand() % spread) - (spread * 0.5f);
	}
	
	if(light_shader == WS_NONE)	light_shader = wsShaderLoad("shaders/lightsource.vert", "shaders/lightsource.frag", false, false);
	
	for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++)		wsLightInitp(i, pointlight_positions[i], (vec3){0.4f, 0.85f, 0.0f}, 0.5f, 0.5f, true);
	for(unsigned int i = 0; i < WS_MAX_SPOTLIGHTS; i++)			wsLightInitf(0, cameras.position[camera_active], cameras.rotation[camera_active], (vec3){0.7f, 0.7f, 0.5f}, 1.0f, 0.5f, 17.5f, false);
	for(unsigned int i = 0; i < WS_MAX_DIRECTIONLIGHTS; i++)	wsLightInitd(0, (vec3){-0.2f, -1.0f, -0.3f}, (vec3){1.0f, 1.0f, 1.0f}, 0.1f, false);
}
void wsGraphicsInitTestCube() {
	// Test cube.
	wsMaterialInit(&cube_material, (vec3){1.0f, 0.5f, 0.31f}, (vec3){1.0f, 0.5f, 0.31f}, (vec3){0.5f, 0.5f, 0.5f}, 8.0f);
	wsModelInit(&cube_model);
	wsGraphicsLoadTexture("textures/test/woodcrate_diffuse.png", &cube_diffuse_map, GL_CLAMP_TO_EDGE, GL_NEAREST);
	wsGraphicsLoadTexture("textures/test/woodcrate_specular.png", &cube_specular_map, GL_CLAMP_TO_EDGE, GL_NEAREST);
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, cube_model.num_verts * sizeof(float), cube_model.vertices, GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	// Light cube.
	glGenVertexArrays(1, &light_vao);
	glBindVertexArray(light_vao);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void wsGraphicsLoadTexture(const char *path, unsigned int *dest_texture, unsigned int wrap_style, unsigned int filter_style) {
	glGenTextures(1, dest_texture);
	glBindTexture(GL_TEXTURE_2D, *dest_texture);
	
	// Clamping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_style);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_style);
	// Filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_style);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_style);
	
	int img_width, img_height, img_channel_num;
	unsigned char *img_data = stbi_load(path, &img_width, &img_height, &img_channel_num, 0);
	if(img_data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
		glGenerateMipmap(GL_TEXTURE_2D);
		printf("Image data '%s' populated\n", path);
	} else printf("ERROR - Failed to load image data at '%s'\n", path);
	stbi_image_free(img_data);
}

void wsGraphicsRenderMesh(mat4 *matrix_model, unsigned int shaderID, unsigned int meshID) {
	glm_mat4_identity(*matrix_model);
	glm_translate(*matrix_model, (vec3){0.0f, 1.0f, 3.0f});
	wsShaderSetMat4(shaderID, "model", matrix_model);
	wsShaderSetNormalMatrix(shaderID, matrix_model);
	
	glBindVertexArray(ojGetVAO(meshID));
	glDrawArrays(GL_TRIANGLES, 0, ojGetBufferSize(meshID));
}

// The exciting stuff!
void wsGraphicsRender() {
	mat4 matrix_model;
	mat4 matrix_view;
	mat4 matrix_perspective;
	mat4 matrix_ortho;
	
	// Rendering stuff.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Get perspective and ortho matrices for camera.
	glm_perspective(glm_rad(cameras.fov[camera_active]), (float)screen_width / screen_height, 0.1f, 100.0f, matrix_perspective);
	glm_ortho(0.0f, screen_width, 0.0f, screen_height, 0.0f, 1000.0f, matrix_ortho);
	
	// Handle FPS camera controls, then generate new lookat matrix (for view matrix) using camera position + rotation.
	wsCameraMakeFPS(camera_active, &matrix_view, 2.5f, 89.0f);
	
	if(wsInputGetPress(GLFW_KEY_G))
		camera_active = (camera_active == WS_CAMERA0) ? WS_CAMERA1 : WS_CAMERA0;
	if(wsInputGetPress(GLFW_KEY_Z)) {
		unsigned int polygon_mode;
		glGetIntegerv(GL_POLYGON_MODE, &polygon_mode);
		glPolygonMode(GL_FRONT_AND_BACK, polygon_mode == GL_FILL ? GL_LINE : GL_FILL);
	}
	
	// Turn off blending for deferred pass, begin deferred rendering.
	glDisable(GL_BLEND);
	wsDefRenGeometryPass(&matrix_model, &matrix_view, &matrix_perspective, camera_active);
	wsGraphicsWorldRender(&matrix_model, &matrix_view, &matrix_perspective, &matrix_ortho);
	wsDefRenLightPass(camera_active);
	// End deferred rendering.
	
	// Turn on blending for forward pass, set blend function, begin forward rendering.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	wsGraphicsLightsRender(&matrix_model, &matrix_view, &matrix_perspective, &matrix_ortho);
	wsGraphicsUIRender(&matrix_model, &matrix_view, &matrix_perspective, &matrix_ortho);
	// End forward rendering.
}

// Render world.
void wsGraphicsWorldRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho) {
	unsigned int shader_active = wsShaderGetActive();
	
	// Test cubes.
	for(unsigned int i = 0; i < 10; i++) {
		// Transform, rotate, then translate model matrix.  matrix multiplication is done in reverse.
		glm_mat4_identity(*matrix_model);
		glm_translate(*matrix_model, cube_positions[i]);
		glm_rotate(*matrix_model, glm_rad(sin(glfwGetTime() * (0.65f)) * 125.0f * (i / 10.0f)), (vec3){0.0f, 1.0f, 0.0f});
		glm_rotate(*matrix_model, glm_rad(cos(glfwGetTime() * (0.65f * (i / 10.0f))) * 125.0f), (vec3){0.0f, 0.0f, 1.0f});
		wsShaderSetMat4(shader_active, "model", matrix_model);
		wsShaderSetNormalMatrix(shader_active, matrix_model);
		
		// Bind diffuse + specular textures to their respective units.  Should probably look into specular as alpha in diffuse map?
		glActiveTexture(GL_TEXTURE0);
		wsShaderSetInt(shader_active, "texture_diffuse", 0);
		glBindTexture(GL_TEXTURE_2D, cube_diffuse_map);
		glActiveTexture(GL_TEXTURE1);
		wsShaderSetInt(shader_active, "texture_specular", 1);
		glBindTexture(GL_TEXTURE_2D, cube_specular_map);
		
		// Draw cube.
		glBindVertexArray(cube_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	
	// Test teapot.
	wsGraphicsRenderMesh(matrix_model, shader_active, mesh_teapot);
}

// Render lights.
void wsGraphicsLightsRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho) {
	// Point lights.
	static bool pointlight_toggle = true;
	if(wsInputGetPress(GLFW_KEY_1)) {
		pointlight_toggle = !pointlight_toggle;
		for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++)
			wsLightTogglep(i, pointlight_toggle);
		if(wsLightGetTogglep(0))
			printf("Turning on pointlights...\n");
		else printf("Turning off pointlights...\n");
	}
	for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++) {
		if(pointlight_toggle) {
			float light_toggle = fabs(sin(glfwGetTime() + (i * 0.775f)));
			wsLightTogglep(i, light_toggle > 0.975f ? true : false);
		}
	}
	// Fireflies!
	for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++) {
		srand(i);
		float seedval =  ((rand() % 8) - 4) * 0.4f;
		float x = pointlight_positions[i][0] + sin(glfwGetTime() * seedval) * 0.3f;
		float y = pointlight_positions[i][1] + cos(glfwGetTime() * seedval) * 0.3f;
		float z = pointlight_positions[i][2] - sin(glfwGetTime() * seedval + 0.25f) * 0.3f;
		wsLightSetPositionp(i, (vec3){x, y, z});
	}
	
	// Spotlights.
	static bool flashlight_toggle = false;
	if(wsInputGetPress(GLFW_KEY_2)) {
		flashlight_toggle = !flashlight_toggle;
		wsLightTogglef(0, flashlight_toggle);
		if(wsLightGetTogglef(0))
			printf("Turning on spotlights...\n");
		else printf("Turning off spotlights...\n");
	}
	// Flashlight.
	wsLightSetPositionf(0, cameras.position[camera_active]);
	wsLightSetRotationf(0, cameras.rotation[camera_active]);
	
	// Directional lights.
	static bool directionlight_toggle = false;
	if(wsInputGetPress(GLFW_KEY_3)) {
		directionlight_toggle = !directionlight_toggle;
		wsLightToggled(0, directionlight_toggle);
		if(wsLightGetToggled(0))
			printf("Turning on directionlights...\n");
		else printf("Turning off directionlights...\n");
	}
	
	// Draw pointlight cubes.
	for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++) {
		glm_mat4_identity(*matrix_model);
		glm_translate(*matrix_model, pointlights.position[i]);
		glm_scale(*matrix_model, (vec3){0.2f, 0.2f, 0.2f});
		
		wsShaderUse(light_shader);
		wsShaderSetVec3(light_shader, "light_color", pointlights.color[i]);
		wsShaderSetInt(light_shader, "toggle", (int)pointlights.toggle[i]);
		wsShaderSetMVP(light_shader, matrix_model, matrix_view, matrix_perspective);
		
		glBindVertexArray(light_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

// Render UI.
void wsGraphicsUIRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho) {
	// Cube labels.
	for(unsigned int i = 0; i < 10; i++) {
		vec3 font_color = {fabs(sin(i * 1.23f)) + 0.25f, fabs(cos(i * 2.73f)) + 0.25f, fabs(sin(i * 0.5f)) + 0.25f};
		char cube_name[2];
		vec3 cube_label_position = {cube_positions[i][0], cube_positions[i][1] + 1.0f, cube_positions[i][2]};
		sprintf(cube_name, "C%d", i);
		wsTextBillboardRender(text_billboard_shader, &cube_name[0], cube_label_position, 0.005f, font_color, camera_active, matrix_view, matrix_perspective);
	}
	
	// UI labels.
	wsTextRender(text_shader, "The quick brown fox jumps over the lazy dog!", (vec2){910.0f, 1030.0f}, 0.35f, (vec3){1.0f, 1.0f, 0.5f}, matrix_ortho);
	
	wsTextRender(text_shader, "[W][A][S][D] for z/x-axis movement", (vec2){10.0f, 1050.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[CTRL][SPACE] for y-axis movement", (vec2){10.0f, 1025.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[1] to toggle point lights", (vec2){10.0f, 1000.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[2] to toggle spotlights", (vec2){10.0f, 975.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[3] to toggle directional lights", (vec2){10.0f, 950.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[G] to switch between cameras", (vec2){10.0f, 925.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[Z] to toggle wireframe mode", (vec2){10.0f, 900.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	
	wsTextRender(text_shader, "kade samson", (vec2){575.0f, 1050.0f}, 0.1f, (vec3){0.5f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "version 66642069", (vec2){575.0f, 1040.0f}, 0.1f, (vec3){0.5f, 1.0f, 1.0f}, matrix_ortho);
}

void wsGraphicsResize() {
	wsDefRenTerminate();
	wsDefRenInit();
}

void wsGraphicsTerminate() {
	wsModelTerminate(&cube_model);
	ojDeleteOBJ(mesh_teapot);
	ojTerminate();
	
	wsTextTerminate();
	wsDefRenTerminate();
	
	glDeleteVertexArrays(1, &cube_vao);
	glDeleteVertexArrays(1, &light_vao);
	
	glDeleteBuffers(1, &vbo);
	
	wsShaderDelete(&light_shader);
	wsShaderDelete(&text_shader);
	wsShaderDelete(&text_billboard_shader);
	
	printf("Graphics terminated\n");
}