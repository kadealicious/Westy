#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normal_matrix;
uniform vec3 light_position;

out vec3 normal;
out vec3 frag_position;
out vec3 light_position_viewspace;

void main() {
	normal = normal_matrix * a_normal;
	light_position_viewspace = vec3(view * vec4(light_position, 1.0f));
	frag_position = vec3(view * model * vec4(a_pos, 1.0f));
	
	gl_Position = projection * view * model * vec4(a_pos, 1.0f);
}