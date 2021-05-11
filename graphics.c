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

// To set vertex attribute pointer(s): 
/* First set the index/location of the pointer.  Then set the size, type, and if coordinates are normalized.
	The stride is equal to the total size of all vertex attribute pointers.  Ex. if we have 5 floats in total, 
	we have a stride of 5 * sizeof(float).  finally, set the *pointer parameter to 
	(void*)(END_INDEX_OF_PREV_ATTRIB * sizeof(float)).  Finally, enable the vertex attribute pointer.  You will 
	forget this part. */

unsigned int text_vbo, vbo;
unsigned int text_vao, cube_vao, light_vao;

short camera_active = WS_CAMERA_OFF;

unsigned int text_shader, text_billboard_shader;
unsigned int light_shader;
unsigned int cube_shader;

static unsigned int next_avaliable_texture_unit = GL_TEXTURE0;

unsigned int cube_diffuse_map;
unsigned int cube_specular_map;

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

vec3 pointlight_positions[] = {
	{ 0.7f,  0.2f,  2.0f},
	{ 2.3f, -3.3f, -4.0f},
	{-4.0f,  2.0f, -12.0f},
	{ 0.0f,  0.0f, -3.0f}
};

int wsGraphicsInit();
void wsGraphicsInitText();
void wsGraphicsInitLighting();
void wsGraphicsInitTestCube();

void wsGraphicsRender();
void wsGraphicsTerminate();

void wsGraphicsLoadTexture(const char *path, unsigned int *dest_texture, unsigned int gl_texture_unit, unsigned int wrap_style, unsigned int filter_style);
// void wsGraphicsFPSCamera(unsigned int ndx, mat4 *view_dest, float speed, float pitch_constraint);

int wsGraphicsInit() {
	stbi_set_flip_vertically_on_load(true);
	
	glClearColor(0.025f, 0.025f, 0.025f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_FRAMEBUFFER_SRGB);
	
	wsGraphicsInitText();
	wsGraphicsInitLighting();
	wsGraphicsInitTestCube();
	wsDeferredRendererInit();
	
	camera_active = WS_CAMERA0;
	wsCameraInit(camera_active, (vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, -90.0f, 0.0f}, 90.0f);
	
	printf("Graphics initialized\n");
	return WS_OKAY;
}

void wsGraphicsInitText() {
	text_shader = wsShaderLoad("shaders/text.vert", "shaders/text.frag", false, false);
	text_billboard_shader = wsShaderLoad("shaders/text_billboard.vert", "shaders/text_billboard.frag", false, false);
	
	wsTextInit();
	wsTextLoadFace("fonts/Roboto_Mono/static/RobotoMono-Medium.ttf", 108, next_avaliable_texture_unit++);
}
void wsGraphicsInitLighting() {
	light_shader = wsShaderLoad("shaders/lightsource.vert", "shaders/lightsource.frag", false, false);
	
	for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++)		wsLightQuickInitp(i, pointlight_positions[i], (vec3){0.4f, 0.85f, 0.0f}, 0.45f, 0.45f, true);
	for(unsigned int i = 0; i < WS_MAX_SPOTLIGHTS; i++)			wsLightQuickInitf(0, cameras.position[camera_active], cameras.rotation[camera_active], (vec3){0.7f, 0.7f, 0.5f}, 1.0f, 0.5f, 17.5f, false);
	for(unsigned int i = 0; i < WS_MAX_DIRECTIONLIGHTS; i++)	wsLightQuickInitd(0, (vec3){-0.2f, -1.0f, -0.3f}, (vec3){1.0f, 0.8f, 0.0f}, 0.5f, false);
}
void wsGraphicsInitTestCube() {
	cube_shader = wsShaderLoad("shaders/textured_phong_dithered.vert", "shaders/textured_phong_dithered.frag", false, false);
	wsMaterialInit(&cube_material, (vec3){1.0f, 0.5f, 0.31f}, (vec3){1.0f, 0.5f, 0.31f}, (vec3){0.5f, 0.5f, 0.5f}, 8.0f);
	wsModelInit(&cube_model);
	
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
	
	glGenVertexArrays(1, &light_vao);
	glBindVertexArray(light_vao);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	unsigned int diffuse_unit = next_avaliable_texture_unit++;
	unsigned int specular_unit = next_avaliable_texture_unit++;
	wsGraphicsLoadTexture("textures/test/woodcrate_diffuse.png", &cube_diffuse_map, diffuse_unit, GL_CLAMP_TO_EDGE, GL_NEAREST);
	wsGraphicsLoadTexture("textures/test/woodcrate_specular.png", &cube_specular_map, specular_unit, GL_CLAMP_TO_EDGE, GL_NEAREST);
	wsShaderSetInt(cube_shader, "material.diffuse_map", diffuse_unit - GL_TEXTURE0);
	wsShaderSetInt(cube_shader, "material.specular_map", specular_unit - GL_TEXTURE0);
	wsShaderSetFloat(cube_shader, "material.shininess", cube_material.shininess);
}

void wsGraphicsLoadTexture(const char *path, unsigned int *dest_texture, unsigned int gl_texture_unit, unsigned int wrap_style, unsigned int filter_style) {
	glGenTextures(1, dest_texture);
	glActiveTexture(gl_texture_unit);
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

// The exciting stuff!
void wsGraphicsRender() {
	mat4 matrix_model;
	mat4 matrix_view;
	mat4 matrix_perspective;
	mat4 matrix_ortho;
	
	// Rendering stuff.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Get perspective and ortho matrices for camera.
	glm_perspective(glm_rad(cameras.fov[camera_active]), (float)window_width / window_height, 0.1f, 100.0f, matrix_perspective);
	glm_ortho(0.0f, window_width, 0.0f, window_height, 0.0f, 1000.0f, matrix_ortho);
	
	wsCameraMakeFPS(camera_active, &matrix_view, 2.5f, 89.0f);
	wsCameraGenViewMatrix(camera_active, &matrix_view);
	
	wsGraphicsWorldRender(&matrix_model, &matrix_view, &matrix_perspective, &matrix_ortho);
	wsGraphicsUIRender(&matrix_model, &matrix_view, &matrix_perspective, &matrix_ortho);
}

// Render world.
void wsGraphicsWorldRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho) {
	// Update cube shader with time, active camera, some transformations, MVP + normal matrices, and lighting info.
	wsShaderUse(cube_shader);
	wsShaderSetFloat(cube_shader, "time", glfwGetTime());
	wsShaderUpdateCamera(cube_shader, camera_active);
	for(unsigned int i = 0; i < 10; i++) {
		glm_mat4_identity(*matrix_model);
		glm_translate(*matrix_model, cube_positions[i]);
		glm_rotate(*matrix_model, glm_rad(sin(glfwGetTime() * (0.65f)) * 125.0f * (i / 10.0f)), (vec3){0.0f, 1.0f, 0.0f});
		glm_rotate(*matrix_model, glm_rad(cos(glfwGetTime() * (0.65f * (i / 10.0f))) * 125.0f), (vec3){0.0f, 0.0f, 1.0f});
		wsShaderSetMVP(cube_shader, matrix_model, matrix_view, matrix_perspective);
		wsShaderSetNormalMatrix(cube_shader, matrix_model);
		wsShaderUpdateLights(cube_shader, WS_MAX_POINTLIGHTS, WS_MAX_SPOTLIGHTS, WS_MAX_DIRECTIONLIGHTS);
		
		glBindVertexArray(cube_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	
	// Draw pointlight cubes.
	static bool pointlight_toggle = true;
	for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++) {
		glm_mat4_identity(*matrix_model);
		glm_translate(*matrix_model, pointlights.position[i]);
		glm_scale(*matrix_model, (vec3){0.2f, 0.2f, 0.2f});
		
		wsShaderUse(light_shader);
		wsShaderSetVec3(light_shader, "light_color", pointlights.color[i]);
		wsShaderSetInt(light_shader, "toggle", (int)pointlights.toggle[i]);
		wsShaderSetMVP(light_shader, matrix_model, matrix_view, matrix_perspective);
		
		if(pointlight_toggle) {
			float light_toggle = fabs(sin(glfwGetTime() + (i * 0.775f)));
			wsLightTogglep(i, light_toggle > 0.975f ? true : false);
			// wsLightSetColorp(i, (vec3){fabs(sin(glfwGetTime())), fabs(sin(glfwGetTime() * i * 2)), fabs(sin(glfwGetTime() * i * 4))});
		}
		
		glBindVertexArray(light_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	
	vec3 firefly_positions[WS_MAX_POINTLIGHTS];
	glm_vec3_add(pointlight_positions[0], (vec3){sin(glfwGetTime()), cos(glfwGetTime()), sin(glfwGetTime())}, firefly_positions[0]);
	glm_vec3_add(pointlight_positions[1], (vec3){cos(glfwGetTime()), sin(glfwGetTime()), cos(glfwGetTime())}, firefly_positions[1]);
	glm_vec3_add(pointlight_positions[2], (vec3){cos(glfwGetTime()), sin(glfwGetTime()), sin(glfwGetTime())}, firefly_positions[2]);
	glm_vec3_add(pointlight_positions[3], (vec3){cos(glfwGetTime()), cos(glfwGetTime()), sin(glfwGetTime())}, firefly_positions[3]);
	wsLightSetPositionp(0, firefly_positions[0]);
	wsLightSetPositionp(1, firefly_positions[1]);
	wsLightSetPositionp(2, firefly_positions[2]);
	wsLightSetPositionp(3, firefly_positions[3]);
	wsLightSetPositionf(0, cameras.position[camera_active]);
	wsLightSetRotationf(0, cameras.rotation[camera_active]);
	
	if(wsInputGetPressOnce(GLFW_KEY_G)) {
		pointlight_toggle = !pointlight_toggle;
		for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++)
			wsLightTogglep(i, pointlight_toggle);
		if(wsLightGetTogglep(0))
			printf("Turning on pointlights...\n");
		else printf("Turning off pointlights...\n");
	}
	
	static bool flashlight_toggle = false;
	if(wsInputGetPressOnce(GLFW_KEY_F)) {
		flashlight_toggle = !flashlight_toggle;
		wsLightTogglef(0, flashlight_toggle);
		if(wsLightGetTogglef(0))
			printf("Turning on spotlights...\n");
		else printf("Turning off spotlights...\n");
	}
	
	static bool directionlight_toggle = false;
	if(wsInputGetPressOnce(GLFW_KEY_H)) {
		directionlight_toggle = !directionlight_toggle;
		wsLightToggled(0, directionlight_toggle);
		if(wsLightGetToggled(0))
			printf("Turning on directionlights...\n");
		else printf("Turning off directionlights...\n");
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
	wsTextRender(text_shader, "The quick brown fox jumps over the lazy dog!", (vec2){window_width - 1010.0f, window_height - 50.0f}, 0.35f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	
	wsTextRender(text_shader, "[W][A][S][D] for z/x-axis", (vec2){10.0f, 1050.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[CTRL][SPACE] for y-axis", (vec2){10.0f, 1025.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[F] to toggle spot lights", (vec2){10.0f, 1000.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[G] to toggle point lights", (vec2){10.0f, 975.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "[H] to toggle directional lights", (vec2){10.0f, 950.0f}, 0.2f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "kade samson", (vec2){10.0f, 20.0f}, 0.1f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "version 66642069", (vec2){10.0f, 10.0f}, 0.1f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
}

void wsGraphicsTerminate() {
	wsModelTerminate(&cube_model);
	wsTextTerminate();
	wsDeferredRendererTerminate();
	
	glDeleteVertexArrays(1, &cube_vao);
	glDeleteVertexArrays(1, &light_vao);
	
	glDeleteBuffers(1, &vbo);
	
	glDeleteProgram(cube_shader);
	glDeleteProgram(light_shader);
	
	printf("Graphics terminated\n");
}