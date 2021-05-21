#include"light.h"
#include"globals.h"

enum CONSTANT_LINEAR_QUADRATIC { CONSTANT, LINEAR, QUADRATIC };
float light_minintensity[3] = { 1.0f, 0.7f, 1.8f };
float light_maxintensity[3] = { 1.0f, 0.014f, 0.000007f };

float wsLightCalcConstant(float intensity)	{ return light_minintensity[CONSTANT] + (light_maxintensity[CONSTANT] - light_minintensity[CONSTANT]) * intensity; }
float wsLightCalcLinear(float intensity)	{ return light_minintensity[LINEAR] + (light_maxintensity[LINEAR] - light_minintensity[LINEAR]) * intensity; }
float wsLightCalcQuadratic(float intensity)	{ return light_minintensity[QUADRATIC] + (light_maxintensity[QUADRATIC] - light_minintensity[QUADRATIC]) * intensity; }

void wsLightSetSizep(unsigned int lightID, float size);
void wsLightSetSizef(unsigned int lightID, float size);

void wsLightSetIntensityp(unsigned int lightID, float intensity);
void wsLightSetIntensityf(unsigned int lightID, float intensity);
void wsLightSetIntensityd(unsigned int lightID, float intensity);

void wsLightTogglep(unsigned int lightID, bool on);
void wsLightTogglef(unsigned int lightID, bool on);
void wsLightToggled(unsigned int lightID, bool on);

void wsLightInitp(unsigned int lightID, vec3 position, vec3 color, float intensity, float size, bool on) {
	wsLightTogglep(lightID, on);
	wsLightSetPositionp(lightID, position);
	wsLightSetColorp(lightID, color);
	wsLightSetIntensityp(lightID, intensity);
	wsLightSetSizep(lightID, size);
}
void wsLightInitf(unsigned int lightID, vec3 position, vec3 rotation, vec3 color, float intensity, float size, unsigned int spread, bool on) {
	wsLightTogglef(lightID, on);
	wsLightSetPositionf(lightID, position);
	wsLightSetRotationf(lightID, rotation);
	wsLightSetColorf(lightID, color);
	wsLightSetIntensityf(lightID, intensity);
	wsLightSetSizef(lightID, size);
	spotlights.cutoff[lightID]			= cos(glm_rad(5.0f));
	spotlights.outer_cutoff[lightID]	= cos(glm_rad(spread + (spread * 0.4f)));
}
void wsLightInitd(unsigned int lightID, vec3 rotation, vec3 color, float intensity, bool on) {
	wsLightToggled(lightID, on);
	wsLightSetRotationd(lightID, rotation);
	wsLightSetColord(lightID, color);
	wsLightSetIntensityd(lightID, intensity);
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

void wsLightSetColorp(unsigned int lightID, vec3 color) { glm_vec3_copy(color, pointlights.color[lightID]); }
void wsLightSetColorf(unsigned int lightID, vec3 color) { glm_vec3_copy(color, spotlights.color[lightID]); }
void wsLightSetColord(unsigned int lightID, vec3 color) { glm_vec3_copy(color, directionlights.color[lightID]); }
void wsLightInterpColorp(unsigned int lightID, vec3 color0, vec3 color1, float interp) {
	vec3 new_color = {
		wsInterpf(color0[0], color1[0], interp), 
		wsInterpf(color0[1], color1[1], interp), 
		wsInterpf(color0[2], color1[2], interp), 
	};
	wsLightSetColorp(lightID, new_color);
}
void wsLightInterpColorf(unsigned int lightID, vec3 color0, vec3 color1, float interp) {
	vec3 new_color = {
		wsInterpf(color0[0], color1[0], interp), 
		wsInterpf(color0[1], color1[1], interp), 
		wsInterpf(color0[2], color1[2], interp), 
	};
	wsLightSetColorf(lightID, new_color);
}
void wsLightInterpColord(unsigned int lightID, vec3 color0, vec3 color1, float interp) {
	vec3 new_color = {
		wsInterpf(color0[0], color1[0], interp), 
		wsInterpf(color0[1], color1[1], interp), 
		wsInterpf(color0[2], color1[2], interp), 
	};
	wsLightSetColord(lightID, new_color);
}

void wsLightSetIntensityp(unsigned int lightID, float intensity) { pointlights.intensity[lightID] = intensity; }
void wsLightSetIntensityf(unsigned int lightID, float intensity) { spotlights.intensity[lightID] = intensity; }
void wsLightSetIntensityd(unsigned int lightID, float intensity) { directionlights.intensity[lightID] = intensity; }

void wsLightSetSizep(unsigned int lightID, float size) {
	pointlights.linear[lightID]		= wsLightCalcLinear(size);
	pointlights.quadratic[lightID]	= wsLightCalcQuadratic(size);
}
void wsLightSetSizef(unsigned int lightID, float size) {
	spotlights.linear[lightID]		= wsLightCalcLinear(size);
	spotlights.quadratic[lightID]	= wsLightCalcQuadratic(size);
}

void wsLightTogglep(unsigned int lightID, bool on) { pointlights.toggle[lightID] = on; }
void wsLightTogglef(unsigned int lightID, bool on) { spotlights.toggle[lightID] = on; }
void wsLightToggled(unsigned int lightID, bool on) { directionlights.toggle[lightID] = on; }

bool wsLightGetTogglep(unsigned int lightID) { return pointlights.toggle[lightID]; }
bool wsLightGetTogglef(unsigned int lightID) { return spotlights.toggle[lightID]; }
bool wsLightGetToggled(unsigned int lightID) { return directionlights.toggle[lightID]; }