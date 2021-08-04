#ifndef ENTITIES_H_
#define ENTITIES_H_

#include"globals.h"

void wsEntitiesInit();
void wsEntitiesTerminate();



/* --- NON-PHYSICS OBJECTS --- */

int wsObjectGetMeshID(unsigned int objID);// Will return -INT_MAX (WS_NONE) if the object does not own a mesh.
int wsObjectGetDiffuseID(unsigned int objID);// Will return -INT_MAX (WS_NONE) if the object does not own a diffuse texture.
int wsObjectGetSpecularID(unsigned int objID);// Will return -INT_MAX (WS_NONE) if the object does not own a diffuse texture.
bool wsObjectGetVisible(unsigned int objID);// Will return true for visible, false for hidden.

void wsObjectSetPosition(unsigned int objID, vec3 position);
void wsObjectSetRotation(unsigned int objID, vec3 rotation);

void wsObjectSetVisible(unsigned int objID, bool is_visible);
void wsObjectToggleVisible(unsigned int objID);

void wsObjectSetMesh(unsigned int objID, const char *path);
void wsObjectSetDiffuse(unsigned int objID, const char *path);
void wsObjectSetSpecular(unsigned int objID, const char *path);

unsigned int wsObjectInit(vec3 position, vec3 rotation, const char *mesh_path, uint8_t mesh_flags, const char *diffuse_path, const char *specular_path);
void wsObjectTerminate(unsigned int creatureID);

/* --- NON-PHYSICS OBJECTS --- */



#endif /* ENTITIES_H_ */