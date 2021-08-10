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
	int normalID[WS_MAX_OBJS];
} wsObjects;
wsObjects objects;

// You know what these do.
float wsObjectGetPositionX(unsigned int objID)	{ return objects.position[objID][X]; }
float wsObjectGetPositionY(unsigned int objID)	{ return objects.position[objID][Y]; }
float wsObjectGetPositionZ(unsigned int objID)	{ return objects.position[objID][Z]; }
float wsObjectGetRotationX(unsigned int objID)	{ return objects.rotation[objID][PITCH]; }
float wsObjectGetRotationY(unsigned int objID)	{ return objects.rotation[objID][YAW]; }
float wsObjectGetRotationZ(unsigned int objID)	{ return objects.rotation[objID][ROLL]; }

int wsObjectGetMeshID(unsigned int objID)		{ return objects.meshID[objID]; }		// Get ID of mesh associated with object objID.
int wsObjectGetDiffuseID(unsigned int objID)	{ return objects.diffuseID[objID]; }	// Get ID of diffuse texture associated with object objID.
int wsObjectGetSpecularID(unsigned int objID)	{ return objects.specularID[objID]; }	// Get ID of specular texture associated with object objID.
int wsObjectGetNormalID(unsigned int objID)		{ return objects.normalID[objID]; }		// Get ID of normal texture associated with object objID.
bool wsObjectGetVisible(unsigned int objID)		{ return objects.is_visible[objID]; }	// Get value of bool is_visible.

void wsObjectSetPosition(unsigned int objID, vec3 position)	{ glm_vec3_copy(position, objects.position[objID]); }	// Set object objID's position.
void wsObjectSetRotation(unsigned int objID, vec3 rotation)	{ glm_vec3_copy(rotation, objects.rotation[objID]); }	// Set object objID's rotation.
void wsObjectSetVisible(unsigned int objID, bool is_visible)	{ objects.is_visible[objID] = is_visible; }			// Set objID's visibility.
void wsObjectToggleVisible(unsigned int objID)	{ objects.is_visible[objID] = !objects.is_visible[objID]; printf("%d\n", objects.is_visible[objID]); }// Toggle objID's visibility.

// Delete old mesh if applicable, then load new one.
void wsObjectSetMesh(unsigned int objID, const char *path) {
	if(wsObjectGetMeshID(objID) != WS_NONE)
		wsMeshDelete(wsObjectGetMeshID(objID));
	objects.meshID[objID] = wsTextureLoad(path, GL_CLAMP_TO_EDGE, WS_TEXTURE_FILTER_STYLE);
}
// Delete old diffuse texture if applicable, then load new one.
void wsObjectSetDiffuse(unsigned int objID, const char *path) {
	if(wsObjectGetDiffuseID(objID) != WS_NONE)
		wsTextureDelete(wsObjectGetDiffuseID(objID));
	objects.diffuseID[objID] = wsTextureLoad(path, GL_CLAMP_TO_EDGE, WS_TEXTURE_FILTER_STYLE);
}
// Delete old specular texture if applicable, then load new one.
void wsObjectSetSpecular(unsigned int objID, const char *path) {
	if(wsObjectGetSpecularID(objID) != WS_NONE)
		wsTextureDelete(wsObjectGetSpecularID(objID));
	objects.specularID[objID] = wsTextureLoad(path, GL_CLAMP_TO_EDGE, WS_TEXTURE_FILTER_STYLE);
}

// Initialize object.
unsigned int wsObjectInit(vec3 position, vec3 rotation, unsigned int meshID, const char *diffuse_path, const char *specular_path, const char *normal_path) {
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
	
	// Load mesh + specified textures.
	wsObjectSetVisible(objID, true);
	objects.meshID[objID] = meshID;
	
	// TODO: ONLY TAKE IN FOLDER PATH AS AN ARGUMENT INSTEAD OF ALL 3 TEXTURE NAMES.
	/* char *diffuse_path;		strcpy(diffuse_path, folder_path);	strncat(diffuse_path, "/diffuse.png", 12);
		char *specular_path;	strcpy(specular_path, folder_path);	strncat(diffuse_path, "/specular.png", 13);
		char *normal_path;		strcpy(normal_path, folder_path);	strncat(diffuse_path, "/normal.png", 11); */
	objects.diffuseID[objID] = wsTextureLoad(diffuse_path, GL_CLAMP_TO_EDGE, WS_TEXTURE_FILTER_STYLE);
	objects.specularID[objID] = wsTextureLoad(specular_path, GL_CLAMP_TO_EDGE, WS_TEXTURE_FILTER_STYLE);
	objects.normalID[objID] = wsTextureLoad(normal_path, GL_CLAMP_TO_EDGE, WS_TEXTURE_FILTER_STYLE);
	
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
	if(objects.normalID[objID] != WS_NONE) {
		wsTextureDelete(objects.normalID[objID]);
		objects.normalID[objID] = WS_NONE;
	}
}
// Free all memory associated with objects.
void wsObjectsTerminate() {
	for(size_t i = 0; i < WS_MAX_OBJS; i++) {
		objects.availableIDs[i] = true;
		objects.num_objs--;
		
		if(objects.meshID[i] != WS_NONE) {
			wsMeshDelete(objects.meshID[i]);
			objects.meshID[i] = WS_NONE;
		}
		if(objects.diffuseID[i] != WS_NONE) {
			wsTextureDelete(objects.diffuseID[i]);
			objects.diffuseID[i] = WS_NONE;
		}
		if(objects.specularID[i] != WS_NONE) {
			wsTextureDelete(objects.specularID[i]);
			objects.specularID[i] = WS_NONE;
		}
		if(objects.normalID[i] != WS_NONE) {
			wsTextureDelete(objects.normalID[i]);
			objects.normalID[i] = WS_NONE;
		}
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
	for(size_t i = 0; i < WS_MAX_OBJS; i++)
		objects.normalID[i] = WS_NONE;
		
	printf("Entities initialized\n");
}
// Free all memory associated with entity manager.
void wsEntitiesTerminate() {
	// Remove objects.
	wsObjectsTerminate();
	
	printf("Entities terminated\n");
}