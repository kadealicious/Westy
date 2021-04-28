#include<stdio.h>
#include<windows.h>
#include<stdbool.h>

#include"globals.h"
#include"graphics.h"
#include"input.h"

// Declare our program globals.
GLFWwindow *program_window;
int screen_width = 640;
int screen_height = 480;
float fov = 90.0f;
vec3 world_up		= {0, 1, 0};
vec3 world_right	= {1, 0, 0};
vec3 world_forward	= {0, 0, -1};

void ProgramFrameBufferSizeCallback(GLFWwindow *win, int width, int height)
	{ glViewport(0, 0, width, height); }

bool ProgramInitGLFW(int screen_width, int screen_height) {
	if(!glfwInit()) { fprintf(stderr, "!Failed to initialize GLFW!\n"); return false; }
	else printf("GLFW initialized\n");

	// All GLFW initialization shtuff.
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	program_window = glfwCreateWindow(screen_width, screen_height, "VS sucks ass but VS Code is hype as fuck (kinda)", NULL, NULL);
	if(program_window == NULL) {
		fprintf(stderr, "!Failed to open GLFW window!\n");
		glfwTerminate();
		return false;
	}
	
	glfwMakeContextCurrent(program_window);

	glfwSwapInterval(1);
	glfwSetInputMode(program_window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetFramebufferSizeCallback(program_window, ProgramFrameBufferSizeCallback);

	printf("GLFW window & context created\n");
	return true;
}

bool ProgramInitGLEW() {
	glewExperimental = true;
	if(glewInit() != GLEW_OK) { fprintf(stderr, "!Failed to initialize GLEW!\n"); return false; }
	else printf("GLEW initialized\n");

	return true;
}

int ProgramRun(unsigned int target_frames, double target_frametime) {
	unsigned int running = 0;
	
	printf("Start run, target %u frames, %f frametime\n", target_frames, target_frametime);

	// Used for restraining the FPS.
	double now_time;
	unsigned int frame_count	= 0;
	double delta_time			= 0.0;
	double last_time			= glfwGetTime();

	while(running == 0) {
		now_time = glfwGetTime();
		delta_time += (double)(now_time - last_time) / target_frametime;
		last_time = now_time;

		while(delta_time >= 1.0) {
			InputProcess();		// Process input!
			// GameUpdate();	// Update game!
			GraphicsRender();	// Render!
			frame_count++;

			if(delta_time < 2.0 && frame_count >= target_frames) {
				printf("%u frames\n", frame_count);
				frame_count = 0;
			}
			delta_time--;
		}
		
		glfwPollEvents();
		glfwSwapBuffers(program_window);

		if(glfwGetKey(program_window, GLFW_KEY_ESCAPE) == GLFW_PRESS || 
			glfwWindowShouldClose(program_window) != 0)
			running = 1;
	}
	return running;
}

void ProgramQuit() {
	printf("\n");
	GraphicsTerminate();
	glfwDestroyWindow(program_window);
	glfwTerminate();
}

int main(int argc, char **argv) {
	if(!ProgramInitGLFW(screen_width, screen_height)) return -1;
	if(!ProgramInitGLEW()) return -1;
	GraphicsInit();
	InputInit(0.005f);

	printf("~Main initialized~\n\n");

	unsigned int target_fps = 72;
	int state = ProgramRun(target_fps, 1.0 / target_fps);
	ProgramQuit();

	printf("~Main terminated.  Goodbye!~");
	return state;
}