/** loop.cpp **/

#include "loop.hpp"

#define DELAY 15 //ms per frame

/*
	This is the main loop of the program.
*/

void loop() {
	camera_c camera;
	landscape_c *landscape = new landscape_c;
	SDL_Event event;
	float mouse_xshift = 0, mouse_yshift = 0;
	bool focus, prevfocus = false;
	bool exit = false;
	int starttime = SDL_GetTicks() - DELAY;
	int endtime = 0;

	while(!exit) {
		starttime+= DELAY;

		playerinputs = SDL_GetKeyState(NULL);

		if(playerinputs[SDLK_RETURN]) {
			delete landscape;
			landscape = new landscape_c;
		}

		camera.move(mouse_xshift, mouse_yshift);

		//DRAW
		for(int d=0;d<(DRAW_3D?2:1);d++) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			camera.draw(d);

			landscape->draw();

			if(DRAW_3D) draw_3d(d);
			else SDL_GL_SwapBuffers();
		}

		//REST
		//Event handling
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exit = true;
		}
		if(SDL_GetAppState() & SDL_APPINPUTFOCUS) {
			focus = true;
			SDL_WarpMouse(WIDTH / 2, HEIGHT / 2);
		}
		else focus = false;
		mouse_xshift = 0;
		mouse_yshift = 0;
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) exit = true;
			if(event.type == SDL_MOUSEMOTION && prevfocus) {
				mouse_xshift+= event.motion.xrel;
				mouse_yshift+= event.motion.yrel;
			}
		}
		prevfocus = focus;

		//Wait
		endtime = SDL_GetTicks() - starttime;
		if(DELAY - endtime > 0) SDL_Delay(DELAY - endtime);
		else starttime = SDL_GetTicks() - DELAY;
	}
	delete landscape;
}
