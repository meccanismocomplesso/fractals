/* Copyright (c) 1994 M.Dobie <mrd@ecs.soton.ac.uk>
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 * 
 * 10-Mar-06: Released on the Blue Ant site <www.the-blueant.com>
 * 27-Nov-05:  - Ported on MS Windows OS
 *			   - Started from swirl.cpp code and created an animation
 *				 adding my colour spectrum insted of colour map
 *			   - Added the Vertigo algorythm created by me. 
 *				<fabio.nelli@virgilio.it>
 * 13-May-97: jwz@jwz.org: turned into a standalone program.
 * 21-Apr-95: improved startup time for TrueColour displays
 *            (limited to 16bpp to save memory) S.Early <sde1000@cam.ac.uk>
 * 09-Jan-95: fixed colour maps (more colourful) and the image now spirals
 *            outwards from the centre with a fixed number of points drawn
 *            every iteration. Thanks to M.Dobie <mrd@ecs.soton.ac.uk>.
 * 1994:      written.   Copyright (c) 1994 M.Dobie <mrd@ecs.soton.ac.uk>
 *            based on original code by R.Taylor
 */

#include <windows.h>
#include <scrnsave.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <time.h>
#include "resource.h"
#include "options.h"
#include "utils.h"

long delay;
long ncolours;
#define GRID_SIZE 4


OPTION options[] =
{
/*	 Type            variable     control              default minimum maximum name */
    {LONG_EDIT_TYPE, &delay,      IDC_DELAY_EDIT,      200,    1,      10000,    "Delay"},
    {LONG_EDIT_TYPE, &ncolours,   IDC_COLOURS_EDIT,    256,     1,      256,    "Colours"},
   {-1, NULL, -1, -1, -1, -1, ""}
};

OPTIONS_STRUCT screenhack_options = 
{
    "Vertigo", options
};


COLORREF *colour_array;
bool 	done = false;
int 	knot_m = 3;
int 	knot_m2 = 9;
int   counter = 1;
int   dim_x = 170;
int 	dim_y = 170;

struct inter_source {
  int x; 
  int y;
};

struct inter_context {

  HDC dpy;
  HDC mem_dc;
  HWND   win;
  HBITMAP bitmap;
  unsigned char *bitmap_data;
  long bits_per_pixel;

  int grid_size;
  //int colors;
  int w;
  int h;
  struct inter_source* source;
};

struct inter_context c;

// ********************************************************************************

static int do_point(int i, int j)
{
    int         value, add;
    double      dx, dy, theta, dist;
    int         qcolours;
    double      rads;
    int 	    knot_x,knot_y;

    qcolours = ncolours / 4;
    rads = (double) ncolours / (2.0 * M_PI);
    value = 0;

    /* nothing to add at first */
    add = 0;    

    /* ************* PRIMO KNOT ************************ */
    /*  case VERTIGO by Fabio Nelli */    

    knot_x = 100;
    knot_y = 100;

    //distanza dal knot
    dx = i - knot_x;
    dy = j - knot_y;
    dist = sqrt(dx * dx + dy * dy);

     if (dy == 0.0 && dx == 0.0)
           theta = 1.0;
        else
           theta = (atan2(dy, dx) + M_PI) / M_PI;
     //  0.2 è l'apertura del vortice 
     add = (int) (ncolours * theta + qcolours * dist *0.4 * exp(-0.01 * dist) - counter * 10);
        
        if (knot_m > 0)
            value += add;
        else
            value -= add;
  
  /* **************** SECONDO KNOT *********************** */
  /* case SPHERE */

    knot_x = 155;
    knot_y = 100;
    dx = i - knot_x;
    dy = j - knot_y;
    dist = sqrt(dx * dx + dy * dy);

    add = 0;
	
    add = (int) (ncolours / (1.0 + 0.01 * (1 - 0.01) * abs(knot_m2) * dist));

    
    if (knot_m > 0)
       value += add;
    else
       value -= add;

    /* ****************** TERZO KNOT ****************************** */
    /*  case WHEEL  */

    knot_x = 20;
    knot_y = -10;
    dx = i - knot_x;
    dy = j - knot_y;
    dist = sqrt(dx * dx + dy * dy);

     	if (dy == 0.0 && dx == 0.0)
           theta = 1.0;
        else
           theta = (atan2(dy, dx) + M_PI) / M_PI;
        if (theta < 1.0)
           add = (int) (ncolours * theta +
           sin(0.1 * (1 - 0.01*counter) * knot_m * dist) * qcolours * exp(-0.01 * dist));
        else
           add = (int) (ncolours * (theta - 1.0) +
           sin(0.1 * (1 - 0.01*counter) * knot_m * dist) * qcolours * exp(-0.01 * dist));

    if (knot_m > 0)
       value += add;
    else
       value -= add;

	/* ********************************************************** */


    /* ******************** QUARTO KNOT ********************************** */
    /*  case PICASSO  */
/*
    knot_x = 60;
    knot_y = 60;
    dx = i - knot_x;
    dy = j - knot_y;
    dist = sqrt(dx * dx + dy * dy);

    add = (int) (dcolours * fabs(cos(0.002 * (1 + 0.01*counter)* knot_m * dist)));

    if (knot_m > 0)
       value += add;
    else
       value -= add;
*/ 
   /* ********************************************************** */
    /* ******************** QUINTO KNOT ********************************** */
    /*  case RAY */
/*
    knot_x = 60;
    knot_y = 60;
    dx = i - knot_x;
    dy = j - knot_y;
    dist = sqrt(dx * dx + dy * dy);

    if (dy == 0.0 && dx == 0.0)
                add = 0;
    else
                add = (int) (dcolours * fabs(sin(2.0 * (1 - 0.01*counter) * atan2(dy, dx))));

    if (knot_m > 0)
       value += add;
    else
       value -= add;
*/ 
   /* ********************************************************** */
    if (value >= 0)
        value = (value % ncolours) + 2;
    else
        value = ncolours - (abs(value) % (ncolours - 1));
    value = value % ncolours;
    return ((int) value);
}

/****************************************************************/

void init(HDC dpy, HWND win, struct inter_context* c) 
{
  int i;
  memset (c, 0, sizeof(*c));

  c->dpy = dpy;
  c->win = win;

  RECT client;
  GetClientRect(win, &client);
  c->w = client.right - client.left;
  c->h = client.bottom - client.top;
  c->mem_dc = CreateCompatibleDC(c->dpy);
  c->bits_per_pixel = GetDeviceCaps(c->dpy, BITSPIXEL);
  
  /* Allocate enough space for the worst case of 32 bits per pixel */
  c->bitmap_data = (unsigned char *)malloc(c->w * c->h * 4);
  c->grid_size = GRID_SIZE;

  colour_array = colSpectrumCoherent(ncolours);

}

void drawPointBitmap(int i,int j,int colore,struct inter_context* c) 
{
    int g = GRID_SIZE;
    int k;
    if ((c->bits_per_pixel == 32)||(c->bits_per_pixel == 24))
    {
    	for(k = 0; k < g; k++)
          ((unsigned long *)(c->bitmap_data))[g*(i+(j*c->w))+k] = colour_array[colore];
    }else if (c->bits_per_pixel == 16)
    {
      for(k = 0; k < g; k++)
	    ((unsigned short *)(c->bitmap_data))[g*(i+(j*c->w))+k] = (unsigned short)colour_array[colore];
    }
    for(k = 1; k < g; k++)
    {
       if ((c->bits_per_pixel == 32)||(c->bits_per_pixel == 24))
       {
                memcpy(((unsigned long *)(c->bitmap_data)) + (c->w * ((g*j)+k)),
                        ((unsigned long *)(c->bitmap_data)) + (c->w * (g*j)),
                        c->w * 4);
       }
       else if (c->bits_per_pixel == 16)
       {
                memcpy(((unsigned short *)(c->bitmap_data)) + (c->w * ((g*j)+k)),
                        ((unsigned short *)(c->bitmap_data)) + (c->w * (g*j)),
                        c->w * 2);
       }
    }
}

void drawBitmap(struct inter_context* c)
{
  c->bitmap = CreateBitmap(c->w, c->h, 1, c->bits_per_pixel, c->bitmap_data);
  HBITMAP old_bmp;
  if ((old_bmp = (HBITMAP)SelectObject(c->mem_dc, c->bitmap)) == NULL)
  {
      MessageBox(c->win, "Error selecting bitmap", "Error", MB_OK);
  }
  BitBlt(c->dpy, 0, 0, c->w, c->h, c->mem_dc, 0, 0, SRCCOPY);
  SelectObject(c->mem_dc, old_bmp);
  DeleteObject(c->bitmap);
}

void draw(struct inter_context* c)
{
	int g = GRID_SIZE;
	for(int j=1; j < c->h/g; j++)
		for(int i=1; i < c->w/g; i++)
			drawPointBitmap(i,j,do_point(i,j),c);
	drawBitmap(c);	
}

void screenhack_init (HDC hdc, HWND hwnd)
{
    init(hdc, hwnd, &c);
	
	/* Set the screenhack timer */
    SetTimer(hwnd, 10, delay, NULL);
}

void screenhack_loop(HDC hdc, HWND hwnd)
{

	if(counter < 500){
		draw(&c);
		if(!done)
			counter++;
		else
			counter--;
		if((counter % 50) == 0)
			init(hdc, hwnd, &c);
	}
	if(counter == 500){
		done = true;
		counter = 499;
	}
	if(counter == 0){
		counter = 1;
		done = false;
	}			
	
	
}
