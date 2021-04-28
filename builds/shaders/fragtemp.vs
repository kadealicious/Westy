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

struct PointLight {
	vec3 position;// In worldspace!!
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

struct DirectionLight {
	vec3 rotation;// As a unit vector!!
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;// In worldspace!!
	vec3 rotation;// As a unit vector!!
	
	float cutoff;
	float outer_cutoff;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

uniform float time;
uniform vec3 view_position;
uniform Material material;
uniform PointLight pointlight;
uniform DirectionLight directionlight;
uniform SpotLight spotlight;

out vec4 frag_color;

void main() {
	// POINT LIGHT
	// Ambient.
	/*vec3 ambient = pointlight.ambient * texture(material.diffuse_map, tex_coord).rgb;
	
	// Diffuse.
	vec3 unit_normal = normalize(normal);
	vec3 ray_direction = normalize(pointlight.position - frag_position);
	float diffuse_strength = max(dot(unit_normal, ray_direction), 0.0f);
	vec3 diffuse = pointlight.diffuse * diffuse_strength * texture(material.diffuse_map, tex_coord).rgb;
	
	// Specular.
	vec3 view_direction = normalize(-frag_position);
	vec3 reflection_direction = reflect(-ray_direction, unit_normal);
	float specular_strength = pow(max(dot(view_direction, reflection_direction), 0.0f), material.shininess);
	vec3 specular = pointlight.specular * specular_strength * texture(material.specular_map, tex_coord).rgb;
	
	// Attenuation.
	float pointlight_distance = length(pointlight.position - frag_position);
	float attenuation = 1.0f / (pointlight.constant + pointlight.linear * pointlight_distance + pointlight.quadratic * (pointlight_distance * pointlight_distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	frag_color = vec4(ambient + diffuse + specular, 1.0f);*/
	
	// SPOTLIGHT
	vec3 ray_direction = normalize(spotlight.position - frag_position);
	
	vec3 ambient = spotlight.ambient * texture(material.diffuse_map, tex_coord).rgb;
	
	vec3 unit_normal = normalize(normal);
	float diffuse_strength = max(dot(unit_normal, ray_direction), 0.0f);
	vec3 diffuse = spotlight.diffuse * diffuse_strength * texture(material.diffuse_map, tex_coord).rgb;
	
	vec3 view_direction = normalize(view_position - frag_position);
	vec3 reflection_direction = reflect(-ray_direction, unit_normal);
	float specular_strength = pow(max(dot(view_direction, reflection_direction), 0.0f), material.shininess);
	vec3 specular = spotlight.specular * specular_strength * texture(material.specular_map, tex_coord).rgb;
	
	float theta = dot(ray_direction, normalize(-spotlight.rotation));
	float epsilon = spotlight.cutoff - spotlight.outer_cutoff;
	float intensity = clamp((theta - spotlight.outer_cutoff) / epsilon, 0.0f, 1.0f);
	diffuse *= intensity;
	specular *= intensity;
	
	float spotlight_distance = length(spotlight.position - frag_position);
	float attenuation = 1.0f / (spotlight.constant + spotlight.linear * spotlight_distance + spotlight.quadratic * (spotlight_distance * spotlight_distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	frag_color = vec4(ambient + diffuse + specular, 1.0f);
	
	// DIRECTIONAL LIGHT
	// Ambient.
	/*vec3 ambient = directionlight.ambient * texture(material.diffuse_map, tex_coord).rgb;
	
	// Diffuse.
	vec3 unit_normal = normalize(normal);
	vec3 ray_direction = normalize(-directionlight.rotation);
	float diffuse_strength = max(dot(unit_normal, ray_direction), 0.0f);
	vec3 diffuse = directionlight.diffuse * diffuse_strength * texture(material.diffuse_map, tex_coord).rgb;
	
	// Specular.
	vec3 view_direction = normalize(-frag_position);
	vec3 reflection_direction = reflect(-ray_direction, unit_normal);
	float specular_strength = pow(max(dot(view_direction, reflection_direction), 0.0f), material.shininess);
	vec3 specular = directionlight.specular * specular_strength * texture(material.specular_map, tex_coord).rgb;
	
	frag_color = vec4(ambient + diffuse + specular, 1.0f);*/
}