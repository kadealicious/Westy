#version 460 core

in vec3 final_color;

uniform vec3 object_color;
uniform vec3 light_color;

out vec4 frag_color;

void main() {
	frag_color = vec4(final_color, 1.0f);
}