#include"camera.h"
#include"input.h"

vec3 camera_euler_interp;

void wsCameraInit(wsCamera *camera, vec3 position, vec3 rotation, float fov);
void wsCameraSyncRotation(wsCamera *camera);
void wsCameraGenViewMatrix(wsCamera *camera, mat4 *view_dest);
void wsCameraFPSMove(wsCamera *camera, vec3 move_array, float speed);
void wsCameraFPSMove(wsCamera *camera, vec3 move_array, float speed);
void wsCameraMouseMove(wsCamera *camera, float offsetx, float offsety, float pitch_constraint);
void wsCameraMouseMoveDamp(wsCamera *camera, float offsetx, float offsety, float pitch_constraint, float damp);

void wsCameraInit(wsCamera *camera, vec3 position, vec3 rotation, float fov) {
	for(int i = 0; i < 3; i++) {
		camera->position[i]		= position[i];
		camera->euler[i]		= rotation[i];
		camera_euler_interp[i]	= rotation[i];
	}
	wsCameraSyncRotation(camera);
	glm_cross(camera->rotation, camera->up, camera->right);
	camera->fov = fov;
	
	printf("Camera @ %p: position: %.2f %.2f %.2f | forward: %.2f %.2f %.2f | field of view: %.2f\n", camera, 
		camera->position[0], camera->position[1], camera->position[2], 
		camera->rotation[0], camera->rotation[1], camera->rotation[2], 
		camera->fov);
}

void wsCameraSyncRotation(wsCamera *camera) {
	camera->rotation[0]	= cos(glm_rad(camera->euler[YAW])) * cos(glm_rad(camera->euler[PITCH]));
	camera->rotation[1]	= sin(glm_rad(camera->euler[PITCH]));
	camera->rotation[2]	= sin(glm_rad(camera->euler[YAW])) * cos(glm_rad(camera->euler[PITCH]));
	glm_normalize(camera->rotation);
	
	// glm_vec3_copy() gives a warning and warnings stress me out.
	camera->up[0] = WORLD_UP[0];
	camera->up[1] = WORLD_UP[1];
	camera->up[2] = WORLD_UP[2];
	glm_normalize(camera->up);
	
	glm_vec3_cross(camera->rotation, camera->up, camera->right);
	glm_normalize(camera->right);
}

void wsCameraSyncRotationDamped(wsCamera *camera) {
	camera->rotation[0]	= cos(glm_rad(camera_euler_interp[YAW])) * cos(glm_rad(camera_euler_interp[PITCH]));
	camera->rotation[1]	= sin(glm_rad(camera_euler_interp[PITCH]));
	camera->rotation[2]	= sin(glm_rad(camera_euler_interp[YAW])) * cos(glm_rad(camera_euler_interp[PITCH]));
	glm_normalize(camera->rotation);
	
	// glm_vec3_copy() gives a warning and warnings stress me out.
	camera->up[0] = WORLD_UP[0];
	camera->up[1] = WORLD_UP[1];
	camera->up[2] = WORLD_UP[2];
	glm_normalize(camera->up);
	
	glm_vec3_cross(camera->rotation, camera->up, camera->right);
	glm_normalize(camera->right);
}

void wsCameraGenViewMatrix(wsCamera *camera, mat4 *view_dest) {
	vec3 camera_lookat;
	glm_vec3_add(camera->position, camera->rotation, camera_lookat);
	glm_lookat(camera->position, camera_lookat, camera->up, *view_dest);
}

void wsCameraMouseMove(wsCamera *camera, float offsetx, float offsety, float pitch_constraint) {
	camera->euler[YAW] += offsetx;
	camera->euler[PITCH] += offsety;
	
	if(camera->euler[PITCH] > pitch_constraint)
		camera->euler[PITCH] = pitch_constraint;
	else if(camera->euler[PITCH] < -pitch_constraint)
		camera->euler[PITCH] = -pitch_constraint;
	
	wsCameraSyncRotation(camera);
}

void wsCameraMouseMoveDamp(wsCamera *camera, float offsetx, float offsety, float pitch_constraint, float damp) {
	damp *= delta_time;
	
	camera->euler[YAW] += offsetx;
	camera->euler[PITCH] += offsety;
	
	camera_euler_interp[YAW] += wsInterpF(camera_euler_interp[YAW], camera->euler[YAW], damp);
	camera_euler_interp[PITCH] += wsInterpF(camera_euler_interp[PITCH], camera->euler[PITCH], damp);
	
	if(camera->euler[PITCH] > pitch_constraint)
		camera->euler[PITCH] = pitch_constraint;
	else if(camera->euler[PITCH] < -pitch_constraint)
		camera->euler[PITCH] = -pitch_constraint;
	
	wsCameraSyncRotationDamped(camera);
}

void wsCameraFPSMove(wsCamera *camera, vec3 move_array, float speed) {
	vec3 move_vector = {0};
	vec3 move_vector_temp = {0};
	
	speed *= delta_time;
	
	// Forward and backward
	glm_vec3_copy(camera->rotation, move_vector_temp);
	move_vector_temp[1] = 0.0f;
	glm_normalize(move_vector_temp);
	glm_vec3_scale(move_vector_temp, speed * move_array[0], move_vector_temp);
	glm_vec3_add(move_vector_temp, move_vector, move_vector);
	
	// Left and right
	glm_vec3_copy(camera->right, move_vector_temp);
	move_vector_temp[1] = 0.0f;
	glm_normalize(move_vector_temp);
	glm_vec3_scale(move_vector_temp, speed * move_array[1], move_vector_temp);
	glm_vec3_add(move_vector_temp, move_vector, move_vector);
	
	// Up and down
	glm_vec3_scale(camera->up, speed * move_array[2], move_vector_temp);
	glm_vec3_add(move_vector_temp, move_vector, move_vector);
	
	// The shit that makes you go places.
	glm_vec3_add(camera->position, move_vector, camera->position);
}

void wsCameraMakeFPS(wsCamera *camera, mat4 *view_dest, float speed, float pitch_constraint) {
	vec3 move_array = {0};
	
	if(wsInputGetPress(GLFW_KEY_W) || wsInputGetPress(GLFW_KEY_UP))
		move_array[FORWARD]++;
	if(wsInputGetPress(GLFW_KEY_S) || wsInputGetPress(GLFW_KEY_DOWN))
		move_array[FORWARD]--;
	if(wsInputGetPress(GLFW_KEY_A) || wsInputGetPress(GLFW_KEY_LEFT))
		move_array[RIGHT]--;
	if(wsInputGetPress(GLFW_KEY_D) || wsInputGetPress(GLFW_KEY_RIGHT))
		move_array[RIGHT]++;
	if(wsInputGetPress(GLFW_KEY_SPACE))
		move_array[UP]++;
	if(wsInputGetPress(GLFW_KEY_LEFT_CONTROL))
		move_array[UP]--;
	if(wsInputGetPress(GLFW_KEY_LEFT_SHIFT))
		glm_vec3_scale(move_array, 10.0f, move_array);
	
	wsCameraFPSMove(camera, move_array, speed);
	wsCameraMouseMove(camera, wsInputGetMouseMoveX() / 3.0, wsInputGetMouseMoveY() / 3.0, pitch_constraint);
}