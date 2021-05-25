#version 460 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_tex_coord;

out vec3 frag_pos;
out vec2 tex_coord;
out vec3 normal;

uniform mat3 normal_matrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	vec4 world_pos = model * vec4(a_pos, 1.0);
	frag_pos = world_pos.xyz;
	tex_coord = a_tex_coord;
	normal = normal_matrix * a_normal;
	
	gl_Position = projection * view * world_pos;
}