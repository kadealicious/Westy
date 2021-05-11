#version 460 core

layout(location = 0) in vec4 vertex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lookat;
uniform float camera_distance;

out vec2 tex_coord;

void main() {
	tex_coord = vertex.zw;
	gl_Position = projection * view * model * lookat * vec4(vertex.xy * (camera_distance * 2), 0.0, 1.0);
}