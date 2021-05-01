#include<stdio.h>
#include<string.h>

#include"text.h"
#include"shader.h"

// Max number of font faces allowed.
#define WS_MAX_FACES 10

// Max index of possible chars.  Using ASCII because fuck it.
const unsigned int WS_NUM_CHARS = 128;

// For textured quads method.
unsigned int text_vao;
unsigned int text_vbo;

unsigned int texture_units[WS_MAX_FACES];
unsigned int texture_unit_active;
static unsigned int texture_units_used = 0;

mat4 matrix_ortho;
mat4 matrix_perspective;
Char *font_chars;

// Initialize text renderer.
bool wsTextInit() {
	font_chars = malloc(sizeof(*font_chars) * WS_NUM_CHARS);
	
	// For struct packing, disables byte-alignment restriction.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	// OpenGL quad vertex information initialization.
	glGenVertexArrays(1, &text_vao);
	glGenBuffers(1, &text_vbo);
	glBindVertexArray(text_vao);
	glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	printf("FreeType init success\n");
	return true;
}

// Load font face.
bool wsTextLoadFace(const char *face_path, unsigned int face_size, unsigned int gl_texture_unit) {
	if(texture_units_used >= WS_MAX_FACES) {
		printf("ERROR - Max number of font faces used\n");
		return false;
	}
	texture_unit_active = texture_units_used++;
	texture_units[texture_unit_active] = gl_texture_unit;
	
	// Necessary for loading fonts in.
	FT_Library ft_lib;
	FT_Face ft_face;
	FT_Error ft_err;
	
	// Library initialization.
	ft_err = FT_Init_FreeType(&ft_lib);
	if(ft_err != WS_OKAY) {
		printf("ERROR - FreeType library @ %p init fail - code 0x%02x\n", &ft_lib, ft_err);
		return false;
	}
	
	// Font loading.
	ft_err = FT_New_Face(ft_lib, face_path, 0, &ft_face);
	if(ft_err != WS_OKAY) {
		printf("ERROR - FreeType face '%s' load fail - code 0x%02x\n", face_path, ft_err);
		return false;
	}
	
	// face_size for pixel height of font, 0 lets FreeType automatically calculate font width.
	FT_Set_Pixel_Sizes(ft_face, 0, face_size);
	
	// Load all characters individually.  Error codes in hex for easy FreeType docs consultation.
	for(unsigned int i = 0; i < WS_NUM_CHARS; i++) {
		FT_Error ft_err = FT_Load_Char(ft_face, (char)i, FT_LOAD_RENDER);
		if(ft_err != WS_OKAY) {
			printf("ERROR - FreeType char\t%c (%d)\tload fail - code 0x%02x\n", (char)i, i, ft_err);
			continue;
		}
		
		// Housekeeping.
		unsigned int glyph_width	= ft_face->glyph->bitmap.width;
		unsigned int glyph_rows		= ft_face->glyph->bitmap.rows;
		unsigned int glyph_left		= ft_face->glyph->bitmap_left;
		unsigned int glyph_top		= ft_face->glyph->bitmap_top;
		
		// Generate texture.
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glActiveTexture(texture_units[texture_unit_active]);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(
			GL_TEXTURE_2D,  
			0, 
			GL_RED, 
			glyph_width, 
			glyph_rows, 
			0, 
			GL_RED, 
			GL_UNSIGNED_BYTE, 
			ft_face->glyph->bitmap.buffer
		);
		
		// Set texture options for OpenGL.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		// Actual Char initialization.  Don't feel like abstracting this into a function.
		font_chars[i].textureID = textureID;
		font_chars[i].advance = ft_face->glyph->advance.x;
		font_chars[i].size[0] = glyph_width;
		font_chars[i].size[1] = glyph_rows;
		font_chars[i].bearing[0] = glyph_left;
		font_chars[i].bearing[1] = glyph_top;
	}
	
	// Bye bye!
	FT_Done_Face(ft_face);
	FT_Done_FreeType(ft_lib);
	
	return true;
}

// Render text in 2d screenspace.
void wsTextRender(unsigned int shaderID, const char* text, vec2 position, float scale, vec3 color, mat4 *matrix_ortho) {
	// Use shader, set color and projection matrices.  Texture unit too!!
	wsShaderUse(shaderID);
	wsShaderSetVec3(shaderID, "text_color", color);
	wsShaderSetMat4(shaderID, "projection", matrix_ortho);
	
	// Loop through string, construct Char information.
	for(unsigned int i = 0; i < strlen(text); i++) {
		char c = text[i];
		Char ch = font_chars[(int)c];
		
		float posx = position[0] + (ch.bearing[0] * scale);
		float posy = position[1] - (ch.size[1] - ch.bearing[1]) * scale;
		float char_width = ch.size[0] * scale;
		float char_height = ch.size[1] * scale;
		
		// For textured quads method.
		float vertices[6][4] = {
			{ posx,					posy + char_height,	0.0f, 0.0f }, 
			{ posx,					posy,				0.0f, 1.0f }, 
			{ posx + char_width,	posy,				1.0f, 1.0f }, 
			
			{ posx,					posy + char_height,	0.0f, 0.0f }, 
			{ posx + char_width,	posy,				1.0f, 1.0f }, 
			{ posx + char_width,	posy + char_height,	1.0f, 0.0f }, 
		};
		
		// Bind texture.
		glActiveTexture(texture_units[texture_unit_active]);
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		
		// Set vertex array/buffer data.
		glBindVertexArray(text_vao);
		glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// Bitshift by 6 to get value in pixels to move for next Char draw.
		position[0] += (ch.advance >> 6) * scale;
	}
	
	// Unbind.  All done!
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


// Render billboard text in 3d worldspace.
void wsTextBillboardRender(unsigned int shaderID, const char* text, vec3 position, float scale, vec3 color, wsCamera *camera, mat4 *matrix_view, mat4 *matrix_perspective) {
	// Calculate end width + height for centering.
	float half_width = 0.0f;
	for(unsigned int i = 0; i < strlen(text); i++) {
		char c = text[i];
		Char ch = font_chars[(int)c];
		
		half_width += ch.size[0] + (ch.advance >> 6);
	}
	half_width *= scale * 0.3f;
	
	// Localspace to worldspace using matrix_model + position.
	mat4 matrix_model, matrix_lookat;
	glm_mat4_identity(matrix_model);
	glm_translate(matrix_model, position);
	
	glm_lookat((vec3){0.0f, 0.0f, 0.0f}, camera->rotation, camera->up, matrix_lookat);
	glm_mat4_inv_fast(matrix_lookat, matrix_lookat);
	
	// Send camera distance to shader for depth alpha.
	float camera_distance = glm_vec3_distance(camera->position, position);
	wsShaderSetFloat(shaderID, "camera_distance", 1.0f / camera_distance);
	
	// Use shader, set color, camera distance, and projection matrices.  Texture unit too!!
	wsShaderUse(shaderID);
	wsShaderSetVec3(shaderID, "text_color", color);
	wsShaderSetMat4(shaderID, "lookat", &matrix_lookat);
	wsShaderSetMVP(shaderID, &matrix_model, matrix_view, matrix_perspective);
	
	// Loop through string, construct Char information.
	vec3 position_local = {-half_width, 0.0f, 0.0f};
	for(unsigned int i = 0; i < strlen(text); i++) {
		// Get corresponding Char obj.
		char c = text[i];
		Char ch = font_chars[(int)c];
		
		float posx = position_local[0] + (ch.bearing[0] * scale);
		float posy = position_local[1] - (ch.size[1] - ch.bearing[1]) * scale;
		float char_width = ch.size[0] * scale;
		float char_height = ch.size[1] * scale;
		
		// For textured quads method.
		float vertices[6][4] = {
			{ posx,					posy + char_height,	0.0f, 0.0f }, 
			{ posx,					posy,				0.0f, 1.0f }, 
			{ posx + char_width,	posy,				1.0f, 1.0f }, 
			
			{ posx,					posy + char_height,	0.0f, 0.0f }, 
			{ posx + char_width,	posy,				1.0f, 1.0f }, 
			{ posx + char_width,	posy + char_height,	1.0f, 0.0f }, 
		};
		
		// Bind texture.
		glActiveTexture(texture_units[texture_unit_active]);
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		
		// Set vertex array/buffer data.
		glBindVertexArray(text_vao);
		glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// Bitshift by 6 to get value in pixels to move for next Char draw.
		position_local[0] += (ch.advance >> 6) * scale;
	}
	
	// Unbind.  All done!
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Free memory associated with text renderer.
void wsTextTerminate() {
	glDeleteVertexArrays(1, &text_vao);
	glDeleteBuffers(1, &text_vbo);
	
	free(font_chars);
}