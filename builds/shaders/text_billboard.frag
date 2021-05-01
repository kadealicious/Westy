#version 460 core

in vec2 tex_coord;

uniform sampler2D text;
uniform vec3 text_color;
uniform float camera_distance;

out vec4 frag_color;

void main() {
	vec4 sampled_color = vec4(1.0, 1.0, 1.0, texture(text, tex_coord).r);
	frag_color = vec4(text_color, camera_distance) * sampled_color;
}