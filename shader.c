#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"shader.h"

unsigned int wsShaderLoad(const char *path_vert, const char *path_frag, bool use_on_load, bool print_on_load) {
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
	source_vert[file_size] = '\0';
	
	// Read fragment shader second.
	file = fopen(path_frag, "rb");
	
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	source_frag = (char*)malloc(file_size + 1);
	fread(source_frag, 1, file_size, file);
	source_frag[file_size] = '\0';
	
	fclose(file);
	
	// I will get warnings if this is not here.
	const char *fuckyouopengl_vert = source_vert;
	const char *fuckyouopengl_frag = source_frag;
	
	if(print_on_load)
		printf("VERTEX_SHADER:\n%s\nFRAGMENT SHADER:\n%s\n", fuckyouopengl_vert, fuckyouopengl_frag);
	
	// Create vertex shader.
	unsigned int vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &fuckyouopengl_vert, NULL);
	glCompileShader(vert_shader);
	
	int success;
	char info_log[512];
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(vert_shader, 512, NULL, info_log);
		printf("%s", info_log);
	}
	
	// Create fragment shader.
	unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &fuckyouopengl_frag, NULL);
	glCompileShader(frag_shader);
	
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(frag_shader, 512, NULL, info_log);
		printf("\n%s", info_log);
	}
	
	// Create shader program.
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);
	
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		printf("\n%s", info_log);
	} else {
		printf("Shader program %d compile success\n", shader_program);
	}
	
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
	free(source_vert);
	free(source_frag);
	
	return shader_program;
}
void wsShaderUse(unsigned int shaderID) { glUseProgram(shaderID); }
void wsShaderSetBool(unsigned int shaderID, const char *var_name, bool value)
	{ glProgramUniform1i(shaderID, glGetUniformLocation(shaderID, var_name), (int)value); }
void wsShaderSetInt(unsigned int shaderID, const char *var_name, int value)
	{ glProgramUniform1i(shaderID, glGetUniformLocation(shaderID, var_name), value); }
void wsShaderSetFloat(unsigned int shaderID, const char *var_name, float value)
	{ glProgramUniform1f(shaderID, glGetUniformLocation(shaderID, var_name), value); }
void wsShaderSetVec2(unsigned int shaderID, const char *var_name, vec3 vec)
	{ glProgramUniform2fv(shaderID, glGetUniformLocation(shaderID, var_name), 1, &(vec[0])); }
void wsShaderSetVec3(unsigned int shaderID, const char *var_name, vec3 vec)
	{ glProgramUniform3fv(shaderID, glGetUniformLocation(shaderID, var_name), 1, &(vec[0])); }
void wsShaderSetVec4(unsigned int shaderID, const char *var_name, vec3 vec)
	{ glProgramUniform4fv(shaderID, glGetUniformLocation(shaderID, var_name), 1, &(vec[0])); }
void wsShaderSetMat3(unsigned int shaderID, const char *var_name, mat3 *mat)
	{ glProgramUniformMatrix3fv(shaderID, glGetUniformLocation(shaderID, var_name), 1, GL_FALSE, mat[0][0]); }
void wsShaderSetMat4(unsigned int shaderID, const char *var_name, mat4 *mat)
	{ glProgramUniformMatrix4fv(shaderID, glGetUniformLocation(shaderID, var_name), 1, GL_FALSE, mat[0][0]); }

void wsShaderUpdateCamera(unsigned int shaderID, unsigned int cameraID)
	{ wsShaderSetVec3(shaderID, "view_position", cameras.position[cameraID]); }
void wsShaderSetMVP(unsigned int shaderID, mat4 *model, mat4 *view, mat4 *projection) {
	wsShaderSetMat4(shaderID, "model", model);
	wsShaderSetMat4(shaderID, "view", view);
	wsShaderSetMat4(shaderID, "projection", projection);
}
void wsShaderSetNormalMatrix(unsigned int shaderID, mat4 *model) {
	mat3 normal3 = GLM_MAT3_IDENTITY_INIT;
	mat4 normal4 = GLM_MAT3_IDENTITY_INIT;
	glm_mat4_inv_fast(*model, normal4);
	glm_mat4_transpose(normal4);
	glm_mat4_pick3(normal4, normal3);
	wsShaderSetMat3(shaderID, "normal_matrix", &normal3);	
}

void wsShaderUpdateLights(unsigned int shaderID, unsigned int num_pointlights, unsigned int num_spotlights, unsigned int num_directionlights) {
	for(unsigned int i = 0; i < num_pointlights; i++)
		wsShaderUpdateLightp(shaderID, i);
	for(unsigned int i = 0; i < num_spotlights; i++)
		wsShaderUpdateLightf(shaderID, 0);
	for(unsigned int i = 0; i < num_directionlights; i++)
		wsShaderUpdateLightd(shaderID, 0);
}
void wsShaderUpdateLightsp(unsigned int shaderID, unsigned int num_pointlights) {
	for(unsigned int i = 0; i < num_pointlights; i++)
		wsShaderUpdateLightp(shaderID, i);
}
void wsShaderUpdateLightp(unsigned int shaderID, unsigned int lightID) {
	char cur_str[35];
	
	sprintf(cur_str, "pointlights[%d].position", lightID);
	wsShaderSetVec3(shaderID, cur_str, pointlights.position[lightID]);
	
	sprintf(cur_str, "pointlights[%d].ambient", lightID);
	wsShaderSetVec3(shaderID, cur_str, pointlights.ambient[lightID]);
	sprintf(cur_str, "pointlights[%d].diffuse", lightID);
	wsShaderSetVec3(shaderID, cur_str, pointlights.diffuse[lightID]);
	sprintf(cur_str, "pointlights[%d].specular", lightID);
	wsShaderSetVec3(shaderID, cur_str, pointlights.specular[lightID]);
	
	sprintf(cur_str, "pointlights[%d].constant", lightID);
	wsShaderSetFloat(shaderID, cur_str, pointlights.constant[lightID]);
	sprintf(cur_str, "pointlights[%d].linear", lightID);
	wsShaderSetFloat(shaderID, cur_str, pointlights.linear[lightID]);
	sprintf(cur_str, "pointlights[%d].quadratic", lightID);
	wsShaderSetFloat(shaderID, cur_str, pointlights.quadratic[lightID]);
}
void wsShaderUpdateLightf(unsigned int shaderID, unsigned int lightID) {
	wsShaderSetVec3(shaderID, "spotlight.position", spotlights.position[lightID]);
	wsShaderSetVec3(shaderID, "spotlight.rotation", spotlights.rotation[lightID]);
	
	wsShaderSetFloat(shaderID, "spotlight.cutoff", spotlights.cutoff[lightID]);
	wsShaderSetFloat(shaderID, "spotlight.outer_cutoff", spotlights.outer_cutoff[lightID]);
	
	wsShaderSetVec3(shaderID, "spotlight.ambient", spotlights.ambient[lightID]);
	wsShaderSetVec3(shaderID, "spotlight.diffuse", spotlights.diffuse[lightID]);
	wsShaderSetVec3(shaderID, "spotlight.specular", spotlights.specular[lightID]);
	
	wsShaderSetFloat(shaderID, "spotlight.constant", spotlights.constant[lightID]);
	wsShaderSetFloat(shaderID, "spotlight.linear", spotlights.linear[lightID]);
	wsShaderSetFloat(shaderID, "spotlight.quadratic", spotlights.quadratic[lightID]);
}
void wsShaderUpdateLightd(unsigned int shaderID, unsigned int lightID) {
	wsShaderSetVec3(shaderID, "directionlight.rotation", spotlights.rotation[lightID]);
	
	wsShaderSetVec3(shaderID, "directionlight.ambient", spotlights.ambient[lightID]);
	wsShaderSetVec3(shaderID, "directionlight.diffuse", spotlights.diffuse[lightID]);
	wsShaderSetVec3(shaderID, "directionlight.specular", spotlights.specular[lightID]);
}