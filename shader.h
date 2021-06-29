#ifndef SHADERS_H_
#define SHADERS_H_

#include<stdbool.h>

#include"globals.h"
#include"camera.h"
#include"light.h"

// Load shader.
int wsShaderLoad(const char *path_vert, const char *path_frag, bool use_on_load, bool print_on_load);
// Use shader.
void wsShaderUse(unsigned int shaderID);
// Get active shader.
unsigned int wsShaderGetActive();
// Used for resetting shader variables to -1 (WS_SHADER_NONE).
void wsShaderDelete(unsigned int *shaderID);


// Set shader bool.
void wsShaderSetBool(unsigned int shaderID, const char *var_name, bool value);
// Set shader int.
void wsShaderSetInt(unsigned int shaderID, const char *var_name, int value);
// Set shader float.
void wsShaderSetFloat(unsigned int shaderID, const char *var_name, float value);
// Set shader vec2: float[2].
void wsShaderSetVec2(unsigned int shaderID, const char *var_name, vec2 vec);
// Set shader vec2: float[3].
void wsShaderSetVec3(unsigned int shaderID, const char *var_name, vec3 vec);
// Set shader vec2: float[4].
void wsShaderSetVec4(unsigned int shaderID, const char *var_name, vec4 vec);
// Set shader mat3: float[3][3].
void wsShaderSetMat3(unsigned int shaderID, const char *var_name, mat3 *mat);
// Set shader mat4: float[4][4].
void wsShaderSetMat4(unsigned int shaderID, const char *var_name, mat4 *mat);

// Update shader's view_position.
void wsShaderUpdateCamera(unsigned int shaderID, unsigned int cameraID);
// Update shader's model, view, projection matrices.
void wsShaderSetMVP(unsigned int shaderID, mat4 *model, mat4 *view, mat4 *projection);
// Update shader's matrix_normal.
void wsShaderSetNormalMatrix(unsigned int shaderID, mat4 *model);

// Update shader with pointlight info.
void wsShaderUpdateLightp(unsigned int shaderID, unsigned int lightID);
// Update shader with spotlight info.
void wsShaderUpdateLightf(unsigned int shaderID, unsigned int lightID);
// Update shader with directional light info.
void wsShaderUpdateLightd(unsigned int shaderID, unsigned int lightID);

#endif