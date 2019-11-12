/*
	Progress.h
*/

#ifndef _PROGRESS_
#define _PROGRESS_

#include "BG.h"

// Simple func to display the loading progress
void progress(HWND *wnd, float x)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	Texture::Enable(false);

	glColor3f(1, 1, 1);

	GLProjection::BeginOrtho2D(0, 600, 800, 0);

	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
		glVertex2i( 95, 285);
		glVertex2i(705, 285);
		glVertex2i(705, 315);
		glVertex2i( 95, 315);
	glEnd();
	glLineWidth(1);

	glBegin(GL_QUADS);
		glVertex2i(100      , 290);
		glVertex2i(100+(int)(600*x), 290);
		glVertex2i(100+(int)(600*x), 310);
		glVertex2i(100      , 310);
	glEnd();

	GLProjection::EndProjection();

	Texture::Enable(true);

	glEnable(GL_DEPTH_TEST);
	//wnd->SwapBuffers();
}

#endif //_PROGRESS_
