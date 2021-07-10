#include"camera.h"
#include"input.h"

unsigned int wsCameraInit(vec3 position, vec3 rotation, float fov);
void wsCameraSyncRotation(unsigned int cameraID);
void wsCameraGenViewMatrix(unsigned int cameraID, mat4 *view_dest);
void wsCameraMove(unsigned int cameraID, vec3 move_array, float speed);
void wsCameraMouseMove(unsigned int cameraID, float offsetx, float offsety, float pitch_constraint);
void wsCameraMouseMoveDamp(unsigned int cameraID, float offsetx, float offsety, float pitch_constraint, float damp);

unsigned int wsCameraInit(vec3 position, vec3 rotation, float fov) {
	static unsigned int num_cameras = 0;
	
	for(unsigned int i = 0; i < 3; i++) {
		cameras.position[num_cameras][i]		= position[i];
		cameras.euler[num_cameras][i]			= rotation[i];
		cameras.euler_interp[i]				= rotation[i];
	}
	wsCameraSyncRotation(num_cameras);
	glm_cross(cameras.rotation[num_cameras], cameras.up[num_cameras], cameras.right[num_cameras]);
	cameras.fov[num_cameras] = fov;
	
	printf("Camera %d: position: %.2f %.2f %.2f | forward: %.2f %.2f %.2f | field of view: %.2f\n", num_cameras, 
		cameras.position[num_cameras][0], cameras.position[num_cameras][1], cameras.position[num_cameras][2], 
		cameras.rotation[num_cameras][0], cameras.rotation[num_cameras][1], cameras.rotation[num_cameras][2], 
		cameras.fov[num_cameras]);
	
	return num_cameras++;
}

unsigned int wsCameraGetActive() { return cameras.active; }
void wsCameraSetActive(unsigned int cameraID) { cameras.active = cameraID; }

void wsCameraGenViewMatrix(unsigned int cameraID, mat4 *view_dest) {
	vec3 lookat;
	glm_vec3_add(cameras.position[cameraID], cameras.rotation[cameraID], lookat);
	glm_lookat(cameras.position[cameraID], lookat, cameras.up[cameraID], *view_dest);
}

// Update camera forward, up, and right vectors according to euler vector.  Only used internally.
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
// Update camera forward, up, and right vectors according to euler vector.  Only used internally.
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
	
	cameras.euler_interp[YAW] += wsInterpf(cameras.euler_interp[YAW], cameras.euler[cameraID][YAW], damp);
	cameras.euler_interp[PITCH] += wsInterpf(cameras.euler_interp[PITCH], cameras.euler[cameraID][PITCH], damp);
	// cameras.euler_interp[ROLL] = (cameras.euler[cameraID][YAW] - cameras.euler_interp[YAW]) * 0.85f;
	
	if(cameras.euler[cameraID][PITCH] > pitch_constraint)
		cameras.euler[cameraID][PITCH] = pitch_constraint;
	else if(cameras.euler[cameraID][PITCH] < -pitch_constraint)
		cameras.euler[cameraID][PITCH] = -pitch_constraint;
	
	wsCameraSyncRotationDamped(cameraID);
}

void wsCameraMove(unsigned int cameraID, vec3 move_array, float speed) {
	vec3 move_vector = {0};
	vec3 move_vector_temp = {0};
	
	speed *= delta_time;
	
	// Forward and backward
	glm_vec3_copy(cameras.rotation[cameraID], move_vector_temp);
	move_vector_temp[UP] = 0.0f;
	glm_normalize(move_vector_temp);
	glm_vec3_scale(move_vector_temp, speed * move_array[FORWARD], move_vector_temp);
	glm_vec3_add(move_vector_temp, move_vector, move_vector);
	
	// Left and right
	glm_vec3_copy(cameras.right[cameraID], move_vector_temp);
	move_vector_temp[UP] = 0.0f;
	glm_normalize(move_vector_temp);
	glm_vec3_scale(move_vector_temp, speed * move_array[RIGHT], move_vector_temp);
	glm_vec3_add(move_vector_temp, move_vector, move_vector);
	
	// Head bob.
	static float head_bob = 0.0f;
	float head_bob_target = ((sin(glfwGetTime() * 20) + 1) * 0.025f);
	head_bob_target *= (move_array[FORWARD] || move_array[RIGHT]);		// Don't bob if not moving.
	head_bob_target += fmin(0.0f, move_array[UP]) * 0.5f;				// Crouching.
	head_bob += (head_bob_target - head_bob) * 0.35f;					// Interpolate head bob.
	
	// FPS camera up and down.
	static float yv = 0.0f;
	float jump_height = 0.15f, gravity = 0.65f;
	if(cameras.position[cameraID][UP] < head_bob) {
		if(move_array[UP] > 0)
			yv = jump_height;
		else cameras.position[cameraID][UP] = head_bob;
	} else yv -= gravity * delta_time;
	cameras.position[cameraID][UP] += yv;
	
	// The shit that makes you go places.
	glm_vec3_add(cameras.position[cameraID], move_vector, cameras.position[cameraID]);
}
void wsCameraFly(unsigned int cameraID, vec3 move_array, float speed) {
	vec3 move_vector = {0};
	vec3 move_vector_temp = {0};
	
	speed *= delta_time;
	
	// Forward and backward
	glm_vec3_copy(cameras.rotation[cameraID], move_vector_temp);
	move_vector_temp[UP] = 0.0f;
	glm_normalize(move_vector_temp);
	glm_vec3_scale(move_vector_temp, speed * move_array[FORWARD], move_vector_temp);
	glm_vec3_add(move_vector_temp, move_vector, move_vector);
	
	// Left and right
	glm_vec3_copy(cameras.right[cameraID], move_vector_temp);
	move_vector_temp[UP] = 0.0f;
	glm_normalize(move_vector_temp);
	glm_vec3_scale(move_vector_temp, speed * move_array[RIGHT], move_vector_temp);
	glm_vec3_add(move_vector_temp, move_vector, move_vector);
	
	// Fly camera up and down.
	glm_vec3_scale(cameras.up[cameraID], speed * move_array[UP], move_vector_temp);
	glm_vec3_add(move_vector_temp, move_vector, move_vector);
	
	// The shit that makes you go places.
	glm_vec3_add(cameras.position[cameraID], move_vector, cameras.position[cameraID]);
}

void wsCameraMakeFPS(unsigned int cameraID, float speed, float pitch_constraint) {
	vec3 move_array = {0};
	static bool is_fly_camera = false;
	
	if(wsInputGetHold(GLFW_KEY_W) || wsInputGetHold(GLFW_KEY_UP))
		move_array[FORWARD]++;
	if(wsInputGetHold(GLFW_KEY_S) || wsInputGetHold(GLFW_KEY_DOWN))
		move_array[FORWARD]--;
	if(wsInputGetHold(GLFW_KEY_A) || wsInputGetHold(GLFW_KEY_LEFT))
		move_array[RIGHT]--;
	if(wsInputGetHold(GLFW_KEY_D) || wsInputGetHold(GLFW_KEY_RIGHT))
		move_array[RIGHT]++;
	if(wsInputGetHold(GLFW_KEY_SPACE))
		move_array[UP]++;
	if(wsInputGetHold(GLFW_KEY_LEFT_CONTROL)) {
		move_array[UP]--;
		move_array[FORWARD] *= 0.5f;
		move_array[RIGHT] *= 0.5f;
	}
	
	wsCameraMove(cameraID, move_array, speed);
	wsCameraMouseMoveDamp(cameraID, wsInputGetMouseMoveX() / 3.0, wsInputGetMouseMoveY() / 3.0, pitch_constraint, 25.0f);
	// wsCameraGenViewMatrix(cameraID, view_dest);
}
void wsCameraMakeFly(unsigned int cameraID, float speed, float pitch_constraint) {
	vec3 move_array = {0};
	static bool is_fly_camera = false;
	
	if(wsInputGetHold(GLFW_KEY_W) || wsInputGetHold(GLFW_KEY_UP))
		move_array[FORWARD]++;
	if(wsInputGetHold(GLFW_KEY_S) || wsInputGetHold(GLFW_KEY_DOWN))
		move_array[FORWARD]--;
	if(wsInputGetHold(GLFW_KEY_A) || wsInputGetHold(GLFW_KEY_LEFT))
		move_array[RIGHT]--;
	if(wsInputGetHold(GLFW_KEY_D) || wsInputGetHold(GLFW_KEY_RIGHT))
		move_array[RIGHT]++;
	if(wsInputGetHold(GLFW_KEY_SPACE))
		move_array[UP]++;
	if(wsInputGetHold(GLFW_KEY_LEFT_CONTROL))
		move_array[UP]--;
	
	wsCameraFly(cameraID, move_array, speed);
	wsCameraMouseMoveDamp(cameraID, wsInputGetMouseMoveX() / 3.0, wsInputGetMouseMoveY() / 3.0, pitch_constraint, 25.0f);
	// wsCameraGenViewMatrix(cameraID, view_dest);
}