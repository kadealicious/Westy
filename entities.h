#ifndef ENTITIES_H_
#define ENTITIES_H_

#include"globals.h"

void wsEntitiesInit();
void wsEntitiesTerminate();



/* --- NON-PHYSICS OBJECTS --- */

// Pos/rot.
float wsObjectGetPositionX(unsigned int objID);
float wsObjectGetPositionY(unsigned int objID);
float wsObjectGetPositionZ(unsigned int objID);
float wsObjectGetRotationX(unsigned int objID);
float wsObjectGetRotationY(unsigned int objID);
float wsObjectGetRotationZ(unsigned int objID);

bool wsObjectGetVisible(unsigned int objID);// Will return true for visible, false for hidden.
int wsObjectGetMeshID(unsigned int objID);// Will return -INT_MAX (WS_NONE) if the object does not own a mesh.
int wsObjectGetDiffuseID(unsigned int objID);// Will return -INT_MAX (WS_NONE) if the object does not own a diffuse texture.
int wsObjectGetSpecularID(unsigned int objID);// Will return -INT_MAX (WS_NONE) if the object does not own a specular texture.
int wsObjectGetNormalID(unsigned int objID);// Will return -INT_MAX (WS_NONE) if the object does not own a normal texture.

void wsObjectSetPosition(unsigned int objID, vec3 position);
void wsObjectSetRotation(unsigned int objID, vec3 rotation);

void wsObjectSetVisible(unsigned int objID, bool is_visible);
void wsObjectToggleVisible(unsigned int objID);
void wsObjectSetMesh(unsigned int objID, const char *path);
void wsObjectSetDiffuse(unsigned int objID, const char *path);
void wsObjectSetSpecular(unsigned int objID, const char *path);

unsigned int wsObjectInit(vec3 position, vec3 rotation, unsigned int meshID, const char *diffuse_path, const char *specular_path, const char *normal_path);
void wsObjectTerminate(unsigned int creatureID);

/* --- NON-PHYSICS OBJECTS --- */



#endif /* ENTITIES_H_ */