#version 460 core

#define MAX_POINTLIGHTS 50
#define MAX_SPOTLIGHTS 25
#define MAX_DIRECTIONLIGHTS 5
#define SSAO_SAMPLES 6

in vec2 tex_coord;

struct PointLights {
	int toggle[MAX_POINTLIGHTS];
	vec3 position[MAX_POINTLIGHTS];
	vec3 color[MAX_POINTLIGHTS];
	float intensity[MAX_POINTLIGHTS];
	float linear[MAX_POINTLIGHTS];
	float quadratic[MAX_POINTLIGHTS];
};
struct SpotLights {
	int toggle[MAX_POINTLIGHTS];
	vec3 position[MAX_POINTLIGHTS];
	vec3 rotation[MAX_POINTLIGHTS];
	vec3 color[MAX_POINTLIGHTS];
	float intensity[MAX_POINTLIGHTS];
	float linear[MAX_POINTLIGHTS];
	float quadratic[MAX_POINTLIGHTS];
	float cutoff[MAX_POINTLIGHTS];
	float outer_cutoff[MAX_POINTLIGHTS];
};
struct DirectionLights {
	int toggle[MAX_POINTLIGHTS];
	vec3 rotation[MAX_POINTLIGHTS];
	vec3 color[MAX_POINTLIGHTS];
	float intensity[MAX_POINTLIGHTS];
};

uniform vec3 view_pos;
uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedospec;

uniform PointLights pointlights;
uniform SpotLights spotlights;
uniform DirectionLights directionlights;

uniform float time;

out vec4 frag_color;

// Lighting.
vec3 CalculatePointLight(uint lightID);
vec3 CalculateSpotLight(uint lightID);
vec3 CalculateDirectionLight(uint lightID);
// RGB/HSV conversions.
vec3 RGBtoHSV(in vec3 RGB);
vec3 HSVtoRGB(in vec3 HSV);
// Dithering.
float BayerFindClosest(int x, int y, float col);
// SSAO.
float OccludeFragment();

// gbuffer stuff.
vec3 frag_pos_sample = texture(g_position, tex_coord).rgb;
vec3 normal_sample = texture(g_normal, tex_coord).rgb;
vec3 albedo_sample = texture(g_albedospec, tex_coord).rgb;
float spec_sample = texture(g_albedospec, tex_coord).a;
// Hard-coded ambient value + viewing direction for lighting calculations.
vec3 light_color = albedo_sample * 0.01;
vec3 view_dir = normalize(view_pos - frag_pos_sample);

void main() {
	vec3 sky_color = vec3(0.0);
	for(uint i = 0; i < MAX_POINTLIGHTS; i++)		{ light_color += CalculatePointLight(i); }
	for(uint i = 0; i < MAX_SPOTLIGHTS; i++)		{ light_color += CalculateSpotLight(i); }
	for(uint i = 0; i < MAX_DIRECTIONLIGHTS; i++) {
		light_color += CalculateDirectionLight(i);
		sky_color += directionlights.color[i] * directionlights.intensity[i] * directionlights.toggle[i] * 0.2;
	}
	
	// Increase light color fragment;
	float light_color_magnitude;
	light_color_magnitude = length(light_color);
	light_color += pow(light_color_magnitude, 2.0) * 2;
	
	// Saturate + brighten fragment.
	vec3 sat_light_color = RGBtoHSV(light_color);
	sat_light_color.y *= 2.0;
	sat_light_color.z *= 2.0;
	light_color = HSVtoRGB(sat_light_color);
	
	// Dither screen.
	vec3 dither_color;
	int xdither = int(mod(gl_FragCoord.x, 8));
	int ydither = int(mod(gl_FragCoord.y, 8));
	dither_color.r = BayerFindClosest(xdither, ydither, light_color.r);
	dither_color.g = BayerFindClosest(xdither, ydither, light_color.g);
	dither_color.b = BayerFindClosest(xdither, ydither, light_color.b);
	
	float specular_color = max(spec_sample * 4, 0.2);
	
	vec3 final_color = mix(dither_color * specular_color, sky_color, floor(specular_color));
	frag_color = vec4(final_color, 1.0);
}

// Point lights.
vec3 CalculatePointLight(uint lightID) {
	// Diffuse.
	vec3 light_dir = normalize(pointlights.position[lightID] - frag_pos_sample);
	vec3 diffuse = max(dot(normal_sample, light_dir), 0.0) * albedo_sample * pointlights.color[lightID];
	
	// Specular.
	vec3 bounce_dir = normalize(light_dir + view_dir);
	float spec_strength = pow(max(dot(normal_sample, bounce_dir), 0.0), 8.0);
	vec3 specular = pointlights.color[lightID] * spec_strength * spec_sample;
	
	// Attenuation.
	float dist = length(pointlights.position[lightID] - frag_pos_sample);
	float attenuation = 1.0 / (1.0 + pointlights.linear[lightID] * dist + pointlights.quadratic[lightID] * dist * dist);
	diffuse *= attenuation;
	specular *= attenuation;
	
	vec3 light_color = (diffuse + specular) * pointlights.intensity[lightID] * pointlights.toggle[lightID];
	return light_color;
}
// Spotlights.
vec3 CalculateSpotLight(uint lightID) {
	// Diffuse.
	vec3 light_dir = normalize(spotlights.position[lightID] - frag_pos_sample);
	vec3 diffuse = max(dot(normal_sample, light_dir), 0.0) * albedo_sample * spotlights.color[lightID];
	
	// Specular.
	vec3 bounce_dir = normalize(light_dir + view_dir);
	float spec_strength = pow(max(dot(normal_sample, bounce_dir), 0.0), 8.0);
	vec3 specular = spotlights.color[lightID] * spec_strength * spec_sample;
	
	// Attenuation.
	float dist = length(spotlights.position[lightID] - frag_pos_sample);
	float attenuation = 1.0 / (1.0 + spotlights.linear[lightID] * dist + spotlights.quadratic[lightID] * dist * dist);
	diffuse *= attenuation;
	specular *= attenuation;
	
	// Edge fade.
	float theta = dot(light_dir, normalize(-spotlights.rotation[lightID]));
	float epsilon = spotlights.cutoff[lightID] - spotlights.outer_cutoff[lightID];
	float intensity = clamp((theta - spotlights.outer_cutoff[lightID]) / epsilon, 0.0f, 1.0f);
	
	// Final values.
	
	return (diffuse + specular) * intensity * spotlights.intensity[lightID] * spotlights.toggle[lightID];
}
// Directional lights.
vec3 CalculateDirectionLight(uint lightID) {
	// Ambient.
	vec3 ambient = directionlights.color[lightID] * albedo_sample * 0.1;
	
	// Diffuse.
	vec3 light_dir = normalize(-directionlights.rotation[lightID]);
	vec3 diffuse = max(dot(normal_sample, light_dir), 0.0) * albedo_sample * directionlights.color[lightID];
	
	// Specular.
	vec3 bounce_dir = normalize(light_dir + view_dir);
	float spec_strength = pow(max(dot(normal_sample, bounce_dir), 0.0), 8.0);
	vec3 specular = directionlights.color[lightID] * spec_strength * spec_sample;
	
	vec3 light_color = (ambient + diffuse + specular) * directionlights.intensity[lightID] * directionlights.toggle[lightID];
	return light_color;
}

// RGB/HSV conversions.
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

// Dithering algorithm.
float BayerFindClosest(int x, int y, float color) {
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
	
	float limit = (dither[x][y] + 1) / 64.0f;
	return clamp(ceil(color - limit), 0.0, 1.0);
}

/*// SSAO.
float OccludeFragment() {
	uint occlusion;
	
	for(uint i = 0; i < SSAO_SAMPLES; i++) {
		vec3 sample = vec3(sin(i * 0.13), cos(i * 0.76), (sin(i * 0.37) * 0.5) + 0.5);
		sample = normalize(sample);
		
		float scale = (float)i / 64.0;
		scale = lerp(0.1, 1.0, scale * scale);
		sample *= scale;
	}
	
	return occlusion;
}*/