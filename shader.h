#ifndef SHADERS_H_
#define SHADERS_H_

#include<stdbool.h>

#include"globals.h"
#include"camera.h"
#include"light.h"

unsigned int wsShaderLoad(const char *path_vert, const char *path_frag, bool use_on_load, bool print_on_load);
void wsShaderUse(unsigned int shaderID);
void wsShaderSetBool(unsigned int shaderID, const char *var_name, bool value);
void wsShaderSetInt(unsigned int shaderID, const char *var_name, int value);
void wsShaderSetFloat(unsigned int shaderID, const char *var_name, float value);
void wsShaderSetVec2(unsigned int shaderID, const char *var_name, vec2 vec);
void wsShaderSetVec3(unsigned int shaderID, const char *var_name, vec3 vec);
void wsShaderSetVec4(unsigned int shaderID, const char *var_name, vec4 vec);
void wsShaderSetMat3(unsigned int shaderID, const char *var_name, mat3 *mat);
void wsShaderSetMat4(unsigned int shaderID, const char *var_name, mat4 *mat);

void wsShaderUpdateCamera(unsigned int shaderID, wsCamera *camera);
void wsShaderSetMVP(unsigned int shaderID, mat4 *model, mat4 *view, mat4 *projection);
void wsShaderSetNormalMatrix(unsigned int shaderID, mat4 *model);

void wsShaderUpdateLights(unsigned int shaderID, wsPointLight *pointlight_arr_start, unsigned int num_pointlights, wsSpotLight *spotlight, wsDirectionLight *directionlight);
void wsShaderUpdateLightsp(unsigned int shaderID, wsPointLight *pointlight_arr_start, unsigned int num_pointlights);
void wsShaderUpdateLightp(unsigned int shaderID, wsPointLight *light_source, unsigned int pointlight_ndx);
void wsShaderUpdateLightf(unsigned int shaderID, wsSpotLight *light_source);
void wsShaderUpdateLightd(unsigned int shaderID, wsDirectionLight *light_source);

#endif