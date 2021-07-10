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

unsigned int mesh_desert;
unsigned int desert_diffuse_map;

unsigned int mesh_cube;
unsigned int cube_diffuse_map;
unsigned int cube_specular_map;

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

void wsGraphicsRender(bool is_paused);
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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	wsDefRenInit();
	wsGraphicsInitText();
	wsGraphicsInitLighting();
	
	wsCameraSetActive(wsCameraInit((vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, -90.0f, 0.0f}, 90.0f));
	wsCameraInit((vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, -90.0f, 0.0f}, 90.0f);
	
	wsObjInit();
	// mesh_teapot = ojLoadOBJ("models/test/teapot.obj", OJ_FACES);
	// mesh_teapot = wsObjLoad("models/test/teapot_normals.obj", WS_FACES | WS_NORMALS);
	
	mesh_desert = wsObjLoad("models/test/rocky-desert-terrain/untitled.obj", WS_FACES | WS_NORMALS | WS_TEX);
	wsGraphicsLoadTexture("models/test/rocky-desert-terrain/RockyDesert_Bitmap_Output_4096.png", &desert_diffuse_map, GL_CLAMP_TO_EDGE, GL_NEAREST);
	
	mesh_cube = wsObjLoad("models/test/crate/cube.obj", WS_FACES | WS_NORMALS | WS_TEX);
	wsGraphicsLoadTexture("models/test/crate/woodcrate_diffuse.png", &cube_diffuse_map, GL_CLAMP_TO_EDGE, GL_NEAREST);
	wsGraphicsLoadTexture("models/test/crate/woodcrate_specular.png", &cube_specular_map, GL_CLAMP_TO_EDGE, GL_NEAREST);
	
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
	wsLightInitf(0, cameras.position[wsCameraGetActive()], cameras.rotation[wsCameraGetActive()], (vec3){0.7f, 0.7f, 0.5f}, 1.0f, 0.5f, 17.5f, false);
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
	wsShaderSetMat4(shaderID, "model", matrix_model);
	wsShaderSetNormalMatrix(shaderID, matrix_model);
	
	glBindVertexArray(wsObjGetVAO(meshID));
	glDrawArrays(GL_TRIANGLES, 0, wsObjGetBufferSize(meshID));
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
	
	// Test cubes.
	for(unsigned int i = 0; i < 10; i++) {
		// Transform, rotate, then translate model matrix.  matrix multiplication is done in reverse.
		glm_mat4_identity(*matrix_model);
		glm_translate(*matrix_model, cube_positions[i]);
		glm_rotate(*matrix_model, glm_rad(sin(glfwGetTime() * (0.65f)) * 125.0f * (i / 10.0f)), (vec3){0.0f, 1.0f, 0.0f});
		glm_rotate(*matrix_model, glm_rad(cos(glfwGetTime() * (0.65f * (i / 10.0f))) * 125.0f), (vec3){0.0f, 0.0f, 1.0f});
		glm_scale(*matrix_model, (vec3){0.5f, 0.5f, 0.5f});
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
		wsGraphicsRenderMesh(matrix_model, shader_active, mesh_cube);
	}
	
	// Desert landscape.
	glActiveTexture(GL_TEXTURE0);
	wsShaderSetInt(shader_active, "texture_diffuse", 0);
	glBindTexture(GL_TEXTURE_2D, desert_diffuse_map);
	glm_mat4_identity(*matrix_model);
	
	const float scale = 11.5f;
	glm_translate(*matrix_model, (vec3){0.0f, -20.0f, 0.0f});
	glm_scale(*matrix_model, (vec3){scale, scale, scale});
	wsGraphicsRenderMesh(matrix_model, shader_active, mesh_desert);
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
	
	// TODO: Make this either follow daylight cycle or turn on a bright interrogation light.
	// Directional lights.
	static bool directionlight_toggle = false;
	if(wsInputGetPress(GLFW_KEY_3)) {
		directionlight_toggle = !directionlight_toggle;
		wsLightToggled(0, directionlight_toggle);
		if(wsLightGetToggled(0))
			printf("Turning on directionlights...\n");
		else printf("Turning off directionlights...\n");
	}
	
	// Draw pointlight cubes.  Must be done in forward render pass.
	for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++) {
		glm_mat4_identity(*matrix_model);
		glm_translate(*matrix_model, pointlights.position[i]);
		glm_scale(*matrix_model, (vec3){0.1f, 0.1f, 0.1f});
		
		wsShaderUse(light_shader);
		wsShaderSetVec3(light_shader, "light_color", pointlights.color[i]);
		wsShaderSetInt(light_shader, "toggle", (int)pointlights.toggle[i]);
		wsShaderSetMVP(light_shader, matrix_model, matrix_view, matrix_perspective);
		
		wsGraphicsRenderMesh(matrix_model, light_shader, mesh_cube);
	}
	
	// Update all lights.
	wsShaderUpdateLightsp(wsShaderGetActive());
	wsShaderUpdateLightsf(wsShaderGetActive());
	wsShaderUpdateLightsd(wsShaderGetActive());
}

// Render UI.
void wsGraphicsUIRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho) {
	// Cube labels.
	for(unsigned int i = 0; i < 10; i++) {
		vec3 font_color = {fabs(sin(i * 1.23f)) + 0.25f, fabs(cos(i * 2.73f)) + 0.25f, fabs(sin(i * 0.5f)) + 0.25f};
		char cube_name[2];
		vec3 cube_label_position = {cube_positions[i][0], cube_positions[i][1] + 1.0f, cube_positions[i][2]};
		sprintf(cube_name, "C%d", i);
		wsTextBillboardRender(text_billboard_shader, &cube_name[0], cube_label_position, 0.005f, font_color, wsCameraGetActive(), matrix_view, matrix_perspective);
	}
	
	// Render game UI.
	ExploreRenderUI(text_shader, matrix_ortho);
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
	wsObjTerminate();
	
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