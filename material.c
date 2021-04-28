#include"material.h"

void wsMaterialInit(wsMaterial *material, vec3 ambient, vec3 diffuse, vec3 specular, float shininess) {
	for(int i = 0; i < 3; i++) {
		material->ambient[i] = ambient[i];
		material->diffuse[i] = diffuse[i];
		material->specular[i] = specular[i];
	}
	material->shininess = shininess;
	
	printf("Material %p: ambient: %.2f %.2f %.2f | diffuse: %.2f %.2f %.2f | specular: %.2f %.2f %.2f | shininess %.2f\n", material, 
		material->ambient[0], material->ambient[1], material->ambient[2], material->diffuse[0], material->diffuse[1], material->diffuse[2], 
		material->specular[0], material->specular[1], material->specular[2], material->shininess);
}

void wsMaterialMakeRainbow(wsMaterial *material) {
	vec3 color = {sin(glfwGetTime() * 2.0f), sin(glfwGetTime() * 0.7f), sin(glfwGetTime() * 1.3f)};
	glm_vec3_mul(color, (vec3){1.0f, 1.0f, 1.0f}, material->diffuse);
	glm_vec3_mul(material->diffuse, (vec3){0.2f, 0.2f, 0.2f}, material->ambient);
}

void wsMaterialMakeEmerald(wsMaterial *material) {
	material->ambient[0] = 0.0215f;
	material->ambient[1] = 0.1745f;
	material->ambient[2] = 0.0215f;
	
	material->diffuse[0] = 0.07568f;
	material->diffuse[1] = 0.61424f;
	material->diffuse[2] = 0.07568f;
	
	material->specular[0] = 0.633f;
	material->specular[1] = 0.727811f;
	material->specular[2] = 0.633f;
	
	material->shininess = 0.6f * 128;
}

void wsMaterialMakePlasticCyan(wsMaterial *material) {
	material->ambient[0] = 0.0f;
	material->ambient[1] = 0.1f;
	material->ambient[2] = 0.06f;
	
	material->diffuse[0] = 0.0f;
	material->diffuse[1] = 0.50980392f;
	material->diffuse[2] = 0.50980392f;
	
	material->specular[0] = 0.50196078f;
	material->specular[1] = 0.50196078f;
	material->specular[2] = 0.50196078f;
	
	material->shininess = 0.25f * 128;
}

void wsMaterialMakeRubberWhite(wsMaterial *material) {
	material->ambient[0] = 0.05f;
	material->ambient[1] = 0.05f;
	material->ambient[2] = 0.05f;
	
	material->diffuse[0] = 1.0f;
	material->diffuse[1] = 1.0f;
	material->diffuse[2] = 1.0f;
	
	material->specular[0] = 0.7f;
	material->specular[1] = 0.7f;
	material->specular[2] = 0.7f;
	
	material->shininess = 0.078125f * 128;
}