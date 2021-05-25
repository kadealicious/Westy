#version 460 core

layout(location = 0) out vec3 g_position;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedospec;

in vec2 tex_coord;
in vec3 frag_pos;
in vec3 normal;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main() {
	// Store position data in first g-buffer texture.
	g_position = frag_pos;
	// Store normal data in second g-buffer texture.
	g_normal = normalize(normal);
	// Store diffuse per-fragment color.
	g_albedospec.rgb = texture(texture_diffuse, tex_coord).rgb;
	// Store specular intensity in alpha component of albedo-spec texture.
	g_albedospec.a = texture(texture_specular, tex_coord).r;
}