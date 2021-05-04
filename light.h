#ifndef LIGHT_H_
#define LIGHT_H_

#include<stdbool.h>
#include"globals.h"

#define WS_MAX_POINTLIGHTS 4
#define WS_MAX_SPOTLIGHTS 4
#define WS_MAX_DIRECTIONLIGHTS 4

typedef struct wsPointLights {
	vec3 position[WS_MAX_POINTLIGHTS];
	vec3 color[WS_MAX_POINTLIGHTS];
	vec3 ambient[WS_MAX_POINTLIGHTS];
	vec3 diffuse[WS_MAX_POINTLIGHTS];
	vec3 specular[WS_MAX_POINTLIGHTS];
	float constant[WS_MAX_POINTLIGHTS];
	float linear[WS_MAX_POINTLIGHTS];
	float quadratic[WS_MAX_POINTLIGHTS];
} wsPointLights;

typedef struct wsSpotLights {
	vec3 position[WS_MAX_SPOTLIGHTS];
	vec3 rotation[WS_MAX_SPOTLIGHTS];
	vec3 color[WS_MAX_SPOTLIGHTS];
	vec3 ambient[WS_MAX_SPOTLIGHTS];
	vec3 diffuse[WS_MAX_SPOTLIGHTS];
	vec3 specular[WS_MAX_SPOTLIGHTS];
	float constant[WS_MAX_SPOTLIGHTS];
	float linear[WS_MAX_SPOTLIGHTS];
	float quadratic[WS_MAX_SPOTLIGHTS];
	float cutoff[WS_MAX_SPOTLIGHTS];
	float outer_cutoff[WS_MAX_SPOTLIGHTS];
} wsSpotLights;

typedef struct wsDirectionLights {
	vec3 rotation[WS_MAX_DIRECTIONLIGHTS];
	vec3 color[WS_MAX_DIRECTIONLIGHTS];
	vec3 ambient[WS_MAX_DIRECTIONLIGHTS];
	vec3 diffuse[WS_MAX_DIRECTIONLIGHTS];
	vec3 specular[WS_MAX_DIRECTIONLIGHTS];
} wsDirectionLights;

wsPointLights pointlights;
wsSpotLights spotlights;
wsDirectionLights directionlights;

// Intensity is a float from 0-1.  Can't guarantee what happens after 1.
void wsLightQuickInitp(unsigned int lightID, vec3 position, vec3 color, float intensity);
// Intensity is a float from 0-1.  Spread is the outer cutoff in degrees.
void wsLightQuickInitf(unsigned int lightID, vec3 position, vec3 rotation, vec3 color, float intensity, unsigned int spread);
// Intensity is a float from 0-1.
void wsLightQuickInitd(unsigned int lightID, vec3 rotation, vec3 color, float intensity);

void wsLightSetPositionp(unsigned int lightID, vec3 position);
void wsLightSetPositionf(unsigned int lightID, vec3 position);
void wsLightSetRotationf(unsigned int lightID, vec3 rotation);
void wsLightSetRotationd(unsigned int lightID, vec3 rotation);

void wsLightSetColorp(unsigned int lightID, vec3 color);
void wsLightSetColorf(unsigned int lightID, vec3 color);
void wsLightSetColord(unsigned int lightID, vec3 color);

void wsLightSetIntensityp(unsigned int lightID, float intensity);
void wsLightSetIntensityf(unsigned int lightID, float intensity);

void wsLightTogglep(unsigned int lightID, bool turn_on);
void wsLightTogglef(unsigned int lightID, bool turn_on);
void wsLightToggled(unsigned int lightID, bool turn_on);

#endif