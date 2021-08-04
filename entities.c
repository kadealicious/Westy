#include"entities.h"
#include"lobj.h"
#include"texturing.h"

/* --- NON-PHYSICS OBJECTS --- */
typedef struct wsObjects {
	vec3 position[WS_MAX_OBJS];
	vec3 rotation[WS_MAX_OBJS];
	
	unsigned int num_objs;
	bool availableIDs[WS_MAX_TEXTURES];
	bool is_visible[WS_MAX_TEXTURES];
	int meshID[WS_MAX_OBJS];
	int diffuseID[WS_MAX_OBJS];
	int specularID[WS_MAX_OBJS];
} wsObjects;
wsObjects objects;

// Get ID of mesh associated with object objID.
int wsObjectGetMeshID(unsigned int objID)		{ return objects.meshID[objID]; }

// Get ID of diffuse texture associated with object objID.
int wsObjectGetDiffuseID(unsigned int objID)	{ return objects.diffuseID[objID]; }

// Get ID of specular texture associated with object objID.
int wsObjectGetSpecularID(unsigned int objID)	{ return objects.specularID[objID]; }

// Get value of bool is_visible.
bool wsObjectGetVisible(unsigned int objID)	{ return objects.is_visible[objID]; }



// Set object objID's position.
void wsObjectSetPosition(unsigned int objID, vec3 position)	{ glm_vec3_copy(position, objects.position[objID]); }

// Set object objID's rotation.
void wsObjectSetRotation(unsigned int objID, vec3 rotation)	{ glm_vec3_copy(rotation, objects.rotation[objID]); }

// Set objID's visibility.
void wsObjectSetVisible(unsigned int objID, bool is_visible)	{ objects.is_visible[objID] = is_visible; }

// Toggle objID's visibility.
void wsObjectToggleVisible(unsigned int objID)	{ objects.is_visible[objID] = !objects.is_visible[objID]; printf("%d\n", objects.is_visible[objID]); }

// Delete old mesh if applicable, then load new one.
void wsObjectSetMesh(unsigned int objID, const char *path) {
	if(wsObjectGetMeshID(objID) != WS_NONE)
		wsMeshDelete(wsObjectGetMeshID(objID));
	objects.meshID[objID] = wsTextureLoad(path, GL_CLAMP_TO_EDGE, GL_NEAREST);
}

// Delete old diffuse texture if applicable, then load new one.
void wsObjectSetDiffuse(unsigned int objID, const char *path) {
	if(wsObjectGetDiffuseID(objID) != WS_NONE)
		wsTextureDelete(wsObjectGetDiffuseID(objID));
	objects.diffuseID[objID] = wsTextureLoad(path, GL_CLAMP_TO_EDGE, GL_NEAREST);
}

// Delete old specular texture if applicable, then load new one.
void wsObjectSetSpecular(unsigned int objID, const char *path) {
	if(wsObjectGetSpecularID(objID) != WS_NONE)
		wsTextureDelete(wsObjectGetSpecularID(objID));
	objects.specularID[objID] = wsTextureLoad(path, GL_CLAMP_TO_EDGE, GL_NEAREST);
}

// Initialize object.
unsigned int wsObjectInit(vec3 position, vec3 rotation, const char *mesh_path, uint8_t mesh_flags, const char *diffuse_path, const char *specular_path) {
	// Get a valid ID for this object.
	unsigned int objID;
	for(size_t i = 0; i < WS_MAX_OBJS; i++) {
		if(objects.availableIDs[i] == true) {
			objID = i;
			objects.availableIDs[i] = false;
			break;
		}
	}
	
	// Set position/rotation.
	wsObjectSetPosition(objID, position);
	wsObjectSetRotation(objID, rotation);
	wsObjectSetVisible(objID, true);
	
	// Load mesh + specified textures.
	objects.meshID[objID] = wsMeshLoadOBJ(mesh_path, mesh_flags);
	objects.diffuseID[objID] = wsTextureLoad(diffuse_path, GL_CLAMP_TO_EDGE, GL_NEAREST);// TODO: FIX TEXTURING THROUGH THE TEXTURING INTERFACE.
	objects.specularID[objID] = wsTextureLoad(specular_path, GL_CLAMP_TO_EDGE, GL_NEAREST);
	
	objects.num_objs++;
	return objID;
}
// Free all memory associated with object objID.
void wsObjectTerminate(unsigned int objID) {
	objects.availableIDs[objID] = true;
	objects.num_objs--;
	
	if(objects.meshID[objID] != WS_NONE) {
		wsMeshDelete(objects.meshID[objID]);
		objects.meshID[objID] = WS_NONE;
	}
	if(objects.diffuseID[objID] != WS_NONE) {
		wsTextureDelete(objects.diffuseID[objID]);
		objects.diffuseID[objID] = WS_NONE;
	}
	if(objects.specularID[objID] != WS_NONE) {
		wsTextureDelete(objects.specularID[objID]);
		objects.specularID[objID] = WS_NONE;
	}
}
/* --- NON-PHYSICS OBJECTS --- */

// General entity handling.

// Initialize entity manager.
void wsEntitiesInit() {
	// Non-physics objects.
	objects.num_objs = 0;
	for(size_t i = 0; i < WS_MAX_OBJS; i++)
		objects.availableIDs[i] = true;
	for(size_t i = 0; i < WS_MAX_OBJS; i++)
		objects.meshID[i] = WS_NONE;
	for(size_t i = 0; i < WS_MAX_OBJS; i++)
		objects.diffuseID[i] = WS_NONE;
	for(size_t i = 0; i < WS_MAX_OBJS; i++)
		objects.specularID[i] = WS_NONE;
		
	printf("Entities initialized\n");
}
// Free all memory associated with entity manager.
void wsEntitiesTerminate() {
	// Remove objects.
	for(size_t i = 0; i < WS_MAX_OBJS; i++)
		wsObjectTerminate(i);
	
	printf("Entities terminated\n");
}