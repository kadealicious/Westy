#include"exploration.h"
#include"camera.h"
#include"light.h"
#include"text.h"
#include"input.h"

float time_of_day = 0.0f;
unsigned int day = 1;

void ExploreInit() {
	wsLightInitd(0, (vec3){-90.0f, 0.0f, 0.0f}, (vec3){1.0f, 1.0f, 0.9f}, 1.0f, true);// Sun.
	wsLightInitd(1, (vec3){90.0f, 0.0f, 0.0f}, (vec3){0.2f, 0.1f, 0.5f}, 0.5f, false);// Moon.
	wsLightInitd(2, (vec3){-90.0f, 0.0f, 0.0f}, (vec3){0.55f, 0.35f, 0.0f}, 1.0f, true);// Sunrise.
	wsLightInitd(3, (vec3){90.0f, 0.0f, 0.0f}, (vec3){1.0f, 0.6f, 0.0f}, 0.5f, false);// Sunset.
	
	printf("You may now begin exploring!\n");
}

void ExploreCycleDaylight() {
	if(wsInputGetHold(GLFW_KEY_J))
		time_of_day -= delta_time;
	else if(wsInputGetHold(GLFW_KEY_K))
		time_of_day -= (delta_time * 0.01f);
	else if(wsInputGetHold(GLFW_KEY_L))
		time_of_day += delta_time;
	
	if(time_of_day >= 24.0f) {
		time_of_day = 0.0f;
		day++;
	} else if(time_of_day < 0.0f) {
		time_of_day = 24.0f;
		day--;
	} else time_of_day += (delta_time * 0.01f);
	
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
void ExploreRenderUI(unsigned int text_shader, mat4 *matrix_ortho) {
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
void ExploreUpdate(unsigned int delta_time) {
	// Camera toggling.
	if(wsInputGetPress(GLFW_KEY_G))
		wsCameraSetActive((wsCameraGetActive() == WS_CAMERA0) ? WS_CAMERA1 : WS_CAMERA0);
	if(wsCameraGetActive() == WS_CAMERA0)
		wsCameraMakeFPS(wsCameraGetActive(), 2.5f, 89.0f);
	else wsCameraMakeFly(wsCameraGetActive(), 2.5f, 89.0f); 
	
	// Flashlight.
	wsLightSetPositionf(0, cameras.position[wsCameraGetActive()]);
	wsLightSetRotationf(0, cameras.rotation[wsCameraGetActive()]);
	
	// Daylight cycle.
	ExploreCycleDaylight();
}

void ExploreTerminate() {
	printf("See ya!\n");
}