#include<stdio.h>
#include<stdlib.h>

#include"graphics.h"
#include"vibrance.h"

unsigned int vbo;
unsigned int vao;

GLuint programID;

float vertices[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};

unsigned int shader_program;

float GraphicsGetRandomColorChannel()
	{ return (float)rand() / (float)RAND_MAX; }

void GraphicsInit() {
	// General OpenGL initialization calls.
	glClearColor(0.69f, 0.82f, 0.96f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	int max_vert_attribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vert_attribs);
	printf("Max # of vertex attributes is %d\n", max_vert_attribs);
	
	 // Gen VBO, gen. & bind VAO.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	
	// Bind & initialize vertex buffer object.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	 // Set vertex attribute pointer(s).
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	shader_program = vibrance_LoadShader("minimal.vert", "minimal.frag");
	
	 // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe.
	
	printf("Graphics initialized\n");
}

void GraphicsRender() { // The exciting stuff!
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Draw test triangle!
	vibrance_UseShader(shader_program);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

void GraphicsTerminate() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(shader_program);
	
	printf("Graphics terminated\n");
}