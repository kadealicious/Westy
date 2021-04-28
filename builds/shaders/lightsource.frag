#version 460 core

uniform vec3 light_color;

out vec4 frag_color;

void main() {
	vec3 light_color_bright = (vec3(1.0) - light_color) * 0.5;
	frag_color = vec4(light_color + light_color_bright, 1.0f);
}