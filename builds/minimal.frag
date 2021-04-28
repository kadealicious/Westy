#version 330 core

in vec3 new_color;
out vec4 frag_color;

void main() {
	frag_color = vec4(new_color, 1.0);
}