#include"camera.h"
#include"input.h"

void wsCameraInit(unsigned int cameraID, vec3 position, vec3 rotation, float fov);
void wsCameraSyncRotation(unsigned int cameraID);
void wsCameraGenViewMatrix(unsigned int cameraID, mat4 *view_dest);
void wsCameraFPSMove(unsigned int cameraID, vec3 move_array, float speed);
void wsCameraFPSMove(unsigned int cameraID, vec3 move_array, float speed);
void wsCameraMouseMove(unsigned int cameraID, float offsetx, float offsety, float pitch_constraint);
void wsCameraMouseMoveDamp(unsigned int cameraID, float offsetx, float offsety, float pitch_constraint, float damp);

void wsCameraInit(unsigned int cameraID, vec3 position, vec3 rotation, float fov) {
	for(unsigned int i = 0; i < 3; i++) {
		cameras.position[cameraID][i]		= position[i];
		cameras.euler[cameraID][i]			= rotation[i];
		cameras.euler_interp[i]				= rotation[i];
	}
	wsCameraSyncRotation(cameraID);
	glm_cross(cameras.rotation[cameraID], cameras.up[cameraID], cameras.right[cameraID]);
	cameras.fov[cameraID] = fov;
	
	printf("Camera %d: position: %.2f %.2f %.2f | forward: %.2f %.2f %.2f | field of view: %.2f\n", cameraID, 
		cameras.position[cameraID][0], cameras.position[cameraID][1], cameras.position[cameraID][2], 
		cameras.rotation[cameraID][0], cameras.rotation[cameraID][1], cameras.rotation[cameraID][2], 
		cameras.fov[cameraID]);
}

// Update camera forward, up, and right vectors.  Only used internally.
void wsCameraSyncRotation(unsigned int cameraID) {
	cameras.rotation[cameraID][0]	= cos(glm_rad(cameras.euler[cameraID][YAW])) * cos(glm_rad(cameras.euler[cameraID][PITCH]));
	cameras.rotation[cameraID][1]	= sin(glm_rad(cameras.euler[cameraID][PITCH]));
	cameras.rotation[cameraID][2]	= sin(glm_rad(cameras.euler[cameraID][YAW])) * cos(glm_rad(cameras.euler[cameraID][PITCH]));
	glm_normalize(cameras.rotation[cameraID]);
	
	// glm_vec3_copy() gives a warning and warnings stress me out.
	cameras.up[cameraID][0] = WS_WORLD_UP[0];
	cameras.up[cameraID][1] = WS_WORLD_UP[1];
	cameras.up[cameraID][2] = WS_WORLD_UP[2];
	glm_normalize(cameras.up[cameraID]);
	
	glm_vec3_cross(cameras.rotation[cameraID], cameras.up[cameraID], cameras.right[cameraID]);
	glm_normalize(cameras.right[cameraID]);
}

void wsCameraSyncRotationDamped(unsigned int cameraID) {
	cameras.rotation[cameraID][0]	= cos(glm_rad(cameras.euler_interp[YAW])) * cos(glm_rad(cameras.euler_interp[PITCH]));
	cameras.rotation[cameraID][1]	= sin(glm_rad(cameras.euler_interp[PITCH]));
	cameras.rotation[cameraID][2]	= sin(glm_rad(cameras.euler_interp[YAW])) * cos(glm_rad(cameras.euler_interp[PITCH]));
	glm_normalize(cameras.rotation[cameraID]);
	
	// glm_vec3_copy() gives a warning and warnings stress me out.
	cameras.up[cameraID][0] = WS_WORLD_UP[0];
	cameras.up[cameraID][1] = WS_WORLD_UP[1];
	cameras.up[cameraID][2] = WS_WORLD_UP[2];
	glm_normalize(cameras.up[cameraID]);
	
	glm_vec3_cross(cameras.rotation[cameraID], cameras.up[cameraID], cameras.right[cameraID]);
	glm_normalize(cameras.right[cameraID]);
}

void wsCameraGenViewMatrix(unsigned int cameraID, mat4 *view_dest) {
	vec3 lookat;
	glm_vec3_add(cameras.position[cameraID], cameras.rotation[cameraID], lookat);
	glm_lookat(cameras.position[cameraID], lookat, cameras.up[cameraID], *view_dest);
}

void wsCameraMouseMove(unsigned int cameraID, float offsetx, float offsety, float pitch_constraint) {
	cameras.euler[cameraID][YAW] += offsetx;
	cameras.euler[cameraID][PITCH] += offsety;
	
	if(cameras.euler[cameraID][PITCH] > pitch_constraint)
		cameras.euler[cameraID][PITCH] = pitch_constraint;
	else if(cameras.euler[cameraID][PITCH] < -pitch_constraint)
		cameras.euler[cameraID][PITCH] = -pitch_constraint;
	
	wsCameraSyncRotation(cameraID);
}

void wsCameraMouseMoveDamp(unsigned int cameraID, float offsetx, float offsety, float pitch_constraint, float damp) {
	damp *= delta_time;
	
	cameras.euler[cameraID][YAW] += offsetx;
	cameras.euler[cameraID][PITCH] += offsety;
	
	cameras.euler_interp[YAW] += wsInterpF(cameras.euler_interp[YAW], cameras.euler[cameraID][YAW], damp);
	cameras.euler_interp[PITCH] += wsInterpF(cameras.euler_interp[PITCH], cameras.euler[cameraID][PITCH], damp);
	
	if(cameras.euler[cameraID][PITCH] > pitch_constraint)
		cameras.euler[cameraID][PITCH] = pitch_constraint;
	else if(cameras.euler[cameraID][PITCH] < -pitch_constraint)
		cameras.euler[cameraID][PITCH] = -pitch_constraint;
	
	wsCameraSyncRotationDamped(cameraID);
}

void wsCameraFPSMove(unsigned int cameraID, vec3 move_array, float speed) {
	vec3 move_vector = {0};
	vec3 move_vector_temp = {0};
	
	speed *= delta_time;
	
	// Forward and backward
	glm_vec3_copy(cameras.rotation[cameraID], move_vector_temp);
	move_vector_temp[1] = 0.0f;
	glm_normalize(move_vector_temp);
	glm_vec3_scale(move_vector_temp, speed * move_array[0], move_vector_temp);
	glm_vec3_add(move_vector_temp, move_vector, move_vector);
	
	// Left and right
	glm_vec3_copy(cameras.right[cameraID], move_vector_temp);
	move_vector_temp[1] = 0.0f;
	glm_normalize(move_vector_temp);
	glm_vec3_scale(move_vector_temp, speed * move_array[1], move_vector_temp);
	glm_vec3_add(move_vector_temp, move_vector, move_vector);
	
	// Up and down
	glm_vec3_scale(cameras.up[cameraID], speed * move_array[2], move_vector_temp);
	glm_vec3_add(move_vector_temp, move_vector, move_vector);
	
	// The shit that makes you go places.
	glm_vec3_add(cameras.position[cameraID], move_vector, cameras.position[cameraID]);
}

void wsCameraMakeFPS(unsigned int cameraID, mat4 *view_dest, float speed, float pitch_constraint) {
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
	
	wsCameraFPSMove(cameraID, move_array, speed);
	wsCameraMouseMove(cameraID, wsInputGetMouseMoveX() / 3.0, wsInputGetMouseMoveY() / 3.0, pitch_constraint);
}