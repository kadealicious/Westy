#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normal_matrix;
uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 object_color;

out vec3 final_color;

void main() {
	vec3 normal = normal_matrix * a_normal;
	vec3 light_position_viewspace = vec3(view * vec4(light_position, 1.0f));
	vec3 vert_position = vec3(view * model * vec4(a_pos, 1.0f));
	
	gl_Position = projection * view * model * vec4(a_pos, 1.0f);
	
	// Ambient.
	float ambient_strength = 0.1f;
	vec3 ambient = ambient_strength * light_color;
	
	// Diffuse.
	vec3 unit_normal = normalize(normal);
	vec3 light_direction = normalize(light_position_viewspace - vert_position);
	float diffuse_strength = max(dot(unit_normal, light_direction), 0.0f);
	vec3 diffuse = diffuse_strength * light_color;
	
	// Specular.
	float specular_strength = 0.5f;
	vec3 view_direction = normalize(-vert_position);
	vec3 reflection_direction = reflect(-light_direction, unit_normal);
	float specular_coeff = pow(max(dot(view_direction, reflection_direction), 0.0f), 8);
	vec3 specular = specular_strength  * specular_coeff * light_color;
	
	final_color = (ambient + diffuse + specular) * object_color;
}