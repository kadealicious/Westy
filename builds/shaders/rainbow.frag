#version 460 core

in vec3 normal;
in vec3 frag_position;
in vec2 tex_coord;

struct Material {
	sampler2D diffuse_map;
	sampler2D specular_map;
	sampler2D emission_map;
	float shininess;
};

uniform float time;
uniform vec3 view_position;
uniform Material material;

out vec4 frag_color;

vec3 MakeRainbow(vec3 col0, vec3 col1) {
	vec3 unit_normal = normalize(normal);
	vec3 view_direction = normalize(view_position - frag_position);
	vec3 col;
	
	col = sin(time * 0.5f) * view_direction;
	
	return col;
}
vec3 MakeClouds() {
	vec3 unit_normal = normalize(normal);
	vec3 view_direction = normalize(view_position - frag_position);
	vec3 col;
	
	col = vec3(view_position.y - frag_position.y);
	
	return col;
}
vec3 MakeWater() {
	vec3 unit_normal = normalize(normal);
	vec3 view_direction = normalize(view_position - frag_position);
	vec3 col;
	
	vec3 wave = vec3(sin(view_direction.x), cos(view_direction.y), 1.0f);
	col = mix(vec3(0.0f), vec3(0.8f), wave);
	
	return col;
}

void main() {
	vec3 output_color;
	
	output_color += MakeRainbow(vec3(0.0f), vec3(1.0f));
	
	frag_color = vec4(output_color, 1.0f);
}