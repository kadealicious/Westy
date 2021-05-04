#ifndef LIGHT_H_
#define LIGHT_H_

#include<stdbool.h>
#include"globals.h"

typedef struct wsPointLight {
	vec3 position;
	vec3 color, ambient, diffuse, specular;
	float constant, linear, quadratic;
} wsPointLight;
typedef struct wsSpotLight {
	vec3 position;
	vec3 rotation;
	vec3 color, ambient, diffuse, specular;
	float constant, linear, quadratic;
	float cutoff;
	float outer_cutoff;
} wsSpotLight;
typedef struct wsDirectionLight {
	vec3 rotation;
	vec3 color, ambient, diffuse, specular;
} wsDirectionLight;

// Intensity is a float from 0-1.  Can't guarantee what happens after 1.
void wsLightQuickInitp(wsPointLight *light_source, vec3 position, vec3 color, float intensity);
// Intensity is a float from 0-1.  Spread is the outer cutoff in degrees.
void wsLightQuickInitf(wsSpotLight *light_source, vec3 position, vec3 rotation, vec3 color, float intensity, unsigned int spread);
// Intensity is a float from 0-1.
void wsLightQuickInitd(wsDirectionLight *light_source, vec3 rotation, vec3 color, float intensity);

void wsLightSetPositionp(wsPointLight *light_source, vec3 position);
void wsLightSetPositionf(wsSpotLight *light_source, vec3 position);
void wsLightSetRotationf(wsSpotLight *light_source, vec3 rotation);
void wsLightSetRotationd(wsDirectionLight *light_source, vec3 rotation);

void wsLightSetColorp(wsPointLight *light_source, vec3 color);
void wsLightSetColorf(wsSpotLight *light_source, vec3 color);
void wsLightSetColord(wsDirectionLight *light_source, vec3 color);

void wsLightSetIntensityp(wsPointLight *light_source, float intensity);
void wsLightSetIntensityf(wsSpotLight *light_source, float intensity);

void wsLightTogglep(wsPointLight *light_source, bool turn_on);
void wsLightTogglef(wsSpotLight *light_source, bool turn_on);
void wsLightToggled(wsDirectionLight *light_source, bool turn_on);

#endif