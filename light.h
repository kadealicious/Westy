#ifndef LIGHT_H_
#define LIGHT_H_

#include<stdbool.h>
#include"globals.h"

#define WS_MAX_POINTLIGHTS 4
#define WS_MAX_SPOTLIGHTS 1
#define WS_MAX_DIRECTIONLIGHTS 1

typedef struct wsPointLights {
	bool toggle[WS_MAX_POINTLIGHTS];
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
	bool toggle[WS_MAX_SPOTLIGHTS];
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
	bool toggle[WS_MAX_DIRECTIONLIGHTS];
	vec3 rotation[WS_MAX_DIRECTIONLIGHTS];
	vec3 color[WS_MAX_DIRECTIONLIGHTS];
	vec3 ambient[WS_MAX_DIRECTIONLIGHTS];
	vec3 diffuse[WS_MAX_DIRECTIONLIGHTS];
	vec3 specular[WS_MAX_DIRECTIONLIGHTS];
} wsDirectionLights;

wsPointLights pointlights;
wsSpotLights spotlights;
wsDirectionLights directionlights;

// Intensity is a float from 0-1.  Can't guarantee what happens after 1, although sometimes it looks pretty cool.
void wsLightQuickInitp(unsigned int lightID, vec3 position, vec3 color, float intensity, float size, bool on);
// Intensity is a float from 0-1.  Spread is the outer cutoff in degrees.
void wsLightQuickInitf(unsigned int lightID, vec3 position, vec3 rotation, vec3 color, float intensity, float size, unsigned int spread, bool on);
// Intensity is a float from 0-1.
void wsLightQuickInitd(unsigned int lightID, vec3 rotation, vec3 color, float intensity, bool on);

void wsLightSetPositionp(unsigned int lightID, vec3 position);
void wsLightSetPositionf(unsigned int lightID, vec3 position);
void wsLightSetRotationf(unsigned int lightID, vec3 rotation);
void wsLightSetRotationd(unsigned int lightID, vec3 rotation);

void wsLightSetColorp(unsigned int lightID, vec3 color);
void wsLightSetColorf(unsigned int lightID, vec3 color);
void wsLightSetColord(unsigned int lightID, vec3 color);

void wsLightSetSizep(unsigned int lightID, float size);
void wsLightSetSizef(unsigned int lightID, float size);

void wsLightSetIntensityp(unsigned int lightID, float intensity);
void wsLightSetIntensityf(unsigned int lightID, float intensity);
void wsLightSetIntensityd(unsigned int lightID, float intensity);

void wsLightTogglep(unsigned int lightID, bool on);
void wsLightTogglef(unsigned int lightID, bool on);
void wsLightToggled(unsigned int lightID, bool on);

bool wsLightGetTogglep(unsigned int lightID);
bool wsLightGetTogglef(unsigned int lightID);
bool wsLightGetToggled(unsigned int lightID);

#endif