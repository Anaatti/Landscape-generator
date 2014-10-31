/** camera.cpp **/

#include "camera.hpp"
#include <cmath>

#define SPEED 1.0
#define ASPEED 0.04
#define MOUSE_SENSITIVITY 0.005

camera_c::camera_c() {
	set_defaults();
}

void camera_c::set_defaults() {
	posx = 0;
	posy = 100;
	posz = 0;
	yaw = 0;
	pitch = -1.1;
}

void camera_c::move(const float mousex, const float mousey) {
	if(playerinputs[SDLK_w]) {
		posx+= SPEED * cos(yaw) * cos(pitch);
		posz+= SPEED * sin(yaw) * cos(pitch);
		posy+= SPEED * sin(pitch);
	}
	if(playerinputs[SDLK_s]) {
		posx-= SPEED * cos(yaw) * cos(pitch);
		posz-= SPEED * sin(yaw) * cos(pitch);
		posy-= SPEED * sin(pitch);
	}
	if(playerinputs[SDLK_a]) {
		posx+= SPEED * cos(yaw - PI / 2.0);
		posz+= SPEED * sin(yaw - PI / 2.0);
	}
	if(playerinputs[SDLK_d]) {
		posx+= SPEED * cos(yaw + PI / 2.0);
		posz+= SPEED * sin(yaw + PI / 2.0);
	}
	if(playerinputs[SDLK_PAGEUP] || SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)) posy+= SPEED;
	if(playerinputs[SDLK_PAGEDOWN] || SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3)) posy-= SPEED;
	if(playerinputs[SDLK_UP]) pitch+= ASPEED;
	if(playerinputs[SDLK_DOWN]) pitch-= ASPEED;
	if(playerinputs[SDLK_LEFT]) yaw-= ASPEED;
	if(playerinputs[SDLK_RIGHT]) yaw+= ASPEED;
	yaw-= mousex * MOUSE_SENSITIVITY;
	pitch+= mousey * MOUSE_SENSITIVITY;
	if(playerinputs[SDLK_KP0]) set_defaults();
	if(yaw < -PI) yaw+= PI * 2;
	if(yaw > PI) yaw-= PI * 2;
	if(pitch < -89.0 / 180.0 * PI) pitch = -89.0 / 180.0 * PI;
	if(pitch > 89.0 / 180.0 * PI) pitch = 89.0 / 180.0 * PI;
}

void camera_c::draw(const int d) const {
	glLoadIdentity();
	//This commented out part is for the broken cross-eyed 3d support.
	/*const float shiftx = DRAW_3D ? ((float)d / 10.0 - 0.05) : 0;
	gluLookAt(posx + 0.5 * cos(yaw + shiftx), posy, posz + 0.5 * sin(yaw + shiftx),
		posx + cos(yaw) * cos(pitch), posy + sin(pitch), posz + sin(yaw) * cos(pitch), 0, 1, 0);*/
	gluLookAt(posx, posy, posz,
		posx + cos(yaw) * cos(pitch), posy + sin(pitch), posz + sin(yaw) * cos(pitch),
		0, 1, 0);
}
