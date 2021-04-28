#include<stdio.h>
#include<string.h>

#include"text.h"
#include"shader.h"

unsigned int vao;
unsigned int vbo;
mat4 *matrix_projection;
Char *font_chars;

bool wsTextInit(unsigned int shaderID, mat4 *projection) {
	matrix_projection = projection;
	font_chars = (Char*)malloc(sizeof(Char) * 128);
	wsShaderSetMat4(shaderID, "projection", matrix_projection);
	
	FT_Library ft_lib;
	FT_Face ft_face;
	FT_Error ft_err;
	
	ft_err = FT_Init_FreeType(&ft_lib);
	if(ft_err != WS_OKAY) {
		printf("ERROR - FreeType library @ %p init fail - code 0x%02x\n", &ft_lib, ft_err);
		return false;
	}
	
	const char *face_path = "fonts/Roboto-Thin.ttf";
	ft_err = FT_New_Face(ft_lib, face_path, 0, &ft_face);
	if(ft_err != WS_OKAY) {
		printf("ERROR - FreeType face '%s' load fail - code 0x%02x\n", face_path, ft_err);
		return false;
	}
	
	FT_Set_Pixel_Sizes(ft_face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	for(unsigned int i = 0; i < 128; i++) {
		ft_err = FT_Load_Char(ft_face, (char)i, FT_LOAD_RENDER);
		if(ft_err != WS_OKAY) {
			printf("ERROR - FreeType char\t%c (%d)\tload fail - code 0x%02x\n", (char)i, i, ft_err);
			continue;
		}
		
		unsigned int glyph_width	= ft_face->glyph->bitmap.width;
		unsigned int glyph_rows		= ft_face->glyph->bitmap.rows;
		unsigned int glyph_left		= ft_face->glyph->bitmap_left;
		unsigned int glyph_top		= ft_face->glyph->bitmap_top;
		
		// Generate texture
		unsigned int textureID;
		glGenTextures(1, &textureID);
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
		
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		font_chars[i].textureID = textureID;
		font_chars[i].advance = ft_face->glyph->advance.x;
		font_chars[i].size[0] = glyph_width;
		font_chars[i].size[1] = glyph_rows;
		font_chars[i].bearing[0] = glyph_left;
		font_chars[i].bearing[1] = glyph_top;
	}
	
	// Free memory
	FT_Done_Face(ft_face);
	FT_Done_FreeType(ft_lib);
	
	// VAO/VBO initialization for textured quads
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	printf("FreeType init success\n");
	return true;
}

void wsTextRender(unsigned int shaderID, const char* text, float x, float y, float scale, vec3 color) {
	wsShaderUse(shaderID);
	wsShaderSetVec3(shaderID, "text_color", color);
	// wsShaderSetMat4(shaderID, "projection", matrix_projection);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindVertexArray(vao);
	
	for(unsigned int i = 0; text[i] != '\0'; i++) {
		char c = text[i];
		Char ch = font_chars[(int)c];
		printf("%c", c);
		
		float posx = x + (ch.bearing[0] * scale);
		float posy = y - (ch.size[1] - ch.bearing[1]) * scale;
		float char_width = ch.size[0] * scale;
		float char_height = ch.size[1] * scale;
		
		float vertices[6][4] = {
			{ posx,					posy + char_height,	0.0f, 0.0f }, 
			{ posx,					posy,				0.0f, 1.0f }, 
			{ posx + char_width,	posy,				1.0f, 1.0f }, 
			
			{ posx,					posy + char_height,	0.0f, 0.0f }, 
			{ posx + char_width,	posy,				1.0f, 1.0f }, 
			{ posx + char_width,	posy + char_height,	1.0f, 0.0f }, 
		};
		
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		x += (ch.advance >> 6) * scale;
	}
	printf("\n");
	
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void wsTextTerminate() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	
	free(font_chars);
}