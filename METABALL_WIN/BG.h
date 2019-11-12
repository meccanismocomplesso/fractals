
#ifndef _BG_
#define _BG_

#include <math.h>
#include "Globals.h"

namespace GLProjection
{

// Sets an orthographic projection. x: 0..width, y: 0..height
static void BeginOrtho2D(float width, float height)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}


// Sets an orthographic projection. x: x1..x2, y: y1..y2
static void BeginOrtho2D(float x1, float y1, float x2, float y2)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(x1, x2, y1, y2);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

// Sets a perspective projection with a different field-of-view
static void BeginPerspective(float fovy, float xratio)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(fovy, xratio, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

// Restore the previous projection
static void EndProjection()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

};

// ************************************************************************

namespace bg
{
	// Draws a static background
	void DrawStatic()
	{
		GLProjection::BeginOrtho2D(1, 1);
		glDepthMask(GL_FALSE);
		
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);
		glEnd();

		glDepthMask(GL_TRUE);
		GLProjection::EndProjection();
	}

	// Draws a background shifted with a random value, good
	// for drawing whitenoise and such.
	void DrawShaky()
	{
		GLProjection::BeginOrtho2D(1, 1);
		glDepthMask(GL_FALSE);

		float x = (rand()%10)/100.0f;
		float y = (rand()%10)/100.0f;

		glTranslatef(-x, -y, 0);
		
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(1.2f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(1.2f, 1.2f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.2f);
		glEnd();

		glDepthMask(GL_TRUE);
		GLProjection::EndProjection();
	}

	// Draws a background with a rotating cube viewed at a
	// very high field-of-view
	void DrawPlasma(float time)
	{
		GLProjection::BeginPerspective(140, 1.33f);
		glDepthMask(GL_FALSE);

		glRotatef(time*32, 1, 0, 0);
		glRotatef(time*67, 0, 1, 0);

		glBegin(GL_QUADS);
			// Front
			glNormal3f(0, 0, 1);
			glTexCoord2f(0, 0); glVertex3f(-1, -1, -1);
			glTexCoord2f(1, 0); glVertex3f( 1, -1, -1);
			glTexCoord2f(1, 1); glVertex3f( 1,  1, -1);
			glTexCoord2f(0, 1); glVertex3f(-1,  1, -1);

			// Left
			glNormal3f(1, 0, 0);
			glTexCoord2f(0, 0); glVertex3f(-1, -1,  1);
			glTexCoord2f(1, 0); glVertex3f(-1, -1, -1);
			glTexCoord2f(1, 1); glVertex3f(-1,  1, -1);
			glTexCoord2f(0, 1); glVertex3f(-1,  1,  1);

			// Bottom
			glNormal3f(0, 1, 0);
			glTexCoord2f(0, 0); glVertex3f(-1, -1,  1);
			glTexCoord2f(1, 0); glVertex3f( 1, -1,  1);
			glTexCoord2f(1, 1); glVertex3f( 1, -1, -1);
			glTexCoord2f(0, 1); glVertex3f(-1, -1, -1);

			// Back
			glNormal3f(0, 0, -10);
			glTexCoord2f(0, 0); glVertex3f( 1, -1,  1);
			glTexCoord2f(1, 0); glVertex3f(-1, -1,  1);
			glTexCoord2f(1, 1); glVertex3f(-1,  1,  1);
			glTexCoord2f(0, 1); glVertex3f( 1,  1,  1);

			// Right
			glNormal3f(0, -1, 0);
			glTexCoord2f(0, 0); glVertex3f( 1, -1, -1);
			glTexCoord2f(1, 0); glVertex3f( 1, -1,  1);
			glTexCoord2f(1, 1); glVertex3f( 1,  1,  1);
			glTexCoord2f(0, 1); glVertex3f( 1,  1, -1);

			// Top
			glNormal3f(0, -1, 0);
			glTexCoord2f(0, 0); glVertex3f(-1,  1,  1);
			glTexCoord2f(1, 0); glVertex3f( 1,  1,  1);
			glTexCoord2f(1, 1); glVertex3f( 1,  1, -1);
			glTexCoord2f(0, 1); glVertex3f(-1,  1, -1);
			
		glEnd();

		glDepthMask(GL_TRUE);
		GLProjection::EndProjection();
	}

	// Draws a background rotating around its center
	void DrawRotateTile(float rot)
	{
		GLProjection::BeginOrtho2D(-1, -1, 1, 1);
		glDepthMask(GL_FALSE);
		
		glRotatef(rot, 0, 0, 1);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);

			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(-1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1.0f);

			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(-1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, -1.0f);

			glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, -1.0f);
		glEnd();

		glDepthMask(GL_TRUE);
		GLProjection::EndProjection();
	}

	// Draw a sine distorted background
	void DrawDistort(float time)
	{
		GLProjection::BeginOrtho2D(-1, -1, 1, 1);
		glScalef(1.2f, 1.2f, 1.0f);

		glDepthMask(GL_FALSE);

		//time *= ;

		float step;
		switch(prefs::polylevel)
		{
		case 0:
			step = 0.3f;
			break;
		case 1:
			step = 0.2f;
			break;
		case 2:
			step = 0.1f;
			break;
		case 3:
			step = 0.05f;
			break;
		}
		float kx = 0.04f;
		float ky = 0.03f;

		float ax = 14.3f;
		float bx = 7.2f;
		float cx = 9.12f;
		float dx = 21.7f;

		for(float a = -1; a < 1; a += step)
		{
			glBegin(GL_TRIANGLE_STRIP);
				for(float b = -1; b <= 1.01f; b += step)
				{
					glTexCoord2f(b*0.5f+0.5f, a*0.5f+0.5f);
					glVertex2f(b + sinf(b*dx+time*bx) * kx, a + sinf(a*ax+time*0.7f*cx) * ky);
					
					glTexCoord2f(b*0.5f+0.5f, (a+step)*0.5f+0.5f);
					glVertex2f(b + sinf(b*dx+time*bx) * kx, a + step + sinf((a+step)*ax+time*0.7f*cx) * ky);
				}
			glEnd();
		}

		glDepthMask(GL_TRUE);
		GLProjection::EndProjection();
	}

	// Draw a waving background
	void DrawWaving(float time)
	{
		GLProjection::BeginOrtho2D(-1, -1, 1, 1);
		glScalef(1.2f, 1.2f, 1.0f);

		glDepthMask(GL_FALSE);

		time *= 0.5f;

		float step;
		switch(prefs::polylevel)
		{
		case 0:
			step = 0.3f;
			break;
		case 1:
			step = 0.2f;
			break;
		case 2:
			step = 0.1f;
			break;
		case 3:
			step = 0.05f;
			break;
		}
		float k = 0.2f;

		for(float a = -1; a < 1; a += step)
		{
			glBegin(GL_TRIANGLE_STRIP);
				for(float b = -1; b <= 1.01f; b += step)
				{
					glTexCoord2f(b*0.5f+0.5f, a*0.5f+0.5f);
					glVertex2f(b + sinf(a+time) * k * (sqrtf(2) - sqrtf(a*a+b*b)), a + sinf(b+time*0.7f) * k * (sqrtf(2) - sqrtf(a*a+b*b)));
					
					glTexCoord2f(b*0.5f+0.5f, (a+step)*0.5f+0.5f);
					glVertex2f(b + sinf(a+step+time) * k *  (sqrtf(2) - sqrtf((a+step)*(a+step)+b*b)), a + step + sinf(b+time*0.7f) * k * (sqrtf(2) - sqrtf((a+step)*(a+step)+b*b)));
				}
			glEnd();
		}

		glDepthMask(GL_TRUE);
		GLProjection::EndProjection();
	}

	// And finally the zooming background
	void DrawZooming(float time)
	{
		GLProjection::BeginOrtho2D(1, 1);
		glDepthMask(GL_FALSE);

		time *= 0.5f;
		
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f+time, 0.0f+time); glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f-time, 0.0f+time); glVertex2f(1.0f, 0.0f);
			glTexCoord2f(1.0f-time, 1.0f-time); glVertex2f(1.0f, 1.0f);
			glTexCoord2f(0.0f+time, 1.0f-time); glVertex2f(0.0f, 1.0f);
		glEnd();

		glDepthMask(GL_TRUE);
		GLProjection::EndProjection();
	}
};

#endif // _BG_
