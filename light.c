#include"light.h"

enum CONSTANT_LINEAR_QUADRATIC { CONSTANT, LINEAR, QUADRATIC };
float light_minintensity[3] = { 1.0f, 0.7f, 1.8f };
float light_maxintensity[3] = { 1.0f, 0.014f, 0.000007f };

float wsLightCalcConstant(float intensity)	{ return light_minintensity[CONSTANT] + (light_maxintensity[CONSTANT] - light_minintensity[CONSTANT]) * intensity; }
float wsLightCalcLinear(float intensity)	{ return light_minintensity[LINEAR] + (light_maxintensity[LINEAR] - light_minintensity[LINEAR]) * intensity; }
float wsLightCalcQuadratic(float intensity)	{ return light_minintensity[QUADRATIC] + (light_maxintensity[QUADRATIC] - light_minintensity[QUADRATIC]) * intensity; }

void wsLightInitp(unsigned int lightID, vec3 position, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic);
void wsLightInitf(unsigned int lightID, vec3 position, vec3 rotation, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic, float cutoff, float outer_cutoff);
void wsLightInitd(unsigned int lightID, vec3 rotation, vec3 ambient, vec3 diffuse, vec3 specular);

void wsLightSetSizep(unsigned int lightID, float size);
void wsLightSetSizef(unsigned int lightID, float size);

void wsLightSetIntensityp(unsigned int lightID, float intensity);
void wsLightSetIntensityf(unsigned int lightID, float intensity);
void wsLightSetIntensityd(unsigned int lightID, float intensity);

void wsLightTogglep(unsigned int lightID, bool on);
void wsLightTogglef(unsigned int lightID, bool on);
void wsLightToggled(unsigned int lightID, bool on);

void wsLightQuickInitp(unsigned int lightID, vec3 position, vec3 color, float intensity, float size, bool on) {
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
	wsLightSetIntensityp(lightID, intensity);
	wsLightSetSizep(lightID, size);
	wsLightTogglep(lightID, on);
}
void wsLightQuickInitf(unsigned int lightID, vec3 position, vec3 rotation, vec3 color, float intensity, float size, unsigned int spread, bool on) {
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
	wsLightSetIntensityf(lightID, intensity);
	wsLightSetSizef(lightID, size);
	wsLightTogglef(lightID, on);
}
void wsLightQuickInitd(unsigned int lightID, vec3 rotation, vec3 color, float intensity, bool on) {
	vec3 ambient;
	glm_vec3_scale(color, 0.1f, ambient);
	
	wsLightInitd(
		lightID, 
		rotation, 
		ambient, 
		color, 
		color
	);
	wsLightSetIntensityd(lightID, intensity);
	wsLightToggled(lightID, on);
}

void wsLightInitp(unsigned int lightID, vec3 position, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic) {
	for(int i = 0; i < 3; i++) {
		pointlights.position[lightID][i] 	= position[i];
		
		pointlights.color[lightID][i]		= diffuse[i];
		pointlights.ambient[lightID][i]		= ambient[i];
		pointlights.diffuse[lightID][i]		= diffuse[i];
		pointlights.specular[lightID][i]	= specular[i];
	}
	pointlights.constant[lightID]			= constant;
	pointlights.linear[lightID]				= linear;
	pointlights.quadratic[lightID]			= quadratic;
	
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
	spotlights.linear[lightID]			= linear;
	spotlights.quadratic[lightID]		= quadratic;
	
	spotlights.cutoff[lightID]			= cutoff;
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

void wsLightSetSizep(unsigned int lightID, float size) {
	pointlights.constant[lightID]	= wsLightCalcConstant(size);
	pointlights.linear[lightID]		= wsLightCalcLinear(size);
	pointlights.quadratic[lightID]	= wsLightCalcQuadratic(size);
}
void wsLightSetSizef(unsigned int lightID, float size) {
	spotlights.constant[lightID]	= wsLightCalcConstant(size);
	spotlights.linear[lightID]		= wsLightCalcLinear(size);
	spotlights.quadratic[lightID]	= wsLightCalcQuadratic(size);
}

void wsLightSetIntensityp(unsigned int lightID, float intensity) {
	glm_vec3_scale(pointlights.color[lightID], intensity * 0.1f, pointlights.ambient[lightID]);
	glm_vec3_scale(pointlights.color[lightID], intensity, pointlights.diffuse[lightID]);
	glm_vec3_scale(pointlights.color[lightID], intensity, pointlights.specular[lightID]);
}
void wsLightSetIntensityf(unsigned int lightID, float intensity) {
	glm_vec3_scale(spotlights.color[lightID], intensity * 0.1f, spotlights.ambient[lightID]);
	glm_vec3_scale(spotlights.color[lightID], intensity, spotlights.diffuse[lightID]);
	glm_vec3_scale(spotlights.color[lightID], intensity, spotlights.specular[lightID]);
}
void wsLightSetIntensityd(unsigned int lightID, float intensity) {
	glm_vec3_scale(directionlights.color[lightID], intensity * 0.1f, directionlights.ambient[lightID]);
	glm_vec3_scale(directionlights.color[lightID], intensity, directionlights.diffuse[lightID]);
	glm_vec3_scale(directionlights.color[lightID], intensity, directionlights.specular[lightID]);
}

void wsLightTogglep(unsigned int lightID, bool on) { pointlights.toggle[lightID] = on; }
void wsLightTogglef(unsigned int lightID, bool on) { spotlights.toggle[lightID] = on; }
void wsLightToggled(unsigned int lightID, bool on) { directionlights.toggle[lightID] = on; }

bool wsLightGetTogglep(unsigned int lightID) { return pointlights.toggle[lightID]; }
bool wsLightGetTogglef(unsigned int lightID) { return spotlights.toggle[lightID]; }
bool wsLightGetToggled(unsigned int lightID) { return directionlights.toggle[lightID]; }