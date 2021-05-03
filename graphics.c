#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#include"graphics.h"
#include"shader.h"
#include"model.h"
#include"material.h"
#include"light.h"
#include"text.h"

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

unsigned int num_pointlights;
wsPointLight pointlights[4];
wsDirectionLight directionlight;
wsSpotLight spotlight;

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

unsigned short wsGraphicsInit();
void wsGraphicsInitLighting();
void wsGraphicsInitTestCube();

void wsGraphicsRender();
void wsGraphicsTerminate();

void wsGraphicsLoadTexture(const char *path, unsigned int *dest_texture, unsigned int gl_texture_unit, unsigned int wrap_style, unsigned int filter_style);
// void wsGraphicsFPSCamera(unsigned int ndx, mat4 *view_dest, float speed, float pitch_constraint);

unsigned short wsGraphicsInit(GLFWwindow *window) {
	stbi_set_flip_vertically_on_load(true);
	
	glClearColor(0.025f, 0.025f, 0.025f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_FRAMEBUFFER_SRGB);
	
	text_shader = wsShaderLoad("shaders/text.vert", "shaders/text.frag", false, false);
	text_billboard_shader = wsShaderLoad("shaders/text_billboard.vert", "shaders/text_billboard.frag", false, false);
	if(!wsTextInit())
		return WS_ERROR_FREETYPE;
	wsTextLoadFace("fonts/Roboto_Mono/static/RobotoMono-Medium.ttf", 108, next_avaliable_texture_unit++);
	// wsTextLoadFace("fonts/Roboto_Mono/static/RobotoMono-ThinItalic.ttf", 108, next_avaliable_texture_unit++);
	
	light_shader = wsShaderLoad("shaders/lightsource.vert", "shaders/lightsource.frag", false, false);
	wsGraphicsInitLighting();
	
	cube_shader = wsShaderLoad("shaders/textured_phong_dithered.vert", "shaders/textured_phong_dithered.frag", false, false);
	wsMaterialInit(&cube_material, (vec3){1.0f, 0.5f, 0.31f}, (vec3){1.0f, 0.5f, 0.31f}, (vec3){0.5f, 0.5f, 0.5f}, 8.0f);
	wsModelInit(&cube_model);
	wsGraphicsInitTestCube();
	
	camera_active = WS_CAMERA0;
	wsCameraInit(camera_active, (vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, -90.0f, 0.0f}, 90.0f);
	
	printf("Graphics initialized\n");
	return WS_OKAY;
}

void wsGraphicsInitLighting() {
	vec3 pointlight_positions[] = {
		{ 0.7f,  0.2f,  2.0f},
		{ 2.3f, -3.3f, -4.0f},
		{-4.0f,  2.0f, -12.0f},
		{ 0.0f,  0.0f, -3.0f}
	};
	
	num_pointlights = sizeof(pointlights) / sizeof(pointlights[0]);
	printf("%d point lights in scene\n", num_pointlights);
	for(int i = 0; i < num_pointlights; i++) {
		wsLightQuickInitp(&pointlights[i], pointlight_positions[i], (vec3){1.0f, 0.8f, 0.0f}, 0.45f);
	}
	
	wsLightQuickInitf(&spotlight, cameras.position[camera_active], cameras.rotation[camera_active], (vec3){1.0f, 0.0f, 1.0f}, 0.3f, 12.5f);
	wsLightQuickInitd(&directionlight, (vec3){-0.2f, -1.0f, -0.3f}, (vec3){1.0f, 1.0f, 0.8f}, 1.0f);
}

void wsGraphicsInitTestCube() {
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
		
		wsShaderUpdateLightsp(cube_shader, &pointlights[0], num_pointlights);
		wsShaderUpdateLightf(cube_shader, &spotlight);
		wsShaderUpdateLightd(cube_shader, &directionlight);
		
		glBindVertexArray(cube_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	
	for(unsigned int i = 0; i < num_pointlights; i++) {
		glm_mat4_identity(*matrix_model);
		glm_translate(*matrix_model, pointlights[i].position);
		glm_scale(*matrix_model, (vec3){0.2f, 0.2f, 0.2f});
		
		wsShaderUse(light_shader);
		wsShaderSetVec3(light_shader, "light_color", (&pointlights[i])->color);
		wsShaderSetMVP(light_shader, matrix_model, matrix_view, matrix_perspective);
		
		wsLightSetColorp(&pointlights[i], (vec3){fabs(sin(glfwGetTime())), fabs(sin(glfwGetTime() * i * 2)), fabs(sin(glfwGetTime() * i * 4))});
		
		glBindVertexArray(light_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	wsLightSetPositionp(&pointlights[0], (vec3){sin(glfwGetTime()), cos(glfwGetTime()), sin(glfwGetTime())});
	wsLightSetPositionp(&pointlights[1], (vec3){cos(glfwGetTime()), sin(glfwGetTime()), cos(glfwGetTime())});
	wsLightSetPositionp(&pointlights[2], (vec3){cos(glfwGetTime()), sin(glfwGetTime()), sin(glfwGetTime())});
	wsLightSetPositionp(&pointlights[3], (vec3){cos(glfwGetTime()), cos(glfwGetTime()), sin(glfwGetTime())});
	wsLightSetPositionf(&spotlight, cameras.position[camera_active]);
	wsLightSetRotationf(&spotlight, cameras.rotation[camera_active]);
}

// Render UI.
void wsGraphicsUIRender(mat4 *matrix_model, mat4 *matrix_view, mat4 *matrix_perspective, mat4 *matrix_ortho) {
	// Cube labels.
	for(unsigned int i = 0; i < 10; i++) {
		vec3 font_color = {fabs(sin(i * 1.23f)) + 0.25f, fabs(cos(i * 2.73f)) + 0.25f, fabs(sin(i * 0.5f)) + 0.25f};
		char cube_name[2];
		vec3 cube_label_position = {cube_positions[i][0], cube_positions[i][1] + 1.0f, cube_positions[i][2]};
		sprintf(cube_name, "%d", i);
		wsTextBillboardRender(text_billboard_shader, &cube_name[0], cube_label_position, 0.005f, font_color, camera_active, matrix_view, matrix_perspective);
	}
	
	// UI labels.
	wsTextRender(text_shader, "The quick brown fox jumps over the lazy dog!", (vec2){window_width - 1010.0f, window_height - 50.0f}, 0.35f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "kade samson", (vec2){10.0f, 20.0f}, 0.1f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
	wsTextRender(text_shader, "version 66642069", (vec2){10.0f, 10.0f}, 0.1f, (vec3){1.0f, 1.0f, 1.0f}, matrix_ortho);
}

void wsGraphicsTerminate() {
	wsModelTerminate(&cube_model);
	wsTextTerminate();
	
	glDeleteVertexArrays(1, &cube_vao);
	glDeleteVertexArrays(1, &light_vao);
	
	glDeleteBuffers(1, &vbo);
	
	glDeleteProgram(cube_shader);
	glDeleteProgram(light_shader);
	
	printf("Graphics terminated\n");
}