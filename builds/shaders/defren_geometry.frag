#version 460 core

layout(location = 0) out vec3 g_position;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedospec;

in vec2 tex_coord;
in vec3 frag_pos;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

void main() {
	g_position = frag_pos;
	
	// Normal mapping.
	// mat3 tbn = mat3(g_tangent, g_bitangent, g_normal);
	vec3 normal_map = (texture(texture_normal, tex_coord).rgb * 2.0) - 1.0;
	g_normal = normalize(normal_map);// normalize(tbn * normal_map);
	
	g_albedospec.rgb = texture(texture_diffuse, tex_coord).rgb;
	// Store specular intensity in alpha component of albedo-spec texture.
	g_albedospec.a = texture(texture_specular, tex_coord).r;
}