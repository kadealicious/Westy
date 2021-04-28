#version 460 core

in vec3 normal;
in vec3 frag_position;
in vec3 light_position_viewspace;

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;

uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

out vec4 frag_color;

void main() {
	// Ambient.
	vec3 ambient = material_ambient * light_ambient;
	
	// Diffuse.
	vec3 unit_normal = normalize(normal);
	vec3 light_direction = normalize(light_position_viewspace - frag_position);
	float diffuse_strength = max(dot(unit_normal, light_direction), 0.0f);
	vec3 diffuse = (material_diffuse * diffuse_strength) * light_diffuse;
	
	// Specular.
	vec3 view_direction = normalize(-frag_position);
	vec3 reflection_direction = reflect(-light_direction, unit_normal);
	float specular_coeff = pow(max(dot(view_direction, reflection_direction), 0.0f), material_shininess);
	vec3 specular = (material_specular * specular_coeff) * light_specular;
	
	vec3 final_color = (ambient + diffuse + specular) * material_diffuse;
	frag_color = vec4(final_color, 1.0f);
}