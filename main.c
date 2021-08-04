#include<stdio.h>
#include<windows.h>
#include<stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#include"globals.h"
#include"multithreading.h"
#include"graphics.h"
#include"input.h"
#include"exploration.h"
#include"entities.h"

// "Local" variables.
int monitor_width;
int monitor_height;
int win_posx;
int win_posy;

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
	printf(" --- BEGIN --- \n\n");
	
	// OG code
	unsigned int state = wsInit();
	if(state == WS_OKAY)
		state = wsRun();
	state = wsQuit(state);
	
	printf("\n --- END --- \n");
	return state;
}

int wsInit() {
	printf("---INIT---\n");
	// Libraries.
	if(!wsInitGLFW()) return WS_ERROR_GLFW;
	if(!wsInitGLEW()) return WS_ERROR_GLEW;
	
	// Engine/Game.
	wsThreadingInit();
	
	// OpenGL handles textures upside down.
	stbi_set_flip_vertically_on_load(true);
	int state = wsGraphicsInit(window);
	if(state != WS_OKAY) return state;
	
	wsInputInit(window, 0.3f);
	wsEntitiesInit();
	wsExploreInit();
	
	printf("---INIT---\n\n");
	return WS_OKAY;
}

bool wsInitGLEW() {
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		fprintf(stderr, "ERROR - Failed to initialize GLEW\n");
		return false;
	} else printf("GLEW initialized\n");
	return true;
}
bool wsInitGLFW() {
	if(!glfwInit()) {
		fprintf(stderr, "ERROR - Failed to initialize GLFW\n");
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
	monitor_primary = glfwGetPrimaryMonitor();
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
	
	printf("---PLATFORM INFO---\n");
	printf("%u fps, %.4f frametime\n", target_fps, (1.0f / target_fps));
	printf("Video mode: %d x %d\n", video_mode->width, video_mode->height);
	printf("Window dimensions: %d x %d\n", window_width, window_height);
	printf("Monitor content scale: %.2f x %.2f\n", content_scale_x, content_scale_y);
	printf("---PLATFORM INFO---\n\n");
	
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
	
	printf("GLFW initialized\n");
	return true;
}

int wsRun() {
	printf("---RUN---\n");
	
	double now_time;
	double last_time = glfwGetTime();
	
	unsigned int state = WS_OKAY;
	while(state == WS_OKAY) {
		now_time = glfwGetTime();
		delta_time = now_time - last_time;
		last_time = now_time;
		
		// TODO: MAKE THIS PART MULTITHREADED.
		wsExploreUpdate(delta_time);
		wsGraphicsRender(is_paused);
		wsInputUpdate();
		glfwPollEvents();
		glfwSwapBuffers(window);
		
		if(wsInputGetPress(GLFW_KEY_F11)) {
			if(glfwGetWindowMonitor(window) == NULL) {
				printf("Switching to fullscreen mode...\n");
				glfwSetWindowMonitor(window, monitor_primary, 0, 0, monitor_width, monitor_height, target_fps);
			} else {
				printf("Switching to windowed mode...\n");
				glfwSetWindowMonitor(window, NULL, win_posx, win_posy, window_width, window_height, target_fps);
			}
		} else if(wsInputGetPress(GLFW_KEY_ESCAPE))
			is_paused = !is_paused;
		
		if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS || 
			glfwWindowShouldClose(window) != 0) {
				state = WS_QUIT_NORMAL;
				break;
		} else if(glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
			state = WS_QUIT_NORMAL_CONSOLE;
			break;
		}
	}
	
	printf("---RUN---\n\n");
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
			printf("Misc. monitor event occurred, event code: %d\n", ev);
			break;
	}
}
void wsFrameBufferSizeCallback(GLFWwindow *window, int width, int height) {
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, width, height);
	wsGraphicsResize();
	
	printf("Resized framebuffer to: %d x %d\n", screen_width, screen_height);
}

int wsQuit(unsigned int app_state) {
	printf("---QUIT---\n");
	
	wsExploreTerminate();
	wsEntitiesTerminate();
	wsGraphicsTerminate();
	wsThreadingTerminate();
	
	// Libraries
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
	
	printf("---QUIT---\n");
	return app_state;
}