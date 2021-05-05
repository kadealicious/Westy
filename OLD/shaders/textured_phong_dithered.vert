#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;

uniform mat3 normal_matrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec3 normal;
out vec3 frag_position;
out vec2 tex_coord;

void main() {
	normal = normal_matrix * a_normal;
	frag_position = vec3(model * vec4(a_pos, 1.0f));
	tex_coord = a_tex_coord;
	
	gl_Position = projection * view * vec4(frag_position, 1.0f);
}