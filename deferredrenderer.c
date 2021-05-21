#include"deferredrenderer.h"
#include"shader.h"

/* Various buffers for deferred rendering: 
	g_buffer:		final image data.  Stored in a texture 2D.
	g_position:		per-pixel position data  Stored in a texture 2D.
	g_normal:		per-pixel normal data.  Stored in a texture 2D.
	g_albedo_spec:	per-pixel color and shading data.  Shading data 
						is a float from 0-1, so it is stored in the alpha 
						channel.  Stored in a texture 2D.
	r_depth:		per-pixel depth data.  Stored in a render buffer. */
unsigned int g_buffer, g_position, g_normal, g_albedospec, r_depth;
// Shader for g-buffer quad.
unsigned int geometry_shader, lighting_shader;

void wsDefRenRenderQuad();

// Create g-buffer and all sub-buffers.
void wsDefRenInit() {
	// Create and bind g-buffer.
	glGenFramebuffers(1, &g_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
	
	// Create and bind position buffer as GL_TEXTURE_2D.
	glGenTextures(1, &g_position);
	glBindTexture(GL_TEXTURE_2D, g_position);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_width, screen_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position, 0);
	
	// Create and bind normal buffer as GL_TEXTURE_2D.
	glGenTextures(1, &g_normal);
	glBindTexture(GL_TEXTURE_2D, g_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_width, screen_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);
	
	// Create and bind color/shading buffer as GL_TEXTURE_2D.
	glGenTextures(1, &g_albedospec);
	glBindTexture(GL_TEXTURE_2D, g_albedospec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_albedospec, 0);
	
	// Create array of attachments and draw them.
	unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, &attachments[0]);
	
	// Finally, create and attach depth buffer.
	glGenRenderbuffers(1, &r_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, r_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, r_depth);
	
	// Check if framebuffer is complete, then unbind framebuffer.
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("WARNING - Framebuffer incomplete!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	geometry_shader = wsShaderLoad("shaders/gbuffer.vert", "shaders/gbuffer.frag", false, false);
	lighting_shader = wsShaderLoad("shaders/defren.vert", "shaders/defren.frag", false, false);
	
	wsShaderSetInt(lighting_shader, "g_position", 0);
	wsShaderSetInt(lighting_shader, "g_normal", 1);
	wsShaderSetInt(lighting_shader, "g_albedospec", 2);
	
	printf("Deferred Renderer init success\n");
}

// Perform geometry pass using g-buffer.
void wsDefRenGeometryPass(mat4 *model, mat4 *view, mat4 *projection, unsigned int cameraID) {
	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	wsShaderUse(geometry_shader);
	wsShaderSetMat4(geometry_shader, "view", view);
	wsShaderSetMat4(geometry_shader, "projection", projection);
}

// Perform lighting pass and render g-buffer to a quad which fills the screen.
void wsDefRenLightPass(unsigned int cameraID) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	wsShaderUse(lighting_shader);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_position);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g_normal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, g_albedospec);
	
	wsShaderSetVec3(lighting_shader, "view_position", cameras.position[cameraID]);
	for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++)		{ wsShaderUpdateLightp(lighting_shader, i); }
	for(unsigned int i = 0; i < WS_MAX_SPOTLIGHTS; i++)			{ wsShaderUpdateLightf(lighting_shader, i); }
	for(unsigned int i = 0; i < WS_MAX_DIRECTIONLIGHTS; i++)	{ wsShaderUpdateLightd(lighting_shader, i); }
	
	// Render the quad to the screen.
	wsDefRenRenderQuad();
	
	/* Copy geometry depth buffer to default framebuffer's depth buffer.  This allows 
		forward-rendered objects to cull correctly based on depth. */
	glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, screen_width, screen_height, 0, 0, screen_width, screen_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* Source: https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/8.2.deferred_shading_volumes/deferred_shading_volumes.cpp
	Renders quad to screen. */
unsigned int quadVAO = 0;
unsigned int quadVBO;
void wsDefRenRenderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

// For when we resize the window.
void wsDefRenResize() {
	// Start with a clean slate.
	/*wsDefRenTerminate();
	
	// Create and bind g-buffer.
	glGenFramebuffers(1, &g_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
	
	// Create and bind position buffer as GL_TEXTURE_2D.
	glGenTextures(1, &g_position);
	glBindTexture(GL_TEXTURE_2D, g_position);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_width, screen_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position, 0);
	
	// Create and bind normal buffer as GL_TEXTURE_2D.
	glGenTextures(1, &g_normal);
	glBindTexture(GL_TEXTURE_2D, g_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screen_width, screen_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);
	
	// Create and bind color/shading buffer as GL_TEXTURE_2D.
	glGenTextures(1, &g_albedospec);
	glBindTexture(GL_TEXTURE_2D, g_albedospec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_albedospec, 0);
	
	// Create array of attachments and draw them.
	unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, &attachments[0]);
	
	// Finally, create and attach depth buffer.
	glGenRenderbuffers(1, &r_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, r_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, r_depth);
	
	// Check if framebuffer is complete, then unbind framebuffer.
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("WARNING - Framebuffer incomplete!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	printf("Deferred Renderer resize success\n");*/
}

// Destroy buffers.  Don't forget to call this!!
void wsDefRenTerminate() {
	glDeleteBuffers(1, &g_buffer);
	glDeleteBuffers(1, &g_position);
	glDeleteBuffers(1, &g_normal);
	glDeleteBuffers(1, &g_albedospec);
	glDeleteBuffers(1, &r_depth);
	
	printf("Deferred Renderer terminated\n");
}