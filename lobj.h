#ifndef LOBJ_H_
#define LOBJ_H_

#include<stdio.h>
#include<stdlib.h>

#include"globals.h"

// TODO: Bitwise or to compare these and use them as options for the loader.  I would like to be able to parse .obj files with everything from just verts to verts, normals, texture coords, etc..
#define WS_MAX_MESHES			50

#define WS_FACES		0x01
#define WS_TEX			0x02
#define WS_NORMALS		0x04

// Structs for easy mesh data manipulation.
typedef float	Vec3[3];
typedef int		Vec3i[3];
typedef float	Vec2[2];
// Holds data for all meshes currently in memory.  Do not interface with this directly.
typedef struct wsObjMeshes {
	unsigned int num_verts[WS_MAX_MESHES];	// Number of vertices.
	unsigned int num_faces[WS_MAX_MESHES];	// Number of faces/indies.
	unsigned int vbo[WS_MAX_MESHES];		// OpenGL VBO.
	unsigned int vao[WS_MAX_MESHES];		// OpenGL VAO.
	unsigned int buffer_size[WS_MAX_MESHES];// Mesh data buffer size.
	unsigned int num_meshes;				// Total # of meshes.
	unsigned int next_meshID;				// Next usable meshID.
	Vec3 *data[WS_MAX_MESHES];				// Sorted mesh data for each mesh.
} wsObjMeshes;

// Main instance of wsObjMeshes.
wsObjMeshes meshes;

// Stubs.
static void wsObjInit();
static int wsObjLoad(const char *path, uint8_t flags);
static void wsObjGenBuffers(unsigned int meshID, uint8_t flags);
static void wsObjDelete(unsigned int meshID);
static void wsObjTerminate();

// Getters.
static unsigned int wsObjGetVAO(unsigned int meshID)		{ return meshes.vao[meshID]; }			// Get meshID's VAO.
static unsigned int wsObjGetVBO(unsigned int meshID)		{ return meshes.vbo[meshID]; }			// Get meshID's VBO.
static unsigned int wsObjGetNumVerts(unsigned int meshID)	{ return meshes.num_verts[meshID]; }	// Get meshID's vertex count.
static unsigned int wsObjGetNumFaces(unsigned int meshID)	{ return meshes.num_faces[meshID]; }	// Get meshID's face count.
static unsigned int wsObjGetBufferSize(unsigned int meshID)	{ return meshes.buffer_size[meshID]; }	// Get meshID's data buffer size.
static unsigned int wsObjGetNumMeshes()						{ return meshes.num_meshes; }			// Get the total number of meshes.

// Initialize lobj.  Call this before any mesh initialization.
static void wsObjInit() {
	meshes.num_meshes	= 0;
	meshes.next_meshID	= 0;
	printf("lobj initialized\n");
}

// Normalizes Vec3 v.

static void wsObjNormalizeV(Vec3 *v) {
	// TODO: Scale vertex normal coords like so: vn * (1.0f / sqrt(dot(vn, vn)))).
	Vec3 v_norm;
	v_norm[0] = *v[0];
	v_norm[1] = *v[1];
	v_norm[2] = *v[2];
	
	float v_norm_dot = (v_norm[0] * v_norm[0]) * (v_norm[1] * v_norm[1]) + (v_norm[2] * v_norm[2]);
	v_norm[0] /= v_norm_dot;
	v_norm[1] /= v_norm_dot;
	v_norm[2] /= v_norm_dot;
}
static void wsObjNormalizeVi(Vec3i *v) {
	// TODO: Scale vertex normal coords like so: vn * (1.0f / sqrt(dot(vn, vn)))).
	Vec3 v_norm;
	v_norm[0] = *v[0];
	v_norm[1] = *v[1];
	v_norm[2] = *v[2];
	
	float v_norm_dot = (v_norm[0] * v_norm[0]) * (v_norm[1] * v_norm[1]) + (v_norm[2] * v_norm[2]);
	v_norm[0] /= v_norm_dot;
	v_norm[1] /= v_norm_dot;
	v_norm[2] /= v_norm_dot;
}
/* Load .obj file into memory.  The return value is used as an ID for the target mesh.
	
	Flags are: 
	WS_FACES	- File contains face indices.
	WS_TEX		- File contains texture coordinates.
	WS_NORMALS	- File contains vertex normal data.
 */
static int wsObjLoad(const char *path, uint8_t flags) {
	bool verbose = true;
	const unsigned int meshID = meshes.next_meshID;
	
	FILE *file;
	size_t file_size;
	size_t num_lines			= 1;
	size_t num_verts			= 0;
	size_t num_vert_normals		= 0;
	size_t num_vert_tex_coords	= 0;
	size_t num_faces			= 0;
	
	// Read .obj into obj_source.
	if((file = fopen(path, "rb"))) {
		//Get num_verts, num_faces, num_lines from file.
		char c = fgetc(file);
		while(c != EOF) {
			switch(c) {
				case 'v': 
					if((flags & WS_NORMALS) || (flags & WS_TEX)) {
						c = fgetc(file);
						switch(c) {
							case 'n': 
								num_vert_normals++;
								break;
							case 't': 
								num_vert_tex_coords++;
								break;
							case ' ': 
							default: 
								num_verts++;
								break;
						}
					} else num_verts++;
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
	if(verbose) {
		printf(".obj \"%s\" opened: \n", path);
		printf("\t~%.2f kb\n", file_size * 0.0009765625f);
		printf("\t%d lines\n", num_lines);
		printf("\t%d vertices\n", num_verts);
		if(flags & WS_NORMALS)
			printf("\t%d vertex normals\n", num_vert_normals);
		if(flags & WS_TEX)
			printf("\t%d vertex texture coords\n", num_vert_tex_coords);
		printf("\t%d faces\n", num_faces);
	}
	
	// Parse obj_source's data into Meshes struct.
	fseek(file, 0, SEEK_SET);
	
	/* ---------- JUST FACES ---------- */
	/* ---------- JUST FACES ---------- */
	/* ---------- JUST FACES ---------- */
	if(!(flags & WS_NORMALS) && !(flags & WS_TEX)) {
		// Storage of line data.
		char type = '\0';
		float line_data[3];
	
		// Arrays which contain relevant mesh data.  Arrays must be spliced together before the data is finally passed on to struct meshes.
		Vec3 *verts_data		= malloc(sizeof(*verts_data) * num_verts);		// Vertex data.
		Vec3i *indices_data		= malloc(sizeof(*indices_data) * num_faces);	// Face/indices data.
		
		// Keep track of the index of each type of coordinate.
		size_t verts_ndx = 0;
		size_t faces_ndx = 0;
		for(int i = 0; i < num_lines; i++) {
			// Check if EOF has been reached before calling fscanf(), otherwise the last line of the file will be discarded prematurely.
			if(!feof(file)) {
				/* Scan line for appropriate data, and store depending on type.
					Storage for a full line will look like this: vx vy vz vtu vtv vtw vnx vny vnz, with vn or vt directly following vxyz in the other's absense. */
				fscanf(file, "%c %f %f %f\n", &type, &line_data[0], &line_data[1], &line_data[2]);
				switch(type) {
					case 'v': 
						verts_data[verts_ndx][0]	= line_data[0];
						verts_data[verts_ndx][1]	= line_data[1];
						verts_data[verts_ndx][2]	= line_data[2];
						verts_ndx++;
						break;
					case 'f': // Face indices start at 1 instead of 0; subtract 1 from all indices to fix this.
						indices_data[faces_ndx][0]	= (int)line_data[0] - 1;
						indices_data[faces_ndx][1]	= (int)line_data[1] - 1;
						indices_data[faces_ndx][2]	= (int)line_data[2] - 1;
						faces_ndx++;
						break;
					
					default: 
						break;
				}
			}
		}
		// Done with the file.
		fclose(file);
		
		/* The fun part...  Organization!
			All .obj files contain a list of faces (indices) that is separate from the actual 
			vertex data.  This must be woven back in to the vertex data, unfortunately.  Whoever 
			designed wavefront files to do this probably had a good reason for this, but it's 
			inconvenient for me and therefore I don't like it.  */
		// Store 3 Vec3 vertices for each face.
		const size_t mesh_data_stride = 3;
		meshes.buffer_size[meshID]	= sizeof(*meshes.data[0]) * (num_faces * mesh_data_stride);
		meshes.data[meshID]			= malloc(meshes.buffer_size[meshID]);
		Vec3 *cur_mesh_data			= meshes.data[meshID];
		
		size_t mesh_data_ndx = 0;// Used to find where we are in the actual mesh data, since each face will take up 3 vertex slots in memory.
		for(int i = 0; i < num_faces; i++) {
			for(unsigned int j = 0; j < 3; j++) {
				// For every face, store the 3 corresponding vertices in the mesh data. 
				cur_mesh_data[mesh_data_ndx + j][0]	= verts_data[indices_data[i][j]][0];
				cur_mesh_data[mesh_data_ndx + j][1]	= verts_data[indices_data[i][j]][1];
				cur_mesh_data[mesh_data_ndx + j][2]	= verts_data[indices_data[i][j]][2];
			}
			mesh_data_ndx += mesh_data_stride;
		}
		
		// Garbage collection.
		free(verts_data);
		free(indices_data);
		
		/* ---------- FACES AND NORMALS ---------- */
		/* ---------- FACES AND NORMALS ---------- */
		/* ---------- FACES AND NORMALS ---------- */
	} else if((flags & WS_NORMALS) && !(flags & WS_TEX)) {
		char type[2] = {'\0', '\0'};
		float line_data[6] = {0.0f};
		
		size_t index_stride = 2;
		Vec3 *verts_data		= malloc(sizeof(*verts_data) * num_verts);					// Vertex data.
		Vec3 *vert_normals_data	= malloc(sizeof(*vert_normals_data) * num_vert_normals);	// Vertex normals data.
		Vec3i *indices_data		= malloc((sizeof(*indices_data) * (num_faces * index_stride)));		// Face/indices data.
		
		size_t verts_ndx = 0;
		size_t vert_normals_ndx = 0;
		size_t faces_ndx = 0;
		for(int i = 0; i < num_lines; i++) {
			if(!feof(file)) {
				// Make sure line is not a comment/of an unrecognized type.
				// Because vertex and face lines will not appear the same, the fscanf() arguments need to change.
				fscanf(file, "%c%c ", &type[0], &type[1]);
				switch(type[0]) {
					case 'v': 
						fscanf(file, "%f %f %f\n", &line_data[0], &line_data[1], &line_data[2]);
						switch(type[1]) {
							case 'n': 
								vert_normals_data[vert_normals_ndx][0]	= line_data[0];
								vert_normals_data[vert_normals_ndx][1]	= line_data[1];
								vert_normals_data[vert_normals_ndx][2]	= line_data[2];
								vert_normals_ndx++;
								break;
							case ' ': 
							default: 
								verts_data[verts_ndx][0]	= line_data[0];
								verts_data[verts_ndx][1]	= line_data[1];
								verts_data[verts_ndx][2]	= line_data[2];
								verts_ndx++;
								break;
						}
						break;
					case 'f': 
						// Face indices start at 1 instead of 0; subtract 1 from all indices to fix this.
						fscanf(file, "%f//%f %f//%f %f//%f\n", &line_data[0], &line_data[1], &line_data[2], &line_data[3], &line_data[4], &line_data[5]);
						
						// Vertex indices.
						indices_data[faces_ndx][0]	= (int)line_data[0] - 1;
						indices_data[faces_ndx][1]	= (int)line_data[2] - 1;
						indices_data[faces_ndx][2]	= (int)line_data[4] - 1;
						// Vertex normal indices.
						indices_data[faces_ndx + 1][0]	= (int)line_data[1] - 1;
						indices_data[faces_ndx + 1][1]	= (int)line_data[3] - 1;
						indices_data[faces_ndx + 1][2]	= (int)line_data[5] - 1;
						wsObjNormalizeVi(&indices_data[faces_ndx + 1]);
						
						faces_ndx += index_stride;
						break;
					
					default: 
						// Consume the rest of the line so we have no garbage to deal with.
						while(type[0] != '\n') { type[0] = fgetc(file); }
						break;
				}
				// printf("[%c%c] - %f %f %f %f %f %f\n", type[0], type[1], line_data[0], line_data[1], line_data[2], line_data[3], line_data[4], line_data[5]);
			}
		}
		fclose(file);
		
		// Store 6 Vec3 vertices/normals for each face.
		const size_t mesh_data_stride = 6;
		meshes.buffer_size[meshID]	= sizeof(*meshes.data[0]) * (num_faces * mesh_data_stride);
		meshes.data[meshID]			= malloc(meshes.buffer_size[meshID]);
		Vec3 *cur_mesh_data			= meshes.data[meshID];
		
		size_t mesh_data_ndx = 0;
		for(int i = 0; i < num_faces; i++) {
			for(unsigned int j = 0; j < 3; j++) {
				// Vertices.
				cur_mesh_data[mesh_data_ndx + (j * index_stride)][0]	= verts_data[indices_data[i * index_stride][j]][0];
				cur_mesh_data[mesh_data_ndx + (j * index_stride)][1]	= verts_data[indices_data[i * index_stride][j]][1];
				cur_mesh_data[mesh_data_ndx + (j * index_stride)][2]	= verts_data[indices_data[i * index_stride][j]][2];
				// Normals.
				cur_mesh_data[mesh_data_ndx + (j * index_stride) + 1][0]	= vert_normals_data[indices_data[(i * index_stride) + 1][j]][0];
				cur_mesh_data[mesh_data_ndx + (j * index_stride) + 1][1]	= vert_normals_data[indices_data[(i * index_stride) + 1][j]][1];
				cur_mesh_data[mesh_data_ndx + (j * index_stride) + 1][2]	= vert_normals_data[indices_data[(i * index_stride) + 1][j]][2];
			}
			mesh_data_ndx += mesh_data_stride;
		}
		
		free(verts_data);
		free(vert_normals_data);
		free(indices_data);
		
		/* ---------- FACES, NORMALS, UV COORDS ---------- */
		/* ---------- FACES, NORMALS, UV COORDS ---------- */
		/* ---------- FACES, NORMALS, UV COORDS ---------- */
	}/* else if(!(flags & WS_NORMALS) && (flags & WS_TEX)) {// TODO: Fix this shit.
		fscanf(file, "%c%c %f/%f %f/%f %f/%f\n", &type[0], &type[1], &line_data[0], &line_data[3], &line_data[2], &line_data[4], &line_data[3], &line_data[5]);
	}*/ else if((flags & WS_NORMALS) && (flags & WS_TEX)) {
		char type[2] = {'\0', '\0'};
		float line_data[9] = {0.0f};
		
		size_t index_stride = 3;
		Vec3 *verts_data		= malloc(sizeof(*verts_data) * num_verts);						// Vertex data.
		Vec3 *vert_normals_data	= malloc(sizeof(*vert_normals_data) * num_vert_normals);		// Vertex normals data.
		Vec2 *vert_tex_data		= malloc(sizeof(*vert_tex_data) * num_vert_tex_coords);			// Vertex texture data.
		Vec3i *indices_data		= malloc((sizeof(*indices_data) * (num_faces * index_stride)));	// Face/indices data.
		
		size_t verts_ndx = 0;
		size_t vert_normals_ndx = 0;
		size_t vert_tex_ndx = 0;
		size_t faces_ndx = 0;
		for(int i = 0; i < num_lines; i++) {
			if(!feof(file)) {
				// Make sure line is not a comment/of an unrecognized type.
				// Because vertex and face lines are not similarly formatted, the fscanf() arguments need to change.
				fscanf(file, "%c%c ", &type[0], &type[1]);
				switch(type[0]) {
					case 'v': 
						fscanf(file, "%f %f %f\n", &line_data[0], &line_data[1], &line_data[2]);
						// printf("[%c%c] - %f %f %f\n", type[0], type[1], line_data[0], line_data[1], line_data[2]);
						switch(type[1]) {
							case 'n': 
								vert_normals_data[vert_normals_ndx][0]	= line_data[0];
								vert_normals_data[vert_normals_ndx][1]	= line_data[1];
								vert_normals_data[vert_normals_ndx][2]	= line_data[2];
								vert_normals_ndx++;
								break;
							case 't': 
								vert_tex_data[vert_tex_ndx][0]	= line_data[0];
								vert_tex_data[vert_tex_ndx][1]	= line_data[1];
								vert_tex_ndx++;
								break;
							case ' ': 
							default: 
								verts_data[verts_ndx][0]	= line_data[0];
								verts_data[verts_ndx][1]	= line_data[1];
								verts_data[verts_ndx][2]	= line_data[2];
								verts_ndx++;
								break;
						}
						break;
					case 'f': 
						// Face indices start at 1 instead of 0; subtract 1 from all indices to fix this.
						fscanf(file, "%f/%f/%f %f/%f/%f %f/%f/%f\n", &line_data[0], &line_data[1], &line_data[2], &line_data[3], &line_data[4], &line_data[5], &line_data[6], &line_data[7], &line_data[8]);
						// printf("[%c%c] - %f/%f/%f %f/%f/%f %f/%f/%f\n", type[0], type[1], line_data[0], line_data[1], line_data[2], line_data[3], line_data[4], line_data[5], line_data[6], line_data[7], line_data[8]);
						
						// Vertex indices.
						indices_data[faces_ndx][0]	= (int)line_data[0] - 1;
						indices_data[faces_ndx][1]	= (int)line_data[3] - 1;
						indices_data[faces_ndx][2]	= (int)line_data[6] - 1;
						// Vertex normal indices.
						indices_data[faces_ndx + 1][0]	= (int)line_data[2] - 1;
						indices_data[faces_ndx + 1][1]	= (int)line_data[5] - 1;
						indices_data[faces_ndx + 1][2]	= (int)line_data[8] - 1;
						wsObjNormalizeVi(&indices_data[faces_ndx + 1]);
						// Vertex texture indices.
						indices_data[faces_ndx + 2][0]	= (int)line_data[1] - 1;
						indices_data[faces_ndx + 2][1]	= (int)line_data[4] - 1;
						indices_data[faces_ndx + 2][2]	= (int)line_data[7] - 1;
						
						faces_ndx += index_stride;
						break;
					
					default: 
						// Consume the rest of the line so we have no garbage to deal with.
						while(type[0] != '\n') { type[0] = fgetc(file); }
						break;
				}
			}
		}
		fclose(file);
		
		// Store 6 Vec3 vertices/normals for each face.
		const size_t mesh_data_stride = 9;// Stride is 8 because UV coords only have 2 data points instead of 3.
		meshes.buffer_size[meshID]	= sizeof(*meshes.data[0]) * (num_faces * mesh_data_stride);
		meshes.data[meshID]			= malloc(meshes.buffer_size[meshID]);
		Vec3 *cur_mesh_data			= meshes.data[meshID];
		
		size_t mesh_data_ndx = 0;
		for(int i = 0; i < num_faces; i++) {
			for(unsigned int j = 0; j < 3; j++) {
				// Vertices.
				cur_mesh_data[mesh_data_ndx + (j * index_stride)][0]	= verts_data[indices_data[i * index_stride][j]][0];
				cur_mesh_data[mesh_data_ndx + (j * index_stride)][1]	= verts_data[indices_data[i * index_stride][j]][1];
				cur_mesh_data[mesh_data_ndx + (j * index_stride)][2]	= verts_data[indices_data[i * index_stride][j]][2];
				// Normals.
				cur_mesh_data[mesh_data_ndx + (j * index_stride) + 1][0]	= vert_normals_data[indices_data[(i * index_stride) + 1][j]][0];
				cur_mesh_data[mesh_data_ndx + (j * index_stride) + 1][1]	= vert_normals_data[indices_data[(i * index_stride) + 1][j]][1];
				cur_mesh_data[mesh_data_ndx + (j * index_stride) + 1][2]	= vert_normals_data[indices_data[(i * index_stride) + 1][j]][2];
				// UVs.
				cur_mesh_data[mesh_data_ndx + (j * index_stride) + 2][0]	= vert_tex_data[indices_data[(i * index_stride) + 2][j]][0];
				cur_mesh_data[mesh_data_ndx + (j * index_stride) + 2][1]	= vert_tex_data[indices_data[(i * index_stride) + 2][j]][1];
			}
			mesh_data_ndx += mesh_data_stride;
		}
		
		free(verts_data);
		free(vert_normals_data);
		free(vert_tex_data);
		free(indices_data);
	} else {
		printf("ERROR - Invalid flags passed to wsObjLoad(), aborting load\n");
		return WS_ERROR_MODEL;
	}
	
	// Generate our OpenGL VBO and VAO.
	wsObjGenBuffers(meshID, flags);
	
	// Yay!
	printf("\'%s\' load success\n", path);
	
	meshes.num_meshes++;
	meshes.next_meshID++;
	return meshID;
}

// Generate OpenGL VAO and VBO for the given mesh.  Automatically called in wsObjLoad().
static void wsObjGenBuffers(unsigned int meshID, uint8_t flags) {
	glGenBuffers(1, &meshes.vbo[meshID]);
	glBindBuffer(GL_ARRAY_BUFFER, meshes.vbo[meshID]);
	glBufferData(GL_ARRAY_BUFFER, meshes.buffer_size[meshes.num_meshes], meshes.data[meshID], GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &meshes.vao[meshID]);
	glBindVertexArray(meshes.vao[meshID]);
	
	if(!(flags & WS_NORMALS) && !(flags & WS_TEX)) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	} else if((flags & WS_NORMALS) && !(flags & WS_TEX)) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	} /* else if((flags & WS_NORMALS) && !(flags & WS_TEX)) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	} */ else if((flags & WS_NORMALS) && (flags & WS_TEX)) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
}

// Remove mesh from memory.
static void wsObjDelete(unsigned int meshID) {
	meshes.num_verts[meshID] = 0;
	meshes.num_faces[meshID] = 0;
	meshes.num_meshes--;
	free(meshes.data[meshID]);// TODO: Only delete meshID's data.  Still need to figure that one out.  Save that one for a rainy day.  Or for whenever I figure the rest of this shit out.
}

// Remove all meshes from memory.
static void wsObjTerminate() {
	if(meshes.num_meshes > 0) {
		for(size_t i = 0; i < meshes.num_meshes; i++) {
			free(meshes.data[i]);
		}
	}
}

#endif // LOBJ_H_