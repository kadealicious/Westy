#include<stdio.h>
#include<windows.h>
#include<stdbool.h>

#include"globals.h"
#include"graphics.h"
#include"input.h"

// Declare globals from globals.h
const vec3 WORLD_UP = {0.0f, 1.0f, 0.0f};

int monitor_width;
int monitor_height;
int win_posx;
int win_posy;
int window_width	= 640;
int window_height	= 480;
unsigned int target_fps = 72;
float delta_time = 1.0f;
float field_of_view = 90.0f;// 45.0f

// "Local" variables.
enum QUIT_STATES { NO_QUIT, QUIT_NORMAL, QUIT_NORMAL_CONSOLE, QUIT_ERROR_GLFW, QUIT_ERROR_GLEW, QUIT_ERROR_FREETYPE, QUIT_ERROR_UNKNOWN };

GLFWwindow *window;
GLFWmonitor *monitor_primary;
GLFWcursor *cursor;

bool wsInitGLFW();
bool wsInitGLEW();
int wsInit();
int wsRun();
void wsQuit(unsigned int app_state);
void wsCenterWindow(GLFWwindow *window);

void wsMonitorCallback(GLFWmonitor *monitor, int ev);
void wsFrameBufferSizeCallback(GLFWwindow *window, int width, int height);

int main(int argc, char **argv) {
	unsigned int app_state = NO_QUIT;
	
	// Libraries
	if(!wsInitGLFW()) {
		app_state = QUIT_ERROR_GLFW;
		wsQuit(app_state);
	} if(!wsInitGLEW()) {
		app_state = QUIT_ERROR_GLEW;
		wsQuit(app_state);
	}
	
	FT_Library ft_lib;
	/*if(!FT_Init_FreeType(&ft_lib))
		return QUIT_ERROR_FREETYPE;*/
	
	// OG code
	app_state = wsInit();
	if(app_state == NO_QUIT) {
		app_state = wsRun();
		wsQuit(app_state);
	}
	return app_state;
}

int wsInit() {
	wsGraphicsInit(window);
	wsInputInit(window, 0.3f);
	
	printf("~All set!~\n\n");
	return NO_QUIT;
}

bool wsInitGLEW() {
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW!\n");
		return false;
	} else printf("GLEW initialized\n");
	return true;
}

bool wsInitGLFW() {
	if(!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW!\n");
		return false;
	} else printf("GLFW initialized\n");
	
	// All GLFW initialization shtuff.
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
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
	
	float content_scale_x;
	float content_scale_y;
	glfwGetMonitorContentScale(monitor_primary, &content_scale_x, &content_scale_y);
	
	float target_frametime = 1.0f / target_fps;
	printf("Target %u frames, %f frametime\n", target_fps, target_frametime);
	printf("Video mode: %d x %d\n", video_mode->width, video_mode->height);
	printf("Window dimensions: %d x %d\n", window_width, window_height);
	printf("Monitor content scale: %f x %f\n", content_scale_x, content_scale_y);
	
	window = glfwCreateWindow(window_width, window_height, "My name?  You're asking me, what my name is?  I'll tell you what my name is.", NULL, NULL);
	if(window == NULL) {
		fprintf(stderr, "Failed to open GLFW window!\n");
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
		printf("Raw mouse motion enabled\n");
	} else printf("Raw mouse motion is not supported.  Sorry!\n");
	
	glfwSetFramebufferSizeCallback(window, wsFrameBufferSizeCallback);
	glfwSetKeyCallback(window, wsInputKeyCallback);
	glfwSetCursorPosCallback(window, wsInputCursorPosCallback);
	glfwSetScrollCallback(window, wsInputScrollCallback);
	
	printf("GLFW window & context created\n");
	return true;
}

int wsRun() {
	unsigned int app_state = NO_QUIT;
	
	// Used for restraining the FPS.  delta_time is from globals.h.
	double now_time;
	double last_time			= glfwGetTime();
	
	while(app_state == NO_QUIT) {
		now_time = glfwGetTime();
		delta_time = now_time - last_time;
		last_time = now_time;
		
		// The meat and potatoes!
		wsGraphicsRender();
		wsInputUpdate();
		
		glfwPollEvents();
		glfwSwapBuffers(window);
		
		if(glfwGetKeyOnce(window, GLFW_KEY_F11)) {
			if(glfwGetWindowMonitor(window) == NULL) {
				/* For some reason, every attempt for fullscreen after the first 
					will first adjust the frame buffer size *slightly,* and THEN 
					actually go fullscreen.  My guess as to why this is is because 
					calls to glfwSetWindowMonitor() also slightly adjusts the frame 
					buffer size before actually going fullscreen. ¯\_(ツ)_/¯ */
				printf("Switching to fullscreen mode...\n");
				glfwSetWindowMonitor(window, monitor_primary, 0, 0, monitor_width, monitor_height, target_fps);
			} else {
				printf("Switching to windowed mode...\n");
				glfwSetWindowMonitor(window, NULL, win_posx, win_posy, window_width, window_height, target_fps);
			}
		}
		
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || 
			glfwWindowShouldClose(window) != 0) {
				app_state = QUIT_NORMAL;
				break;
		} else if(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
			app_state = QUIT_NORMAL_CONSOLE;
			break;
		}
	}
	return app_state;
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

void wsQuit(unsigned int app_state) {
	printf("\n");
	
	wsGraphicsTerminate();
	
	glfwDestroyCursor(cursor);
	glfwDestroyWindow(window);
	glfwTerminate();
	
	printf("Exit code %d.  ", app_state);
	switch(app_state) {
		case NO_QUIT: 
			printf("App should not have quit.  Uh oh.\n");
			getchar();
			break;
		case QUIT_NORMAL: 
			printf("Exiting normally.\n");
			break;
		case QUIT_NORMAL_CONSOLE: 
			printf("Exiting normally w/ console.  Nerd.\n");
			printf("Hit enter to exit.\n");
			getchar();
			break;
		case QUIT_ERROR_GLFW:
			printf("There was a GLFW error.\n");
			getchar();
			break;
		case QUIT_ERROR_GLEW: 
			printf("There was a GLEW error.\n");
			getchar();
			break;
		case QUIT_ERROR_FREETYPE: 
			printf("There was a FreeType error.\n");
			getchar();
			break;
		case QUIT_ERROR_UNKNOWN: 
			printf("Unknown error.  What did you fuck up this time??\n");
			getchar();
			break;
	}
	
	printf("~Goodbye!~\n");
}

// DEBUG SHIT.
/*void PrintMemoryUsage() {
	struct rusage memusage;
	getrusage(RUSAGE_SELF, &memusage);
	printf("Nerd stat: %ld\n", memusage.ru_maxrss);
}*/