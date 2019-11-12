/* imsmap, Copyright (c) 1992 Juergen Nickelsen <nickel@cs.tu-berlin.de>
 * Derived from code by Markus Schirmer, TU Berlin.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * Revision History:
 * 10-Nov-2005: Ported on Windows XP by Fabio Nelli <fabio.nelli@virgilio.it>
 * 24-aug-92: jwz: hacked.
 * 17-May-97: jwz: hacked more.
 */

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <scrnsave.h>
#include <time.h>
#include "resource.h"
#include "options.h"
#include "utils.h"

long delay;
long ncolours;
long mode;

OPTION options[] =
{
/*	 Type            variable     control              default minimum maximum name */
    {LONG_EDIT_TYPE, &delay,      IDC_DELAY_EDIT,      200,    1,      10000,    "Delay"},
    {LONG_EDIT_TYPE, &ncolours,   IDC_COLOURS_EDIT,    256,     1,      256,    "Colours"},
    {LONG_EDIT_TYPE, &mode,		  IDC_MODE_EDIT,       0,      0,      3,         "Mode"},
    {-1, NULL, -1, -1, -1, -1, ""}
};

OPTIONS_STRUCT screenhack_options = 
{
    "ImsMap", options
};

#define NSTEPS 7
#define COUNT (1 << NSTEPS)
//#define MAX_NC 256
#define ITERATIONS 7
//int NC;
int MODE;
//Dimensioni della WINDOW
int SW,SH;
COLORREF* colore;
#define CELL(c, r) cell[((unsigned int)(c)) + ((unsigned int) (r)) * SW]
 
static int extra_krinkly_p;
static signed char *cell = NULL;

void init_map ()
{
  unsigned long fg_pixel = 0, bg_pixel = 0;
  int fg_h, bg_h;
  double fg_s, fg_v, bg_s, bg_v;	

  //INIT dei PARAMETRI 
  MODE = (int) mode;
  //NC = (int) ncolours;
  //Qui faccio una scelta : DA PARAMETRIZZARE
  extra_krinkly_p = !(random100() % 15); //DA PARAMETRIZZARE

  double distance, fg_H, bg_H, dh;

  RETRY:
      fg_h = random100() % 360;
      fg_s = random();
      fg_v = random();

      bg_h = fg_h;
      bg_s = fg_s;
      bg_v = fg_v;

      switch (MODE)
	{
	case 0:
	  bg_h = random100() % 360;
	  if (fg_v < 0.4)
	    goto RETRY;
	  distance = fg_h - bg_h;
	  if (distance < 0)
	    distance = -distance;
	  if (distance > 360)
	    distance = 180 - (distance - 180);
	  if (distance < 30)
	    goto RETRY;
	  break;

	case 1:
	  bg_s = random();
	  if (fg_v < 0.4)
	    goto RETRY;
	  distance = fg_s - bg_s;
	  if (distance < 0)
	    distance = -distance;
	  if (distance < 0.2)
	    goto RETRY;
	  break;

	case 2:
	  bg_v = random();
	  distance = fg_v - bg_v;
	  if (distance < 0)
	    distance = -distance;
	  if (distance < 0.4)
	    goto RETRY;
	  break;

	default:
	  bg_h = random100() % 360;
	  bg_s = random();
	  bg_v = random();

	  fg_H = ((double) fg_h) / 360;
	  bg_H = ((double) bg_h) / 360;
	  dh = fg_H - bg_H;
	  if (dh < 0) dh = -dh;
	  if (dh > 0.5) dh = 0.5 - (dh - 0.5);
	  distance = sqrt ((dh * dh) +
			   ((fg_s - bg_s) * (fg_s - bg_s)) +
			   ((fg_v - bg_v) * (fg_v - bg_v)));
	  if (distance < 0.2)
	    goto RETRY;
	}
	  
}


#define HEIGHT_TO_PIXEL(height)				\
	((height) < 0					\
	 ? (extra_krinkly_p				\
	    ? ncolours - ((-(height)) % ncolours)		\
	    : 0)					\
	 : ((height) >= ncolours				\
	    ? (extra_krinkly_p				\
	       ? (height) % ncolours			\
	       : ncolours-1)				\
	    : (height)))


static int set (unsigned int l,unsigned int c, unsigned int size, int height)
{
  int rang = 1 << (NSTEPS - size);
  height = height + (random100() % rang) - rang / 2;
  height = HEIGHT_TO_PIXEL(height);
  CELL (l, c) = height;
  return height;
}


static void draw_map (HDC hdc)
{
  int xstep, ystep, xnextStep, ynextStep;
  int x, y, i, x1, x2, y1, y2;
  //unsigned int pixel, qpixels [4];
  int pixel,qpixels[4];	

  int backwards = random100() & 1;

  cell = (signed char *) calloc (SW * SH, 1);
  if (cell == NULL)
    exit (1);

  CELL (0, 0) = 0;
  xstep = (backwards ? -COUNT : COUNT);
  ystep = COUNT;
  for (i = 0; i < ITERATIONS; i++)
  {
	xnextStep = xstep / 2;
	ynextStep = ystep / 2;
	for (x = (backwards ? SW-1 : 0);(backwards ? x >= 0 : x < SW);x += xstep)
	{
		x1 = x + xnextStep;
		if (x1 < 0)
		    x1 = SW-1;
		else if (x1 >= SW)
			x1 = 0;

		x2 = x + xstep;
		if (x2 < 0)
			x2 = SW-1;
		else if (x2 >= SW)
			x2 = 0;

		for (y = 0; y < SH; y += ystep)
	    {
			y1 = y + ynextStep;
			if (y1 < 0)
				y1 = SH-1;
			else if (y1 >= SH)
				y1 = 0;

			y2 = y + ystep;
			if (y2 < 0)
				y2 = SH-1;
			else if (y2 >= SH)
				y2 = 0;

            qpixels [0] = HEIGHT_TO_PIXEL(CELL(x,y));
			qpixels [1] = HEIGHT_TO_PIXEL(CELL(x, y2));
			qpixels [2] = HEIGHT_TO_PIXEL(CELL(x2,y));
			qpixels [3] = HEIGHT_TO_PIXEL(CELL(x2,y2));
			
			pixel = set (x, y1, i,((int) CELL (x, y) + (int) CELL (x, y2) + 1) / 2);
			if (pixel != qpixels[0] || pixel != qpixels[1] || pixel != qpixels[2] || pixel != qpixels[3])
				SetPixel(hdc, x, y1, colore[pixel]);

			pixel = set (x1, y, i,((int) CELL (x, y) + (int) CELL (x2, y) + 1) / 2);
			if (pixel != qpixels[0] || pixel != qpixels[1] || pixel != qpixels[2] || pixel != qpixels[3])
				SetPixel(hdc, x1, y, colore[pixel]);

			pixel = set (x1, y1, i,((int) CELL (x, y) + (int) CELL (x, y2) +
						(int) CELL (x2, y) + (int) CELL (x2, y2) + 2)/ 4);
			if (pixel != qpixels[0] || pixel != qpixels[1] || pixel != qpixels[2] || pixel != qpixels[3])
				SetPixel(hdc, x1, y1, colore[pixel]);
		}	
	}
  
    xstep = xnextStep;
    ystep = ynextStep;
	}
	free (cell);
	
}

void screenhack_init (HDC dc, HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	SH = rect.bottom;
	SW = rect.right;
	colore = colSpectrum(ncolours);
    init_map();

    /* Set the screenhack timer */
    SetTimer(hwnd, 10, delay, NULL);
}

void screenhack_loop(HDC hdc, HWND hWnd)
{
    RECT client;
    GetClientRect(hWnd, &client);
    FillRect(hdc, &client, (HBRUSH)GetStockObject(BLACK_BRUSH));
	write_label(hdc,SW,SH);
	colore = colSpectrum(ncolours);
    draw_map(hdc);
	init_map();
	Sleep(2000);
}

