/** main.cpp **/

#include "main.hpp"

/*

	This programs simply generates a scene with random midpoint displacement algorithm.
	The scene is also lit and shadowed in a pretty bad way and therefore it takes quite a lot of time.

	NOTE:
	The landscape.cpp file contains all the interesting stuff and all the defines you can easily play with.

*/

const short WIDTH = 800;
const short HEIGHT = 600;
short BLUR_WIDTH, BLUR_HEIGHT; //automatically set
GLuint g3D_texture;
GLuint ground_texture;
Uint8 *playerinputs;

void log(const char *text) { printf("%s\n", text); }

int main(int argc, char **argv) {
	srand(time(NULL));
	int i = 0;
	do {
		BLUR_WIDTH = 2;
		for(int j=i;j>0;j--) BLUR_WIDTH*= 2;
		i++;
	} while(BLUR_WIDTH < WIDTH);
	i = 0;
	do {
		BLUR_HEIGHT = 2;
		for(int j=i;j>0;j--) BLUR_HEIGHT*= 2;
		i++;
	} while(BLUR_HEIGHT < HEIGHT);

	//Starting SDL
	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		throw 0;
	}

	SDL_WM_SetCaption("Landscape", "Landscape");
	SDL_ShowCursor(0);

	//OpenGL settings
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

	//Starting OpenGL
	if(SDL_SetVideoMode(WIDTH * (DRAW_3D ? 2 : 1), HEIGHT, 0, SDL_OPENGL) == NULL) {
		printf("SDL_SetVideoMode error: %s\n", SDL_GetError());
		printf("Couldn't initialize OpenGL with the default settings! Trying other settings...\n");
		bool success = false;
		for(int i=0;i<2;i++) {
			if(i == 1) {
				printf("Trying with 16 bit colors...\n");
				SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 4);
				SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 4);
				SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 4);
				SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 4);
				SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 16);
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
				printf("Trying with 16 x multisampling.\n");
				if(SDL_SetVideoMode(WIDTH * (DRAW_3D ? 2 : 1), HEIGHT, 0, SDL_OPENGL) == NULL)
					printf("SDL_SetVideoMode error: %s\n", SDL_GetError());
				else success = true;
			}
			if(success) break;
			for(int j=8;j>=2;j/=2) {
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, j);
				printf("Trying with %i x multisampling.\n", j);
				if(SDL_SetVideoMode(WIDTH * (DRAW_3D ? 2 : 1), HEIGHT, 0, SDL_OPENGL) == NULL)
					printf("SDL_SetVideoMode error: %s\n", SDL_GetError());
				else { j = 0; success = true; }
			}
			if(success) break;
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
			printf("Trying without multisampling.\n");
			if(SDL_SetVideoMode(WIDTH * (DRAW_3D ? 2 : 1), HEIGHT, 0, SDL_OPENGL) == NULL)
				printf("SDL_SetVideoMode error: %s\n", SDL_GetError());
			else break;
			if(i == 1) throw 1;
		}
	}

	//Check the got values
	int value;
	char text[30];
	SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE, &value);
	sprintf(text, "Frame buffer size: %i / %i", value, 32);
	log(text);
	SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
	sprintf(text, "Depth buffer size: %i / %i", value, 24);
	log(text);
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value);
	if(value) log("Got doublebuffering");
	else log("Couldn't get doublebuffering");
	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &value);
	if(value) {
		SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &value);
		sprintf(text, "Got %i x multisampling", value);
		log(text);
	}
	else log("Didn't get multisampling");

	//OpenGL setting
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_FOG);
	const float fog_color[3] = {0.8, 0.8, 0.5};
	glFogfv(GL_FOG_COLOR, fog_color);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 0);
	glFogf(GL_FOG_END, 1000);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glColor4f(1, 1, 1, 1);
	glClearColor(1, 1, 0.5, 1);

	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65, (float)WIDTH / (float)HEIGHT, 0.1, 500);
	glMatrixMode(GL_MODELVIEW);

	//Create 3d texture
	glGenTextures(1, &g3D_texture);
	glBindTexture(GL_TEXTURE_2D, g3D_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Create ground texture
	glGenTextures(1, &ground_texture);
	glBindTexture(GL_TEXTURE_2D, ground_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	unsigned char *pixels = create_texture1(256, 256, 3);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	free(pixels);

	loop();

	glDeleteTextures(1, &g3D_texture);
	glDeleteTextures(1, &ground_texture);
	SDL_Quit();
	return 0;
}
