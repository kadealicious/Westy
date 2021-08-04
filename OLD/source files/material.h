#ifndef MATERIAL_H_
#define MATERIAL_H_

#include"globals.h"

typedef struct {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} wsMaterial;

typedef struct {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} wsEmitMaterial;

void wsMaterialInit(wsMaterial *material, vec3 ambient, vec3 diffuse, vec3 specular, float shininess);

// Tester materials.
void wsMaterialMakeRainbow(wsMaterial *material);// Fun material colors.  :)
void wsMaterialMakeEmerald(wsMaterial *material);
void wsMaterialMakePlasticCyan(wsMaterial *material);
void wsMaterialMakeRubberWhite(wsMaterial *material);

#endif