#include"light.h"

enum CONSTANT_LINEAR_QUADRATIC { CONSTANT, LINEAR, QUADRATIC };
float light_minintensity[3] = { 1.0f, 0.7f, 1.8f };
float light_maxintensity[3] = { 1.0f, 0.014f, 0.000007f };

float wsLightCalcConstant(float intensity) { return light_minintensity[CONSTANT] + (light_maxintensity[CONSTANT] - light_minintensity[CONSTANT]) * intensity; }
float wsLightCalcLinear(float intensity) { return light_minintensity[LINEAR] + (light_maxintensity[LINEAR] - light_minintensity[LINEAR]) * intensity; }
float wsLightCalcQuadratic(float intensity) { return light_minintensity[QUADRATIC] + (light_maxintensity[QUADRATIC] - light_minintensity[QUADRATIC]) * intensity; }

void wsLightInitp(unsigned int lightID, vec3 position, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic);
void wsLightInitf(unsigned int lightID, vec3 position, vec3 rotation, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic, float cutoff, float outer_cutoff);
void wsLightInitd(unsigned int lightID, vec3 rotation, vec3 ambient, vec3 diffuse, vec3 specular);

void wsLightQuickInitp(unsigned int lightID, vec3 position, vec3 color, float intensity) {
	vec3 ambient;
	glm_vec3_scale(color, 0.1f, ambient);
	
	wsLightInitp(
		lightID, 
		position, 
		ambient, 
		color, 
		color, 
		wsLightCalcConstant(intensity) , 
		wsLightCalcLinear(intensity), 
		wsLightCalcQuadratic(intensity)
	);
}
void wsLightQuickInitf(unsigned int lightID, vec3 position, vec3 rotation, vec3 color, float intensity, unsigned int spread) {
	vec3 ambient;
	glm_vec3_scale(color, 0.1f, ambient);
	
	wsLightInitf(
		lightID, 
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
void wsLightQuickInitd(unsigned int lightID, vec3 rotation, vec3 color, float intensity) {
	vec3 ambient;
	glm_vec3_scale(color, 0.1f, ambient);
	
	wsLightInitd(
		lightID, 
		rotation, 
		ambient, 
		color, 
		color
	);
}

void wsLightInitp(unsigned int lightID, vec3 position, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic) {
	for(int i = 0; i < 3; i++) {
		pointlights.position[lightID][i] 	= position[i];
		
		pointlights.color[lightID][i]		= diffuse[i];
		pointlights.ambient[lightID][i]		= ambient[i];
		pointlights.diffuse[lightID][i]		= diffuse[i];
		pointlights.specular[lightID][i]	= specular[i];
	}
	pointlights.constant[lightID]	= constant;
	pointlights.linear[lightID]		= linear;
	pointlights.quadratic[lightID]	= quadratic;
	
	printf("Point light %d initialized\n", lightID);
}
void wsLightInitf(unsigned int lightID, vec3 position, vec3 rotation, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic, float cutoff, float outer_cutoff) {
	for(int i = 0; i < 3; i++) {
		spotlights.position[lightID][i] = position[i];
		spotlights.rotation[lightID][i] = rotation[i];
		
		spotlights.color[lightID][i]	= diffuse[i];
		spotlights.ambient[lightID][i]	= ambient[i];
		spotlights.diffuse[lightID][i]	= diffuse[i];
		spotlights.specular[lightID][i]	= specular[i];
	}
	spotlights.constant[lightID]		= constant;
	spotlights.linear[lightID]		= linear;
	spotlights.quadratic[lightID]	= quadratic;
	
	spotlights.cutoff[lightID]		= cutoff;
	spotlights.outer_cutoff[lightID]	= outer_cutoff;
	
	printf("Spot light %d initialized\n", lightID);
}
void wsLightInitd(unsigned int lightID, vec3 rotation, vec3 ambient, vec3 diffuse, vec3 specular) {
	for(int i = 0; i < 3; i++) {
		directionlights.rotation[lightID][i]	= rotation[i];
		
		directionlights.color[lightID][i]		= diffuse[i];
		directionlights.ambient[lightID][i]		= ambient[i];
		directionlights.diffuse[lightID][i]		= diffuse[i];
		directionlights.specular[lightID][i]	= specular[i];
	}
	
	printf("Directional light %d initialized\n", lightID);
}

void wsLightSetPositionp(unsigned int lightID, vec3 position) {
	pointlights.position[lightID][0] = position[0];
	pointlights.position[lightID][1] = position[1];
	pointlights.position[lightID][2] = position[2];
}
void wsLightSetPositionf(unsigned int lightID, vec3 position) {
	spotlights.position[lightID][0] = position[0];
	spotlights.position[lightID][1] = position[1];
	spotlights.position[lightID][2] = position[2];
}
void wsLightSetRotationf(unsigned int lightID, vec3 rotation) {
	spotlights.rotation[lightID][0] = rotation[0];
	spotlights.rotation[lightID][1] = rotation[1];
	spotlights.rotation[lightID][2] = rotation[2];
}
void wsLightSetRotationd(unsigned int lightID, vec3 rotation) {
	directionlights.rotation[lightID][0] = rotation[0];
	directionlights.rotation[lightID][1] = rotation[1];
	directionlights.rotation[lightID][2] = rotation[2];
}

void wsLightSetColorp(unsigned int lightID, vec3 color) {
	for(unsigned int i = 0; i < 3; i++) {
		pointlights.color[lightID][i]		= color[i];
		pointlights.ambient[lightID][i]		= color[i] * 0.1f;
		pointlights.diffuse[lightID][i]		= color[i];
		pointlights.specular[lightID][i]	= color[i];
	}
}
void wsLightSetColorf(unsigned int lightID, vec3 color) {
	for(unsigned int i = 0; i < 3; i++) {
		spotlights.color[lightID][i]	= color[i];
		spotlights.ambient[lightID][i]	= color[i] * 0.1f;
		spotlights.diffuse[lightID][i]	= color[i];
		spotlights.specular[lightID][i]	= color[i];
	}
}
void wsLightSetColord(unsigned int lightID, vec3 color) {
	for(unsigned int i = 0; i < 3; i++) {
		directionlights.color[lightID][i]		= color[i];
		directionlights.ambient[lightID][i]		= color[i] * 0.1f;
		directionlights.diffuse[lightID][i]		= color[i];
		directionlights.specular[lightID][i]	= color[i];
	}
}

void wsLightSetIntensityp(unsigned int lightID, float intensity) {
	pointlights.constant[lightID]	= wsLightCalcConstant(intensity);
	pointlights.linear[lightID]		= wsLightCalcLinear(intensity);
	pointlights.quadratic[lightID]	= wsLightCalcQuadratic(intensity);
}
void wsLightSetIntensityf(unsigned int lightID, float intensity) {
	spotlights.constant[lightID]	= wsLightCalcConstant(intensity);
	spotlights.linear[lightID]		= wsLightCalcLinear(intensity);
	spotlights.quadratic[lightID]	= wsLightCalcQuadratic(intensity);
}

void wsLightTogglep(unsigned int lightID, bool turn_on) {
	if(turn_on) {
		for(unsigned int i = 0; i < 3; i++) {
			pointlights.ambient[lightID][i]		= pointlights.color[lightID][i] * 0.1f;
			pointlights.diffuse[lightID][i]		= pointlights.color[lightID][i];
			pointlights.specular[lightID][i]	= pointlights.color[lightID][i];
		}
	} else {
		glm_vec3_zero(pointlights.ambient[lightID]);
		glm_vec3_zero(pointlights.diffuse[lightID]);
		glm_vec3_zero(pointlights.specular[lightID]);
	}
}
void wsLightTogglef(unsigned int lightID, bool turn_on) {
	if(turn_on) {
		for(unsigned int i = 0; i < 3; i++) {
			spotlights.ambient[lightID][i]	= spotlights.color[lightID][i] * 0.1f;
			spotlights.diffuse[lightID][i]	= spotlights.color[lightID][i];
			spotlights.specular[lightID][i]	= spotlights.color[lightID][i];
		}
	} else {
		glm_vec3_zero(spotlights.ambient[lightID]);
		glm_vec3_zero(spotlights.diffuse[lightID]);
		glm_vec3_zero(spotlights.specular[lightID]);
	}
}
void wsLightToggled(unsigned int lightID, bool turn_on) {
	if(turn_on) {
		for(unsigned int i = 0; i < 3; i++) {
			directionlights.ambient[lightID][i]	= directionlights.color[lightID][i] * 0.1f;
			directionlights.diffuse[lightID][i]	= directionlights.color[lightID][i];
			directionlights.specular[lightID][i]	= directionlights.color[lightID][i];
		}
	} else {
		glm_vec3_zero(directionlights.ambient[lightID]);
		glm_vec3_zero(directionlights.diffuse[lightID]);
		glm_vec3_zero(directionlights.specular[lightID]);
	}
}