/** landscape.cpp **/

#include "landscape.hpp"
#include "math.hpp"

/*

	NOTES:

	size should ALWAYS be 2^x
	setting LIGHTING_SIZE too small might cause calculation errors
	increasing LIGHTING_SIZE will only slow down the calculation, not affect the lighting quality
	LIGHTING_SMOOTHING should be at least 4, or 12 if CHEAP_LIGHTING is enabled, to decrease the jagginess of the lighting
	CHEAP_LIGHTING approximately halves the loading time but will cause striped shadows if LIGHTING_SMOOTHING is too small
	if CHEAP_LIGHTING is enables real SHADOW will be the mean of SHADOW and 1
	only size really affects the loading time, nothing else


	HOW TO MODIFY THE LANDSCAPE:

	To increase precision:
	size * 2
	randomlessness * 2

	To increase size:
	size * 2
	WORLD_SIZE * 2

	To increase flattness:
	increase randomlessness

*/

/*
	The lighting is calculated in a simple way by targetting the camera into the "sun" and rendering the scene with openGL in every vertex.
	If the sky is seen the vertex is lit.
	Because the scene is first drawn with the graphic card and the frame buffer analyzed with the prosessor after that, it is pretty slow.
*/

#define WORLD_SIZE 96
#define LIGHTING_SIZE 32
#define LIGHTING_SMOOTHING 12
#define CHEAP_LIGHTING
#define SHADOW 0.0

extern GLuint ground_texture;

void draw2d();
void draw3d();

landscape_c::landscape_c(): size(128) {
	const int timer = SDL_GetTicks();
	coords = new coordinate[(size + 1) * (size + 1)];
	for(int i=0;i<=size;i++) {
		for(int j=0;j<=size;j++) {
			coords[i * (size + 1) + j].x = linear_function(0, size, -WORLD_SIZE, WORLD_SIZE, j);
			coords[i * (size + 1) + j].y = 0;
			coords[i * (size + 1) + j].z = linear_function(0, size, -WORLD_SIZE, WORLD_SIZE, i);
		}
	}
	coords[0].y = randomf(0, 100);
	coords[size].y = randomf(0, 100);
	coords[size * (size + 1)].y = randomf(0, 100);
	coords[size * (size + 1) + size].y = randomf(0, 100);

	//The landscape algorithm - random midpoint displacement algorithm
	const float randomlessness = 1.0;
	int a = 2;
	int b = 1;
	int c = size;
	/*
	      b
	  ->0.0.0
	  | .....
	 a| 0.0.0
	  | .....
	  ->0.0.0.0
	      \---/
		  ^ c
	   i--/
	*/
	for(int i=size/2;i!=0;i/=2) {
		for(int j=0;j<a;j++) {
			for(int k=0;k<b;k++) {
				coords[size * j / (a - 1) * (size + 1) + i + k * c].y
					= (coords[size * j / (a - 1) * (size + 1) + i + k * c - c / 2].y
					+ coords[size * j / (a - 1) * (size + 1) + i + k * c + c / 2].y)
					/ 2.0 + randomf(-(float)i / randomlessness, (float)i / randomlessness);
				coords[(i + k * c) * (size + 1) + size * j / (a - 1)].y
					= (coords[(i + k * c - c / 2) * (size + 1) + size * j / (a - 1)].y
					+ coords[(i + k * c + c / 2) * (size + 1) + size * j / (a - 1)].y)
					/ 2.0 + randomf(-(float)i / randomlessness, (float)i / randomlessness);
			}
		}
		for(int j=0;j<a-1;j++) {
			for(int k=0;k<b;k++) {
				coords[(size * j / (a - 1) + i) * (size + 1) + i + k * c].y
					= (coords[(size * j / (a - 1) + i - c / 2) * (size + 1) + i + k * c - c / 2].y
					+ coords[(size * j / (a - 1) + i + c / 2) * (size + 1) + i + k * c - c / 2].y
					+ coords[(size * j / (a - 1) + i - c / 2) * (size + 1) + i + k * c + c / 2].y
					+ coords[(size * j / (a - 1) + i + c / 2) * (size + 1) + i + k * c + c / 2].y)
					/ 4.0f + randomf(-(float)i / randomlessness, (float)i / randomlessness);
			}
		}
		a = a * 2 - 1;
		b*= 2;
		c/= 2;
	}

	//Pull edges down
	for(int i=0;i<=size;i++) {
		coords[i].y = -100;
		coords[size * (size + 1) + i].y = -100;
		coords[i * (size + 1)].y = -100;
		coords[i * (size + 1) + size].y = -100;
	}

	//Create the OpenGL drawing stuff...
	vertexes = new float[(size + 1) * (size + 1) * 3];
	colors = new float[(size + 1) * (size + 1) * 3];
	texcoords = new float[(size + 1) * (size + 1) * 2];
	index = new unsigned int[size * size * 2 * 3];
	for(int i=0;i<(size+1)*(size+1);i++) {
		vertexes[i * 3] = coords[i].x;
		vertexes[i * 3 + 1] = coords[i].y;
		vertexes[i * 3 + 2] = coords[i].z;
		const float tempcolor = clampf(linear_function(0, 100, 0.5, 1, coords[i].y), 0, 1);
		colors[i * 3] = tempcolor * randomf(0.78, 0.82);
		colors[i * 3 + 1] = tempcolor * randomf(0.58, 0.62);
		colors[i * 3 + 2] = 0;
	}
	for(int i=0;i<=size;i++) {
		for(int j=0;j<=size;j++) {
			texcoords[(i * (size + 1) + j) * 2] = j / 8.0;
			texcoords[(i * (size + 1) + j) * 2 + 1] = i / 8.0;
		}
	}
	for(int i=0;i<size;i++) {
		for(int j=0;j<size;j++) {
			index[(i * size + j) * 6] = i * (size + 1) + j + 1;
			index[(i * size + j) * 6 + 1] = i * (size + 1) + j;
			index[(i * size + j) * 6 + 2] = (i + 1) * (size + 1) + j;
			index[(i * size + j) * 6 + 3] = i * (size + 1) + j + 1;
			index[(i * size + j) * 6 + 4] = (i + 1) * (size + 1) + j;
			index[(i * size + j) * 6 + 5] = (i + 1) * (size + 1) + j + 1;
		}
	}

	//Calculate lighting - THIS TAKES LONG!
	float *lighting = new float[(size + 1) * (size + 1)];
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_FOG);
	glViewport(0, 0, LIGHTING_SIZE, LIGHTING_SIZE);
	unsigned char framebuffer[3];
	glVertexPointer(3, GL_FLOAT, sizeof(float[3]), vertexes);
	glColor4f(1, 1, 1, 1);
	for(int i=0;i<(size+1)*(size+1);i++) {
		//Render loading bar
		if(i % 1000 == 0) {
			glClear(GL_COLOR_BUFFER_BIT);
			glViewport(0, 0, WIDTH, HEIGHT);
			draw2d();
			glColor4f(0, 0, 0, 1);
			glBegin(GL_TRIANGLE_STRIP);
				glVertex2f(-1, -0.1); glVertex2f((float)i / float((size + 1) * (size + 1)) * 2 - 1, -0.1);
				glVertex2f(-1, 0.1); glVertex2f((float)i / float((size + 1) * (size + 1)) * 2 - 1, 0.1);
			glEnd();
			SDL_GL_SwapBuffers();
			glColor4f(1, 1, 1, 1);
			draw3d();
			glViewport(0, 0, LIGHTING_SIZE, LIGHTING_SIZE);
		}
		//Render scene for lighting calculation
		#ifdef CHEAP_LIGHTING
		if(i % 2 == 0) lighting[i] = 1;
		else {
		#endif
			glClear(GL_COLOR_BUFFER_BIT);
			glLoadIdentity();
			gluLookAt(coords[i].x,     coords[i].y,       coords[i].z,
					  coords[i].x + 1, coords[i].y + 0.75, coords[i].z + 1, 0, 1, 0);
			glDrawElements(GL_TRIANGLES, size * size * 2 * 3, GL_UNSIGNED_INT, index);
			glReadPixels(LIGHTING_SIZE / 2, LIGHTING_SIZE / 2, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);
			if(framebuffer[0] == 255
				&& framebuffer[1] == 255
				&& framebuffer[2] == 255)
					lighting[i] = SHADOW;
				else lighting[i] = 1;
		#ifdef CHEAP_LIGHTING
		}
		#endif
	}
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_FOG);
	glViewport(0, 0, WIDTH, HEIGHT);
	delete [] coords;
	//Smoothen lighting
	for(int a=0;a<LIGHTING_SMOOTHING;a++) {
		float *old_lighting = lighting;
		lighting = new float[(size + 1) * (size + 1)];
		for(int i=0;i<size+1;i++) {
			for(int j=0;j<size+1;j++) {
				lighting[i * (size + 1) + j] = 0;
				lighting[i * (size + 1) + j]+= old_lighting[i * (size + 1) + j];
				lighting[i * (size + 1) + j]+= i - 1 >= 0 ? old_lighting[(i - 1) * (size + 1) + j] : 1;
				lighting[i * (size + 1) + j]+= i + 1 <= size ? old_lighting[(i + 1) * (size + 1) + j] : 1;
				lighting[i * (size + 1) + j]+= j - 1 >= 0 ? old_lighting[i * (size + 1) + j - 1] : 1;
				lighting[i * (size + 1) + j]+= j + 1 <= size ? old_lighting[i * (size + 1) + j + 1] : 1;
				lighting[i * (size + 1) + j]/= 5.0;
			}
		}
		delete [] old_lighting;
	}
	//Apply lighting
	for(int i=0;i<(size+1)*(size+1);i++) {
		colors[i * 3]*= lighting[i];
		colors[i * 3 + 1]*= lighting[i];
		colors[i * 3 + 2]*= lighting[i];
	}
	delete [] lighting;
	printf("New landscape generated in %f seconds.\n", float(SDL_GetTicks() - timer) / 1000.0);
}

landscape_c::~landscape_c() {
	delete [] vertexes;
	delete [] colors;
	delete [] texcoords;
	delete [] index;
}

void landscape_c::draw() const {
	glBindTexture(GL_TEXTURE_2D, ground_texture);
	glVertexPointer(3, GL_FLOAT, sizeof(float[3]), vertexes);
	glColorPointer(3, GL_FLOAT, sizeof(float[3]), colors);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float[2]), texcoords);
	glDrawElements(GL_TRIANGLES, size * size * 2 * 3, GL_UNSIGNED_INT, index);
}
