#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#include"texturing.h"

/* --- TEXTURE LOADING --- */
typedef struct wsTextures {
	unsigned int GLtextures[WS_MAX_TEXTURES];
	unsigned int num_textures;
	bool availableIDs[WS_MAX_TEXTURES];
} wsTextures;
wsTextures textures;

// Get dereferenced OpenGL-compatible texture pointer.
unsigned int wsTextureGetGL(unsigned int textureID)	{ return textures.GLtextures[textureID]; }

// Load a texture using the stbi lib.
int wsTextureLoad(const char *path, unsigned int wrap_style, unsigned int filter_style) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// Clamping.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_style);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_style);
	// Filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_style);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_style);
	
	int img_width, img_height, img_channel_num;
	unsigned char *img_data = stbi_load(path, &img_width, &img_height, &img_channel_num, 0);
	if(img_data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
		glGenerateMipmap(GL_TEXTURE_2D);
		printf("Image data '%s' populated\n", path);
	} else printf("ERROR - Failed to load image data at '%s'\n", path);
	stbi_image_free(img_data);
	
	unsigned int textureID;
	for(size_t i = 0; i < WS_MAX_TEXTURES; i++) {
		if(textures.availableIDs[i] == true) {
			textureID = i;
			textures.availableIDs[i] = false;
			break;
		}
	}
	textures.GLtextures[textureID] = texture;
	textures.num_textures++;
	return textureID;
}

// Remove texture from memory.
void wsTextureDelete(unsigned int textureID) {
	textures.availableIDs[textureID] = true;
	textures.num_textures--;
	glDeleteTextures(1, &(textures.GLtextures[textureID]));
}
/* --- TEXTURE LOADING --- */

// Initialize texturing.  Call this before any texture initialization.
void wsTexturingInit() {
	// OpenGL handles textures upside down.
	stbi_set_flip_vertically_on_load(true);
	
	for(size_t i = 0; i < WS_MAX_TEXTURES; i++)
		textures.availableIDs[i] = true;
	
	printf("Texturing initialized\n");
}

// Remove all textures from memory.
void wsTexturingTerminate() {
	if(textures.num_textures > 0) {
		for(size_t i = 0; i < textures.num_textures; i++) {
			if(textures.availableIDs[i] == false)
				glDeleteTextures(WS_MAX_TEXTURES, &(textures.GLtextures[i]));
		}
		textures.num_textures = 0;
	}
}