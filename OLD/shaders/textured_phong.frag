#version 460 core

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

in vec3 normal;
in vec3 frag_position;
in vec2 tex_coord;

#define NUM_POINT_LIGHTS 4

uniform float time;
uniform vec3 view_position;
uniform Material material;
uniform PointLight pointlights[NUM_POINT_LIGHTS];
uniform DirectionLight directionlight;
uniform SpotLight spotlight;

out vec4 frag_color;

vec3 CalculateDirectionLight(DirectionLight light, vec3 unit_normal, vec3 view_direction);
vec3 CalculatePointLight(PointLight light, vec3 unit_normal, vec3 view_direction);
vec3 CalculateSpotLight(SpotLight light, vec3 unit_normal, vec3 view_direction);

void main() {
	vec3 output_color;
	vec3 unit_normal = normalize(normal);
	vec3 view_direction = normalize(view_position - frag_position);
	
	output_color = CalculateDirectionLight(directionlight, unit_normal, view_direction);
	for(int i = 0; i < NUM_POINT_LIGHTS; i++)
		output_color += CalculatePointLight(pointlights[i], unit_normal, view_direction);
	output_color += CalculateSpotLight(spotlight, unit_normal, view_direction);
	
	frag_color = vec4(output_color, 1.0f);
}

vec3 CalculateDirectionLight(DirectionLight light, vec3 unit_normal, vec3 view_direction) {
	// Ambient.
	vec3 ambient = light.ambient * texture(material.diffuse_map, tex_coord).rgb;
	
	// Diffuse.
	vec3 ray_direction = normalize(-light.rotation);
	float diffuse_strength = max(dot(unit_normal, ray_direction), 0.0f);
	vec3 diffuse = light.diffuse * diffuse_strength * texture(material.diffuse_map, tex_coord).rgb;
	
	// Specular.
	vec3 reflection_direction = reflect(-ray_direction, unit_normal);
	float specular_strength = pow(max(dot(view_direction, reflection_direction), 0.0f), material.shininess);
	vec3 specular = light.specular * specular_strength * texture(material.specular_map, tex_coord).rgb;
	
	return (ambient + diffuse + specular);
}
vec3 CalculatePointLight(PointLight light, vec3 unit_normal, vec3 view_direction) {
	// Ambient.
	vec3 ambient = light.ambient * texture(material.diffuse_map, tex_coord).rgb;
	
	// Diffuse.
	vec3 ray_direction = normalize(light.position - frag_position);
	float diffuse_strength = max(dot(unit_normal, ray_direction), 0.0f);
	vec3 diffuse = light.diffuse * diffuse_strength * texture(material.diffuse_map, tex_coord).rgb;
	
	// Specular.
	vec3 reflection_direction = reflect(-ray_direction, unit_normal);
	float specular_strength = pow(max(dot(view_direction, reflection_direction), 0.0f), material.shininess);
	vec3 specular = light.specular * specular_strength * texture(material.specular_map, tex_coord).rgb;
	
	// Attenuation.
	float light_distance = length(light.position - frag_position);
	float attenuation = 1.0f / (light.constant + light.linear * light_distance + light.quadratic * (light_distance * light_distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);
}
vec3 CalculateSpotLight(SpotLight light, vec3 unit_normal, vec3 view_direction) {
	// Direction from light to fragment.
	vec3 ray_direction = normalize(light.position - frag_position);
	
	// Diffuse.
	float diffuse_strength = max(dot(unit_normal, ray_direction), 0.0f);
	
	// Specular.
	vec3 reflection_direction = reflect(-ray_direction, unit_normal);
	float specular_strength = pow(max(dot(view_direction, reflection_direction), 0.0f), material.shininess);
	
	// Attenuation.
	float dist = length(light.position - frag_position);
	float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
	
	// Edge fade.
	float theta = dot(ray_direction, normalize(-light.rotation));
	float epsilon = light.cutoff - light.outer_cutoff;
	float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0f, 1.0f);
	
	// Final values.
	vec3 ambient = light.ambient * texture(material.diffuse_map, tex_coord).rgb;
	vec3 diffuse = light.diffuse * diffuse_strength * texture(material.diffuse_map, tex_coord).rgb;
	vec3 specular = light.specular * specular_strength * texture(material.specular_map, tex_coord).rgb;
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	
	return (ambient + diffuse + specular);
}