#version 460 core

in vec2 tex_coord;

uniform sampler2D tex;

out vec4 color_frag;

void main() {
	color_frag = texture(tex, tex_coord);
}