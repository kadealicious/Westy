#include<stdio.h>
#include<windows.h>
#include<stdbool.h>

#include"globals.h"
#include"graphics.h"
#include"deferredrenderer.h"
#include"input.h"

// Declare globals from globals.h
const vec3 WS_WORLD_UP		= {0.0f, 1.0f, 0.0f};
const vec3 WS_WORLD_RIGHT	= {1.0f, 0.0f, 0.0f};
const vec3 WS_WORLD_FORWARD	= {0.0f, 0.0f, 1.0f};

int monitor_width;
int monitor_height;
int win_posx;
int win_posy;
int window_width	= 640;
int window_height	= 480;
int screen_width	= 640;
int screen_height	= 480;
unsigned int target_fps = 60;
float delta_time = 1.0f;

// "Local" variables.
FILE *console_log;

GLFWwindow *window;
GLFWmonitor *monitor_primary;
GLFWcursor *cursor;

bool wsInitGLFW();
bool wsInitGLEW();
int wsInit();
int wsRun();
int wsQuit(unsigned int app_state);
void wsCenterWindow(GLFWwindow *window);

void wsMonitorCallback(GLFWmonitor *monitor, int ev);
void wsFrameBufferSizeCallback(GLFWwindow *window, int width, int height);

int main(int argc, char **argv) {
	// console_log = freopen("consolelog.txt", "w", stdout);
	
	printf(" --- BEGIN --- \n\n");
	
	// OG code
	unsigned int state = wsInit();
	if(state == WS_OKAY)
		state = wsRun();
	state = wsQuit(state);
	
	printf("\n --- END --- \n");
	// fclose(console_log);
	return state;
}

int wsInit() {
	printf("~INIT~\n");
	if(!wsInitGLFW()) return WS_ERROR_GLFW;
	if(!wsInitGLEW()) return WS_ERROR_GLEW;
	
	int state = wsGraphicsInit(window);
	if(state != WS_OKAY) return state;
	
	wsInputInit(window, 0.3f);
	
	printf("~INIT~\n\n");
	return WS_OKAY;
}

bool wsInitGLEW() {
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		fprintf(stderr, "ERROR - GLEW init fail\n");
		return false;
	} else printf("GLEW init success\n");
	return true;
}
bool wsInitGLFW() {
	if(!glfwInit()) {
		fprintf(stderr, "ERROR - GLFW init fail\n");
		return false;
	}
	
	// All GLFW initialization shtuff.
	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifndef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	
	// Less important, still GLFW.
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	
	// int monitor_count;
	// GLFWmonitor **monitors		= glfwGetMonitors(&monitor_count);
	monitor_primary				= glfwGetPrimaryMonitor();
	glfwSetMonitorCallback(wsMonitorCallback);
	
	// int video_mode_count;
	// const GLFWvidmode *video_modes	= glfwGetVideoModes(monitor_primary, &video_mode_count);
	const GLFWvidmode *video_mode	= glfwGetVideoMode(monitor_primary);
	window_width	= video_mode->width / 2;
	window_height	= video_mode->height / 2;
	screen_width = window_width;
	screen_height = window_height;
	
	float content_scale_x;
	float content_scale_y;
	glfwGetMonitorContentScale(monitor_primary, &content_scale_x, &content_scale_y);
	
	printf("%u fps, %.4f frametime\n", target_fps, (1.0f / target_fps));
	printf("Video mode: %d x %d\n", video_mode->width, video_mode->height);
	printf("Window dimensions: %d x %d\n", window_width, window_height);
	printf("Monitor content scale: %f x %f\n", content_scale_x, content_scale_y);
	
	window = glfwCreateWindow(window_width, window_height, "Deferred rendering?  More like deferred findings.", NULL, NULL);
	if(window == NULL) {
		fprintf(stderr, "ERROR - Failed to open GLFW window\n");
		return false;
	}
	monitor_width = video_mode->width;
	monitor_height = video_mode->height;
	wsCenterWindow(window);
	glfwSetWindowPos(window, win_posx, win_posy);
	
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if(glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	} else printf("WARNING - Raw mouse motion not supported.  Sorry!\n");
	
	glfwSetFramebufferSizeCallback(window, wsFrameBufferSizeCallback);
	glfwSetKeyCallback(window, wsInputKeyCallback);
	glfwSetCursorPosCallback(window, wsInputCursorPosCallback);
	glfwSetScrollCallback(window, wsInputScrollCallback);
	
	printf("GLFW init success\n");
	return true;
}

int wsRun() {
	printf("~RUN~\n");
	
	double now_time;
	double last_time = glfwGetTime();
	
	unsigned int state = WS_OKAY;
	while(state == WS_OKAY) {
		now_time = glfwGetTime();
		delta_time = now_time - last_time;
		last_time = now_time;
		
		// The meat and potatoes!
		wsGraphicsRender();
		wsInputUpdate();
		
		glfwPollEvents();
		glfwSwapBuffers(window);
		
		if(wsInputGetPressOnce(GLFW_KEY_F11)) {
			if(glfwGetWindowMonitor(window) == NULL) {
				printf("Switching to fullscreen mode...\n");
				glfwSetWindowMonitor(window, monitor_primary, 0, 0, monitor_width, monitor_height, target_fps);
				screen_width = monitor_width;
				screen_height = monitor_height;
				
				wsDefRenResize();
			} else {
				printf("Switching to windowed mode...\n");
				glfwSetWindowMonitor(window, NULL, win_posx, win_posy, window_width, window_height, target_fps);
				screen_width = window_width;
				screen_height = window_height;
				
				wsDefRenResize();
			}
		}
		
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || 
			glfwWindowShouldClose(window) != 0) {
				state = WS_QUIT_NORMAL;
				break;
		} else if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
			state = WS_QUIT_NORMAL_CONSOLE;
			break;
		}
	}
	
	printf("~RUN~\n\n");
	return state;
}

void wsCenterWindow(GLFWwindow *window) {
	win_posx = (monitor_width / 2) - (window_width / 2);
	win_posy = (monitor_height / 2) - (window_height / 2);
}
// For monitor state changes.
void wsMonitorCallback(GLFWmonitor *monitor, int ev) {
	switch(ev) {
		case GLFW_CONNECTED:
			printf("Monitor connected\n");
			monitor = glfwGetPrimaryMonitor();
			break;
		case GLFW_DISCONNECTED:
			printf("Monitor disconnected\n");
			monitor = glfwGetPrimaryMonitor();
			break;
		default:
			printf("Misc. monitor event occurred: %d\n", ev);
			break;
	}
}
void wsFrameBufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
	printf("Frame buffer size changed to: %d x %d\n", width, height);
}

int wsQuit(unsigned int app_state) {
	printf("~QUIT~\n");
	
	wsGraphicsTerminate();
	
	glfwDestroyCursor(cursor);
	glfwDestroyWindow(window);
	glfwTerminate();
	
	printf("Exit code %d.  ", app_state);
	switch(app_state) {
		case WS_OKAY: 
			printf("App should not have quit.  Uh oh.\n");
			getchar();
			break;
		case WS_QUIT_NORMAL: 
			printf("Exiting normally.  Goodbye!\n");
			break;
		case WS_QUIT_NORMAL_CONSOLE: 
			printf("Exiting normally w/ console.  Nerd.\n");
			printf("Hit enter to exit.\n");
			getchar();
			break;
		case WS_ERROR_GLFW:
			printf("There was a GLFW error.\n");
			getchar();
			break;
		case WS_ERROR_GLEW: 
			printf("There was a GLEW error.\n");
			getchar();
			break;
		case WS_ERROR_FREETYPE: 
			printf("There was a FreeType error.\n");
			getchar();
			break;
		case WS_ERROR_UNKNOWN: 
			printf("Unknown error.  What did you fuck up this time??\n");
			getchar();
			break;
	}
	
	printf("~QUIT~\n");
	return app_state;
}