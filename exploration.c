#include"exploration.h"
#include"camera.h"
#include"light.h"
#include"text.h"
#include"input.h"
#include"entities.h"

float time_of_day = 12.0f;
unsigned int day = 1;

unsigned int obj_crate;
unsigned int env_desert;

void wsExploreInit() {
	wsLightInitp(0, (vec3){-3.0f, 0.0f, 0.0f}, (vec3){0.4f, 0.85f, 0.0f}, 0.5f, 0.5f, true);
	wsLightInitf(0, cameras.position[wsCameraGetActive()], cameras.rotation[wsCameraGetActive()], (vec3){0.7f, 0.7f, 0.5f}, 1.0f, 0.5f, 17.5f, false);
	
	wsLightInitd(0, (vec3){-90.0f, 0.0f, 0.0f}, (vec3){0.8f, 0.9f, 1.0f}, 1.0f, false);// Sun.
	wsLightInitd(1, (vec3){90.0f, 0.0f, 0.0f}, (vec3){0.2f, 0.1f, 0.5f}, 0.5f, false);// Moon.
	wsLightInitd(2, (vec3){-90.0f, 0.0f, 0.0f}, (vec3){0.55f, 0.35f, 0.0f}, 1.0f, false);// Sunrise.
	wsLightInitd(3, (vec3){90.0f, 0.0f, 0.0f}, (vec3){1.0f, 0.6f, 0.0f}, 0.5f, false);// Sunset.
	wsLightInitd(4, (vec3){0.0f, 0.0f, 0.0f}, (vec3){1.0f, 1.0f, 1.0f}, 1.0f, true);// Interrogation light.
	
	wsCameraInit((vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, -90.0f, 0.0f}, 90.0f);
	wsCameraInit((vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, -90.0f, 0.0f}, 90.0f);
	wsCameraSetActive(0);
	
	obj_crate = wsObjectInit((vec3){0.0f, 0.0f, 3.0f}, (vec3){-45.0f, 45.0f, 45.0f}, "models/test/crate/crate.obj", WS_FACES | WS_NORMALS | WS_TEX, "models/test/crate/woodcrate_diffuse.png", "models/test/crate/woodcrate_specular.png");
	
	printf("You may now begin exploring!\n");
}

void wsExploreCycleDaylight() {
	if(wsInputGetHold(GLFW_KEY_J))
		time_of_day -= delta_time * 2.5f;
	else if(wsInputGetHold(GLFW_KEY_K))
		time_of_day -= (delta_time * 0.01f);
	else if(wsInputGetHold(GLFW_KEY_L))
		time_of_day += delta_time * 2.5f;
	
	if(time_of_day >= 24.0f) {
		time_of_day = 0.0f;
		day++;
	} else if(time_of_day < 0.0f) {
		time_of_day = 24.0f;
		day--;
	} else time_of_day += (delta_time * 0.01f);
	
	// Directional lights.
	static bool directionlight_toggle = false;
	if(wsInputGetPress(GLFW_KEY_3)) {
		directionlight_toggle = !directionlight_toggle;
		wsLightToggled(0, directionlight_toggle);
		wsLightToggled(1, directionlight_toggle);
		wsLightToggled(2, directionlight_toggle);
		wsLightToggled(3, directionlight_toggle);
		wsLightToggled(4, !directionlight_toggle);
		if(wsLightGetToggled(0))
			printf("Turning on daylight cycle...\n");
		else printf("Turning off daylight cycle...\n");
	}
	if(directionlight_toggle) {
		float sunlight_amt = fabs(-cos(time_of_day / 3.82f));// Gives sin wave with troughs at 0 and 24.002.
		float sun_anglex = ((180 * ((time_of_day * 4.16f) * 0.01f)) - 90.0f);
		float sun_angley = -(45 * ((time_of_day * 4.16f) * 0.01f));
		vec3 sun_angle = {sun_anglex - 180, sun_angley - 180, 0.0f};
		// printf("day: %d | time of day: %f, amount of sun: %f, sun angle x: %f, sun_angle y: %f\n", day, time_of_day, sunlight_amt, sun_anglex, sun_angley);
		
		// Sun.
		if(time_of_day > 6.0f && time_of_day < 18.0f) {
			wsLightToggled(0, true);
			wsLightSetRotationd(0, sun_angle);
			wsLightSetIntensityd(0, (sunlight_amt * 0.25f));
		} else wsLightToggled(0, false);
		// Moon.
		if(time_of_day < 6.0f || time_of_day > 18.0f){
			wsLightToggled(1, true);
			wsLightSetRotationd(1, sun_angle);
			wsLightSetIntensityd(1, (sunlight_amt * 0.15f));
		} else wsLightToggled(1, false);
		
		// Sunrise.
		if(time_of_day > 3.0f && time_of_day < 8.0f) {
			float sunrise_amt = fmax(0.0f, sin(time_of_day + 8.0f)) * 0.15f;
			wsLightToggled(2, true);
			wsLightSetRotationd(2, sun_angle);
			wsLightSetIntensityd(2, sunrise_amt);
		} else wsLightToggled(2, false);
		// Sunset.
		if(time_of_day > 15.0f && time_of_day < 20.0f) {
			float sunset_amt = fmax(0.0f, sin(time_of_day + 10.0f)) * 0.2f;
			wsLightToggled(3, true);
			wsLightSetRotationd(3, sun_angle);
			wsLightSetIntensityd(3, sunset_amt);
		} else wsLightToggled(3, false);
	}
}
void wsExploreDebugLights() {
	// Point lights.
	static bool pointlight_toggle = true;
	if(wsInputGetPress(GLFW_KEY_1)) {
		pointlight_toggle = !pointlight_toggle;
		for(unsigned int i = 0; i < WS_MAX_POINTLIGHTS; i++)
			wsLightTogglep(i, pointlight_toggle);
		if(wsLightGetTogglep(0))
			printf("Turning on pointlights...\n");
		else printf("Turning off pointlights...\n");
	}
	
	// Spotlights.
	static bool flashlight_toggle = false;
	if(wsInputGetPress(GLFW_KEY_2)) {
		flashlight_toggle = !flashlight_toggle;
		wsLightTogglef(0, flashlight_toggle);
		if(wsLightGetTogglef(0))
			printf("Turning on spotlights...\n");
		else printf("Turning off spotlights...\n");
	}
}
void wsExploreRenderUI(unsigned int text_shader, mat4 *matrix_ortho) {
	// Render clock.
	char clock_text[7];
	bool military = true;
	double min_dec;
	unsigned int min = modf(time_of_day, &min_dec) * 60;
	unsigned int hour = (int)time_of_day % 24;
	
	// Military/12-hour clock variants.
	if(military) {
		sprintf(clock_text, "day %d, [%02d:%02d]", day, hour, min);
	} else {
		unsigned int hour_disp = hour > 12 ? (hour - 12) : hour;
		if(hour_disp == 0) hour_disp = 12;
		
		char mantissa = 'a';
		if(hour >= 12.0f) mantissa = 'p';
		
		sprintf(clock_text, "day %d, [%02d:%02d %cm]", day, hour_disp, min, mantissa);
	}
	vec3 clock_color = {1.0f, 1.0f, 1.0f};
	wsTextRender(text_shader, clock_text, (vec2){930.0f, 1030.0f}, 0.2f, clock_color, matrix_ortho);
}
void wsExploreUpdate(unsigned int delta_time) {
	// Camera toggling.
	static bool is_fly = false;
	if(wsInputGetPress(GLFW_KEY_G))
		is_fly = !is_fly;
	if(!is_fly)
		wsCameraMakeFPS(wsCameraGetActive(), 2.5f, 89.0f);
	else wsCameraMakeFly(wsCameraGetActive(), 2.5f, 89.0f);
	
	// Cube visibility toggling.
	if(wsInputGetPress(GLFW_KEY_X))
		wsObjectToggleVisible(obj_crate);
	
	// Flashlight.
	wsLightSetPositionf(0, cameras.position[wsCameraGetActive()]);
	wsLightSetRotationf(0, cameras.rotation[wsCameraGetActive()]);
	
	// Daylight cycle.
	wsExploreCycleDaylight();
	wsExploreDebugLights();
}

void wsExploreTerminate() {
	printf("See ya!\n");
}