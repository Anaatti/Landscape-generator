/** draw_3d.cpp **/

#include "global.hpp"

extern GLuint g3D_texture;

void draw2d() {
	//glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
}

void draw3d() {
	//glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

/*
	This would draw the scene in cross-eyed 3d.
	However, the support for that is incomplete. At least the camera placing should be fixed for the camera_c class in its draw function.
	This feature can be turned on in the global.hpp
*/
void draw_3d(const unsigned char ddd_state) {
	glBindTexture(GL_TEXTURE_2D, g3D_texture);
	if(ddd_state == 0) {
		glViewport(0, 0, BLUR_WIDTH, BLUR_HEIGHT);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, BLUR_WIDTH, BLUR_HEIGHT, 0);
	}
	else {
		glViewport(WIDTH, 0, WIDTH, HEIGHT);
		draw2d();
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0, 0.0); glVertex3f(-1, -1, -1);
			glTexCoord2f((float)WIDTH / (float)BLUR_WIDTH, 0.0); glVertex3f(1, -1, -1);
			glTexCoord2f(0.0, (float)HEIGHT / (float)BLUR_HEIGHT); glVertex3f(-1, 1, -1);
			glTexCoord2f((float)WIDTH / (float)BLUR_WIDTH, (float)HEIGHT / (float)BLUR_HEIGHT); glVertex3f(1, 1, -1);
		glEnd();
		glViewport(WIDTH - 1, 0, 2, HEIGHT);
		glBegin(GL_TRIANGLE_STRIP);
			glVertex2f(-1, -1); glVertex2f(1, -1); glVertex2f(-1, 1); glVertex2f(1, 1);
		glEnd();
		draw3d();
		SDL_GL_SwapBuffers();
	}
	glViewport(0, 0, WIDTH, HEIGHT);
}
