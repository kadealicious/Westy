#version 460 core

#define MAX_POINTLIGHTS 4
#define MAX_SPOTLIGHTS 1
#define MAX_DIRECTIONLIGHTS 1

struct Material {
	sampler2D diffuse_map;
	sampler2D specular_map;
	sampler2D emission_map;
	float shininess;
};

struct PointLights {
	vec3 position[MAX_POINTLIGHTS];
	
	vec3 ambient[MAX_POINTLIGHTS];
	vec3 diffuse[MAX_POINTLIGHTS];
	vec3 specular[MAX_POINTLIGHTS];
	
	float constant[MAX_POINTLIGHTS];
	float linear[MAX_POINTLIGHTS];
	float quadratic[MAX_POINTLIGHTS];
};
struct SpotLights {
	vec3 position[MAX_SPOTLIGHTS];
	vec3 rotation[MAX_SPOTLIGHTS];
	
	float cutoff[MAX_SPOTLIGHTS];
	float outer_cutoff[MAX_SPOTLIGHTS];
	
	vec3 ambient[MAX_SPOTLIGHTS];
	vec3 diffuse[MAX_SPOTLIGHTS];
	vec3 specular[MAX_SPOTLIGHTS];
	
	float constant[MAX_SPOTLIGHTS];
	float linear[MAX_SPOTLIGHTS];
	float quadratic[MAX_SPOTLIGHTS];
};
struct DirectionLights {
	vec3 rotation[MAX_DIRECTIONLIGHTS];
	
	vec3 ambient[MAX_DIRECTIONLIGHTS];
	vec3 diffuse[MAX_DIRECTIONLIGHTS];
	vec3 specular[MAX_DIRECTIONLIGHTS];
};

in vec3 normal;
in vec3 frag_position;
in vec2 tex_coord;

uniform float time;
uniform vec3 view_position;

uniform Material material;
uniform PointLights pointlights;
uniform SpotLights spotlights;
uniform DirectionLights directionlights;

out vec4 frag_color;

vec3 CalculateDirectionLight(uint lightID, vec3 unit_normal, vec3 view_direction);
vec3 CalculatePointLight(uint lightID, vec3 unit_normal, vec3 view_direction);
vec3 CalculateSpotLight(uint lightID, vec3 unit_normal, vec3 view_direction);
float BayerFindClosest(int x, int y, float col);

const float Epsilon = 1e-10;

vec3 RGBtoHSV(in vec3 RGB)
{
    vec4  P   = (RGB.g < RGB.b) ? vec4(RGB.bg, -1.0, 2.0/3.0) : vec4(RGB.gb, 0.0, -1.0/3.0);
    vec4  Q   = (RGB.r < P.x) ? vec4(P.xyw, RGB.r) : vec4(RGB.r, P.yzx);
    float C   = Q.x - min(Q.w, Q.y);
    float H   = abs((Q.w - Q.y) / (6.0 * C + Epsilon) + Q.z);
    vec3  HCV = vec3(H, C, Q.x);
    float S   = HCV.y / (HCV.z + Epsilon);
    return vec3(HCV.x, S, HCV.z);
}

vec3 HSVtoRGB(in vec3 HSV)
{
    float H   = HSV.x;
    float R   = abs(H * 6.0 - 3.0) - 1.0;
    float G   = 2.0 - abs(H * 6.0 - 2.0);
    float B   = 2.0 - abs(H * 6.0 - 4.0);
    vec3  RGB = clamp( vec3(R,G,B), 0.0, 1.0 );
    return ((RGB - 1.0) * HSV.y + 1.0) * HSV.z;
}

void main() {
	vec3 light_color = vec3(0.0f), output_color;
	vec3 unit_normal = normalize(normal);
	vec3 view_direction = normalize(view_position - frag_position);
	float light_intensity = 1.0;
	
	float light_color_magnitude;
	for(uint i = 0; i < MAX_POINTLIGHTS; i++)
		light_color += CalculatePointLight(i, unit_normal, view_direction) * light_intensity;
	for(uint i = 0; i < MAX_SPOTLIGHTS; i++)
		light_color += CalculateSpotLight(i, unit_normal, view_direction) * light_intensity;
	for(uint i = 0; i < MAX_DIRECTIONLIGHTS; i++)
		light_color += CalculateDirectionLight(i, unit_normal, view_direction) * light_intensity;
	
	light_color_magnitude = length(light_color);
	light_color += pow(light_color_magnitude, 2.0) * 2;
	
	vec3 sat_light_color = RGBtoHSV(light_color);
	sat_light_color.y *= 2.0;
	sat_light_color.z *= 2.0;
	light_color = HSVtoRGB(sat_light_color);
	
	vec3 dither_color;
	int xdither = int(mod(gl_FragCoord.x, 8));
	int ydither = int(mod(gl_FragCoord.y, 8));
	dither_color.r = BayerFindClosest(xdither, ydither, light_color.r);
	dither_color.g = BayerFindClosest(xdither, ydither, light_color.g);
	dither_color.b = BayerFindClosest(xdither, ydither, light_color.b);
	
	output_color = dither_color;
	
	frag_color = vec4(output_color, 1.0);
}

float BayerFindClosest(int x, int y, float col) {
	float limit = 0.0f;
	int dither[8][8] = {
		{0, 32, 8, 40, 2, 34, 10, 42},		// 8x8 Bayer ordered dithering 
		{48, 16, 56, 24, 50, 18, 58, 26},	// pattern. Each input pixel 
		{12, 44, 4, 36, 14, 46, 6, 38},		// is scaled to the 0..63 range 
		{60, 28, 52, 20, 62, 30, 54, 22},	// before looking in this table 
		{3, 35, 11, 43, 1, 33, 9, 41},		// to determine the action.
		{51, 19, 59, 27, 49, 17, 57, 25},
		{15, 47, 7, 39, 13, 45, 5, 37},
		{63, 31, 55, 23, 61, 29, 53, 21}
	};
	
	if(x < 8)
		limit = (dither[x][y] + 1) / 64.0f;
	if(col < limit)
		return 0.0f;
	
	return 1.0f;
}

vec3 CalculatePointLight(uint lightID, vec3 unit_normal, vec3 view_direction) {
	// Ambient.
	vec3 ambient = pointlights.ambient[lightID] * texture(material.diffuse_map, tex_coord).rgb;
	
	// Diffuse.
	vec3 ray_direction = normalize(pointlights.position[lightID] - frag_position);
	float diffuse_strength = max(dot(unit_normal, ray_direction), 0.0f);
	vec3 diffuse = pointlights.diffuse[lightID] * diffuse_strength * texture(material.diffuse_map, tex_coord).rgb;
	
	// Specular.
	vec3 reflection_direction = reflect(-ray_direction, unit_normal);
	float specular_strength = pow(max(dot(view_direction, reflection_direction), 0.0f), material.shininess);
	vec3 specular = pointlights.specular[lightID] * specular_strength * texture(material.specular_map, tex_coord).rgb;
	
	// Attenuation.
	float light_distance = length(pointlights.position[lightID] - frag_position);
	float attenuation = 1.0f / (pointlights.constant[lightID] + pointlights.linear[lightID] * light_distance + pointlights.quadratic[lightID] * (light_distance * light_distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);
}
vec3 CalculateSpotLight(uint lightID, vec3 unit_normal, vec3 view_direction) {
	// Direction from light to fragment.
	vec3 ray_direction = normalize(spotlights.position[lightID] - frag_position);
	
	// Diffuse.
	float diffuse_strength = max(dot(unit_normal, ray_direction), 0.0f);
	
	// Specular.
	vec3 reflection_direction = reflect(-ray_direction, unit_normal);
	float specular_strength = pow(max(dot(view_direction, reflection_direction), 0.0f), material.shininess);
	
	// Attenuation.
	float dist = length(spotlights.position[lightID] - frag_position);
	float attenuation = 1.0f / (spotlights.constant[lightID] + spotlights.linear[lightID] * dist + spotlights.quadratic[lightID] * (dist * dist));
	
	// Edge fade.
	float theta = dot(ray_direction, normalize(-spotlights.rotation[lightID]));
	float epsilon = spotlights.cutoff[lightID] - spotlights.outer_cutoff[lightID];
	float intensity = clamp((theta - spotlights.outer_cutoff[lightID]) / epsilon, 0.0f, 1.0f);
	
	// Final values.
	vec3 ambient = spotlights.ambient[lightID] * texture(material.diffuse_map, tex_coord).rgb;
	vec3 diffuse = spotlights.diffuse[lightID] * diffuse_strength * texture(material.diffuse_map, tex_coord).rgb;
	vec3 specular = spotlights.specular[lightID] * specular_strength * texture(material.specular_map, tex_coord).rgb;
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	
	return (ambient + diffuse + specular);
}
vec3 CalculateDirectionLight(uint lightID, vec3 unit_normal, vec3 view_direction) {
	// Ambient.
	vec3 ambient = directionlights.ambient[lightID] * texture(material.diffuse_map, tex_coord).rgb;
	
	// Diffuse.
	vec3 ray_direction = normalize(-directionlights.rotation[lightID]);
	float diffuse_strength = max(dot(unit_normal, ray_direction), 0.0f);
	vec3 diffuse = directionlights.diffuse[lightID] * diffuse_strength * texture(material.diffuse_map, tex_coord).rgb;
	
	// Specular.
	vec3 reflection_direction = reflect(-ray_direction, unit_normal);
	float specular_strength = pow(max(dot(view_direction, reflection_direction), 0.0f), material.shininess);
	vec3 specular = directionlights.specular[lightID] * specular_strength * texture(material.specular_map, tex_coord).rgb;
	
	return (ambient + diffuse + specular);
}