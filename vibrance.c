#include<stdio.h>
#include<stdlib.h>

#include"vibrance.h"

unsigned int vibrance_LoadShader(const char *path_vert, const char *path_frag) {
	unsigned int shader_program;
	FILE *file;
	char *source_vert, *source_frag;
	unsigned int file_size;
	
	 // Read vertex shader first.
	file = fopen(path_vert, "rb");
	
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	source_vert = (char*)malloc(file_size + 1);
	fread(source_vert, 1, file_size, file);
	
	 // Read fragment shader second.
	file = fopen(path_frag, "rb");
	
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	source_frag = (char*)malloc(file_size + 1);
	fread(source_frag, 1, file_size, file);
	
	fclose(file);
	
	// Create vertex shader.
	unsigned int vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &source_vert, NULL);
	glCompileShader(vert_shader);
	
	int success;
	char info_log[512];
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(vert_shader, 512, NULL, info_log);
		printf("!Vertex shader '%s' compilation failed! %s\n", GlobalGetVariableName(vert_shader), info_log);
	} else printf("Vertex shader '%s' compiled\n", GlobalGetVariableName(vert_shader));
	
	// Create fragment shader.
	unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &source_frag, NULL);
	glCompileShader(frag_shader);
	
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(frag_shader, 512, NULL, info_log);
		printf("!Fragment shader '%s' compilation failed! %s\n", GlobalGetVariableName(frag_shader), info_log);
	} else printf("Fragment shader '%s' compiled\n", GlobalGetVariableName(frag_shader));
	
	// Create shader program.
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);
	
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		printf("!Shader program compilation failed! %s\n", info_log);
	} else {
		glUseProgram(shader_program);
		printf("Shader program compiled\n");
	}
	
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
	
	return shader_program;
}


void vibrance_UseShader(unsigned int id) { glUseProgram(id); }
void vibrance_SetBool(unsigned int id, const char* name, bool value)
	{ glUniform1i(glGetUniformLocation(id, name), (int)value); }
void vibrance_SetInt(unsigned int id, const char* name, int value)
	{ glUniform1i(glGetUniformLocation(id, name), value); }
void vibrance_SetFloat(unsigned int id, const char* name, float value)
	{ glUniform1f(glGetUniformLocation(id, name), (int)value); }