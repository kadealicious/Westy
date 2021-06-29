#ifndef LOBJ_H_
#define LOBJ_H_

#include<stdio.h>
#include<stdlib.h>

#include"globals.h"

// TODO: Bitwise or to compare these and use them as options for the loader.  I would like to be able to parse .obj files with everything from just verts to verts, normals, texture coords, etc..
#define OJ_MAX_MESHES	50
#define OJ_VERTS		idk_yet
#define OJ_NORMALS		idk_yet
#define OJ_TEX_COORDS	idk_yet

// Structs for easy mesh data manipulation.
typedef float	Vec3[3];
typedef int		Vec3i[3];
typedef float	Vec2[2];
// Holds data for all meshes currently in memory.  Do not interface with this directly.
typedef struct ojMeshes {
	unsigned int num_verts[OJ_MAX_MESHES];	// Number of vertices.
	unsigned int num_faces[OJ_MAX_MESHES];	// Number of faces/indies.
	unsigned int vbo[OJ_MAX_MESHES];		// OpenGL VBO.
	unsigned int vao[OJ_MAX_MESHES];		// OpenGL VAO.
	unsigned int buffer_size[OJ_MAX_MESHES];// Mesh data buffer size.
	unsigned int num_meshes;				// Total # of meshes.
	unsigned int next_meshID;				// Next usable meshID.
	Vec3 *data[OJ_MAX_MESHES];				// Sorted mesh data for each mesh.
} ojMeshes;

// Main instance of ojMeshes.
ojMeshes meshes;

// Stubs.
static void ojInit();
static unsigned int ojLoadOBJ(const char *path);
static void ojGenBuffers(unsigned int meshID);
static void ojDeleteOBJ(unsigned int meshID);

// Getters.
static unsigned int ojGetVAO(unsigned int meshID)			{ return meshes.vao[meshID]; }			// Get meshID's VAO.
static unsigned int ojGetVBO(unsigned int meshID)			{ return meshes.vbo[meshID]; }			// Get meshID's VBO.
static unsigned int ojGetNumVerts(unsigned int meshID)		{ return meshes.num_verts[meshID]; }	// Get meshID's vertex count.
static unsigned int ojGetNumFaces(unsigned int meshID)		{ return meshes.num_faces[meshID]; }	// Get meshID's face count.
static unsigned int ojGetBufferSize(unsigned int meshID)	{ return meshes.buffer_size[meshID]; }	// Get meshID's data buffer size.

// Initialize lobj.  Call this before any mesh initialization.
static void ojInit() {
	meshes.num_meshes	= 0;
	meshes.next_meshID	= 0;
	printf("lobj initialized\n");
}

// Load .obj file into memory.  The return value is used as an ID for the target mesh.
static unsigned int ojLoadOBJ(const char *path) {
	bool verbose = false;
	const unsigned int meshID = meshes.next_meshID;
	
	FILE *file;
	size_t file_size;
	size_t num_lines = 1;
	size_t num_verts = 0;
	size_t num_faces = 0;
	enum FILE_MODES { UNDETERMINED = -1, FACE, FACE_TEX, FACE_NORM, FACE_TEX_NORM };
	int mode = UNDETERMINED;
	
	// Read .obj into obj_source.
	if((file = fopen(path, "rb"))) {
		//Get num_verts, num_faces, num_lines from file.
		char c = fgetc(file);
		while(c != EOF) {
			switch(c) {
				case 'v': 
					num_verts++;
					break;
				case 'f': 
					num_faces++;
					break;
				case '\n': 
					num_lines++;
					break;
			}
			c = fgetc(file);
		}
		meshes.num_verts[meshID]	= num_verts;
		meshes.num_faces[meshID]	= num_faces;
		
		// Get file size.
		fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		fseek(file, 0, SEEK_SET);
	} else return WS_ERROR_FILE;
	
	// Print mesh stats once file loaded.
	if(verbose)
		printf(".obj \"%s\" opened: \n\t~%.2f kb\n\t%d lines\n\t%d vertices\n\t%d faces\n", path, file_size * 0.0009765625f, num_lines, num_verts, num_faces);
	
	// Parse obj_source's data into Meshes struct.
	fseek(file, 0, SEEK_SET);
	char type = '\0';
	float line_data[3];
	
	// Arrays which contain relevant mesh data.  Arrays must be spliced together before the data is finally passed on to struct meshes.
	Vec3 *verts_data		= malloc(sizeof(*verts_data) * num_verts);	// Vertex data.
	Vec3i *faces_data		= malloc(sizeof(*faces_data) * num_faces);	// Face/indices data.
	
	// In case of commented/invalid lines, we must know which line of valid data we are on.
	size_t verts_ndx = 0;
	size_t faces_ndx = 0;
	for(int i = 0; i < num_lines; i++) {
		// If check this before fscanf, else the last line will be left out.
		if(!feof(file)) {
			// Scan line for appropriate data, and store depending on type.
			fscanf(file, "%c %f %f %f\n", &type, &line_data[0], &line_data[1], &line_data[2]);
			switch(type) {
				case 'v': 
					verts_data[verts_ndx][0]	= line_data[0];
					verts_data[verts_ndx][1]	= line_data[1];
					verts_data[verts_ndx][2]	= line_data[2];
					verts_ndx++;
					break;
				case 'f': // Face indices start at 1 instead of 0; subtract 1 from all indices to fix this.
					faces_data[faces_ndx][0]	= (int)line_data[0] - 1;
					faces_data[faces_ndx][1]	= (int)line_data[1] - 1;
					faces_data[faces_ndx][2]	= (int)line_data[2] - 1;
					faces_ndx++;
					break;
				
				case 'o': 
				case 's': 
				case '#': 
				case '\n': 
				case '\0': 
				default: 
					// printf("\"%s\": Unhandled type '%c' on line %d\n", path, type, (i + 1));
					break;
			}
			// printf("%d | v - %d/%d | f - %d/%d\n", i, (verts_ndx / 3), num_verts, (faces_ndx / 3), num_faces);
			// printf("%d | v - %d/%d | f - %d/%d | %c - %f %f %f\n", i, (verts_ndx / 3), num_verts, (faces_ndx / 3), num_faces, type, line_data[0], line_data[1], line_data[2]);
		}
	}
	// Done with the file.
	fclose(file);
	
	/* The fun part...  Organization!
		All .obj files contain a list of faces (indices) that is separate from the actual 
		vertex data.  This must be woven back in to the vertex data, unfortunately.  Whoever 
		designed wavefront files to do this probably had a good reason for this, but it's 
		inconvenient for me and therefore I don't like it.  */
	// Allocate enough memory for a (num_faces * 3) number of Vec3's.
	meshes.buffer_size[meshID]	= sizeof(*meshes.data[0]) * (num_faces * 3);
	meshes.data[meshID]			= malloc(meshes.buffer_size[meshID]);
	Vec3 *cur_mesh_data			= meshes.data[meshID];
	
	size_t mesh_data_ndx = 0;// Used to find where we are in the actual mesh data, since each face will take up 3 vertex slots in memory.
	for(int i = 0; i < num_faces; i++) {
		for(unsigned int j = 0; j < 3; j++) {
			// For every face, store the 3 corresponding vertices in the mesh data. 
			cur_mesh_data[mesh_data_ndx + j][0]	= verts_data[faces_data[i][j]][0];
			cur_mesh_data[mesh_data_ndx + j][1]	= verts_data[faces_data[i][j]][1];
			cur_mesh_data[mesh_data_ndx + j][2]	= verts_data[faces_data[i][j]][2];
		}
		mesh_data_ndx += 3;
	}
	
	// Garbage collection.
	free(verts_data);
	free(faces_data);
	
	// Generate our OpenGL VBO and VAO.
	ojGenBuffers(meshID);
	
	// Yay!
	printf("\'%s\' load success\n", path);
	
	meshes.num_meshes++;
	meshes.next_meshID++;
	return meshID;
}

// Generate OpenGL VAO and VBO for the given mesh.  This is automatically called in ojLoadOBJ().
static void ojGenBuffers(unsigned int meshID) {
	glGenBuffers(1, &meshes.vbo[meshID]);
	glBindBuffer(GL_ARRAY_BUFFER, meshes.vbo[meshID]);
	glBufferData(GL_ARRAY_BUFFER, meshes.buffer_size[meshes.num_meshes], meshes.data[meshID], GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &meshes.vao[meshID]);
	glBindVertexArray(meshes.vao[meshID]);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

// Remove mesh from memory.  Must be called for every mesh.
static void ojDeleteOBJ(unsigned int meshID) {
	meshes.num_verts[meshID] = 0;
	meshes.num_faces[meshID] = 0;
	meshes.num_meshes--;
	free(meshes.data[meshID]);// TODO: Only delete meshID's data.  Still need to figure that one out.  Save that one for a rainy day.  Or for whenever I figure the rest of this shit out.
}

// Remove all meshes from memory.  Always call this at when lobj is no longer needed.
static void ojTerminate() {
	// Double frees lead to undefined behavior and I don't like that.  I'd rather do this.
	bool is_empty = true;
	for(int i = 0; i < OJ_MAX_MESHES; i++) {
		if(meshes.num_verts[i] != 0) {
			is_empty = false;
			break;
		}
	}
	if(!is_empty)
		free(meshes.data);
}

#endif // LOBJ_H_