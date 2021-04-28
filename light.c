#include"light.h"

enum CONSTANT_LINEAR_QUADRATIC { CONSTANT, LINEAR, QUADRATIC };
float light_minintensity[3] = { 1.0f, 0.7f, 1.8f };
float light_maxintensity[3] = { 1.0f, 0.014f, 0.000007f };

float wsLightCalcConstant(float intensity) { return light_minintensity[CONSTANT] + (light_maxintensity[CONSTANT] - light_minintensity[CONSTANT]) * intensity; }
float wsLightCalcLinear(float intensity) { return light_minintensity[LINEAR] + (light_maxintensity[LINEAR] - light_minintensity[LINEAR]) * intensity; }
float wsLightCalcQuadratic(float intensity) { return light_minintensity[QUADRATIC] + (light_maxintensity[QUADRATIC] - light_minintensity[QUADRATIC]) * intensity; }

void wsLightInitp(wsPointLight *light_source, vec3 position, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic);
void wsLightInitf(wsSpotLight *light_source, vec3 position, vec3 rotation, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic, float cutoff, float outer_cutoff);
void wsLightInitd(wsDirectionLight *light_source, vec3 rotation, vec3 ambient, vec3 diffuse, vec3 specular);

void wsLightQuickInitp(wsPointLight *light_source, vec3 position, vec3 color, float intensity) {
	vec3 ambient;
	glm_vec3_scale(color, 0.1f, ambient);
	
	wsLightInitp(
		light_source, 
		position, 
		ambient, 
		color, 
		color, 
		wsLightCalcConstant(intensity) , 
		wsLightCalcLinear(intensity), 
		wsLightCalcQuadratic(intensity)
	);
}
void wsLightQuickInitf(wsSpotLight *light_source, vec3 position, vec3 rotation, vec3 color, float intensity, unsigned int spread) {
	vec3 ambient;
	glm_vec3_scale(color, 0.1f, ambient);
	
	wsLightInitf(
		light_source, 
		position, 
		rotation, 
		ambient, 
		color, 
		color, 
		wsLightCalcConstant(intensity), 
		wsLightCalcLinear(intensity), 
		wsLightCalcQuadratic(intensity), 
		cos(glm_rad(5.0f)), 
		cos(glm_rad(spread + (spread * 0.4f)))
	);
}
void wsLightQuickInitd(wsDirectionLight *light_source, vec3 rotation, vec3 color, float intensity) {
	vec3 ambient;
	glm_vec3_scale(color, 0.1f, ambient);
	
	wsLightInitd(
		light_source, 
		rotation, 
		ambient, 
		color, 
		color
	);
}

void wsLightInitp(wsPointLight *light_source, vec3 position, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic) {
	for(int i = 0; i < 3; i++) {
		light_source->position[i] = position[i];
		
		light_source->color[i]		= diffuse[i];
		light_source->ambient[i]	= ambient[i];
		light_source->diffuse[i]	= diffuse[i];
		light_source->specular[i]	= specular[i];
	}
	light_source->constant = constant;
	light_source->linear = linear;
	light_source->quadratic = quadratic;
	
	printf("Point light @ %p initialized\n", light_source);
}
void wsLightInitf(wsSpotLight *light_source, vec3 position, vec3 rotation, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic, float cutoff, float outer_cutoff) {
	for(int i = 0; i < 3; i++) {
		light_source->position[i] = position[i];
		light_source->rotation[i] = rotation[i];
		
		light_source->color[i]		= diffuse[i];
		light_source->ambient[i]	= ambient[i];
		light_source->diffuse[i]	= diffuse[i];
		light_source->specular[i]	= specular[i];
	}
	light_source->constant = constant;
	light_source->linear = linear;
	light_source->quadratic = quadratic;
	
	light_source->cutoff = cutoff;
	light_source->outer_cutoff = outer_cutoff;
	
	printf("Spot light @ %p initialized\n", light_source);
}
void wsLightInitd(wsDirectionLight *light_source, vec3 rotation, vec3 ambient, vec3 diffuse, vec3 specular) {
	for(int i = 0; i < 3; i++) {
		light_source->rotation[i] = rotation[i];
		
		light_source->color[i]		= diffuse[i];
		light_source->ambient[i]	= ambient[i];
		light_source->diffuse[i]	= diffuse[i];
		light_source->specular[i]	= specular[i];
	}
	
	printf("Directional light @ %p initialized\n", light_source);
}

void wsLightSetPositionp(wsPointLight *light_source, vec3 position) {
	light_source->position[0] = position[0];
	light_source->position[1] = position[1];
	light_source->position[2] = position[2];
}
void wsLightSetPositionf(wsSpotLight *light_source, vec3 position) {
	light_source->position[0] = position[0];
	light_source->position[1] = position[1];
	light_source->position[2] = position[2];
}
void wsLightSetRotationf(wsSpotLight *light_source, vec3 rotation) {
	light_source->rotation[0] = rotation[0];
	light_source->rotation[1] = rotation[1];
	light_source->rotation[2] = rotation[2];
}
void wsLightSetRotationd(wsDirectionLight *light_source, vec3 rotation) {
	light_source->rotation[0] = rotation[0];
	light_source->rotation[1] = rotation[1];
	light_source->rotation[2] = rotation[2];
}

void wsLightSetColorp(wsPointLight *light_source, vec3 color) {
	for(unsigned int i = 0; i < 3; i++) {
		light_source->color[i]		= color[i];
		light_source->ambient[i]	= color[i] * 0.1f;
		light_source->diffuse[i]	= color[i];
		light_source->specular[i]	= color[i];
	}
}
void wsLightSetColorf(wsSpotLight *light_source, vec3 color) {
	for(unsigned int i = 0; i < 3; i++) {
		light_source->color[i]		= color[i];
		light_source->ambient[i]	= color[i] * 0.1f;
		light_source->diffuse[i]	= color[i];
		light_source->specular[i]	= color[i];
	}
}
void wsLightSetColord(wsDirectionLight *light_source, vec3 color) {
	for(unsigned int i = 0; i < 3; i++) {
		light_source->color[i]		= color[i];
		light_source->ambient[i]	= color[i] * 0.1f;
		light_source->diffuse[i]	= color[i];
		light_source->specular[i]	= color[i];
	}
}

void wsLightSetIntensityp(wsPointLight *light_source, float intensity) {
	light_source->constant	= wsLightCalcConstant(intensity);
	light_source->linear	= wsLightCalcLinear(intensity);
	light_source->quadratic	= wsLightCalcQuadratic(intensity);
}
void wsLightSetIntensityf(wsSpotLight *light_source, float intensity) {
	light_source->constant	= wsLightCalcConstant(intensity);
	light_source->linear	= wsLightCalcLinear(intensity);
	light_source->quadratic	= wsLightCalcQuadratic(intensity);
}

void wsLightTogglep(wsPointLight *light_source, bool turn_on) {
	if(turn_on) {
		for(unsigned int i = 0; i < 3; i++) {
			light_source->ambient[i]	= light_source->color[i] * 0.1f;
			light_source->diffuse[i]	= light_source->color[i];
			light_source->specular[i]	= light_source->color[i];
		}
	} else {
		glm_vec3_zero(light_source->ambient);
		glm_vec3_zero(light_source->diffuse);
		glm_vec3_zero(light_source->specular);
	}
}
void wsLightTogglef(wsSpotLight *light_source, bool turn_on) {
	if(turn_on) {
		for(unsigned int i = 0; i < 3; i++) {
			light_source->ambient[i]	= light_source->color[i] * 0.1f;
			light_source->diffuse[i]	= light_source->color[i];
			light_source->specular[i]	= light_source->color[i];
		}
	} else {
		glm_vec3_zero(light_source->ambient);
		glm_vec3_zero(light_source->diffuse);
		glm_vec3_zero(light_source->specular);
	}
}
void wsLightToggled(wsDirectionLight *light_source, bool turn_on) {
	if(turn_on) {
		for(unsigned int i = 0; i < 3; i++) {
			light_source->ambient[i]	= light_source->color[i] * 0.1f;
			light_source->diffuse[i]	= light_source->color[i];
			light_source->specular[i]	= light_source->color[i];
		}
	} else {
		glm_vec3_zero(light_source->ambient);
		glm_vec3_zero(light_source->diffuse);
		glm_vec3_zero(light_source->specular);
	}
}