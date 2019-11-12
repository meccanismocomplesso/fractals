/* interference.c --- colored fields via decaying sinusoidal waves.
 * An entry for the RHAD Labs Screensaver Contest.
 *
 * Author: Hannu Mallat <hmallat@cs.hut.fi>
 *
 * Copyright (C) 1998 Hannu Mallat.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 * decaying sinusoidal waves, which extend spherically from their
 * respective origins, move around the plane. a sort of interference
 * between them is calculated and the resulting twodimensional wave
 * height map is plotted in a grid, using softly changing colours.
 *
 * not physically (or in any sense) accurate, but fun to look at for 
 * a while. you may tune the speed/resolution/interestingness tradeoff 
 * with X resources, see below.
 *
 * Created      : Wed Apr 22 09:30:30 1998, hmallat
 * Last modified: Wed Apr 22 09:30:30 1998, hmallat
 * Last modified: Sun Aug 31 23:40:14 2003, 
 *              david slimp <rock808@DavidSlimp.com>
 * 		added -hue option to specify base color hue
 * Revision History:
 * 01-Sep-2005: Ported on Windows XP by Fabio Nelli <fabio.nelli@virgilio.it>
 *
 * TODO:
 *
 *    This really needs to be sped up.
 *
 *    I've tried making it use XPutPixel/XPutImage instead of XFillRectangle,
 *    but that doesn't seem to help (it's the same speed at gridsize=1, and
 *    it actually makes it slower at larger sizes.)
 *
 *    I played around with shared memory, but clearly I still don't know how
 *    to use the XSHM extension properly, because it doesn't work yet.
 *
 *    Hannu had put in code to use the double-buffering extension, but that
 *    code didn't work for me on Irix.  I don't think it would help anyway,
 *    since it's not the swapping of frames that is the bottleneck (or a source
 *    of visible flicker.)
 *
 *     -- jwz, 4-Jun-98
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
long count;
long speed;
long radius;
long grid_size;
long shift;

OPTION options[] =
{
/*	 Type            variable     control              default minimum maximum name */
    {LONG_EDIT_TYPE, &delay,      IDC_DELAY_EDIT,      5,    1,      10000,    "Delay"},
    {LONG_EDIT_TYPE, &ncolours,   IDC_COLOURS_EDIT,    256,     1,     256,    "Colours"},
	{LONG_EDIT_TYPE, &count,	  IDC_COUNT_EDIT,		3,     1,        10,    "Count"},
	{LONG_EDIT_TYPE, &speed,	  IDC_SPEED_EDIT,		10,     1,       100,    "Speed"},
	{LONG_EDIT_TYPE, &radius,	  IDC_RADIUS_EDIT,	   800,     1,      1000,    "Radius"},
	{LONG_EDIT_TYPE, &grid_size,  IDC_GRIDSIZE_EDIT,	 4,     1,        16,    "GridSize"},
	{LONG_EDIT_TYPE, &shift,	  IDC_SHIFT_EDIT,		60,     1,       100,    "Shift"},
	{-1, NULL, -1, -1, -1, -1, ""}
};

OPTIONS_STRUCT screenhack_options = 
{
    "StrangeIcon", options
};

int sh,sw;

COLORREF *colours;

struct inter_source {
  int x; 
  int y;
  double x_theta;
  double y_theta;
};

struct inter_context {

  HDC dpy;
  HDC mem_dc;
  HWND   win;
  HBITMAP bitmap;
  unsigned char *bitmap_data;
  long bits_per_pixel;

  int count;
  int grid_size;
  int colors;
  int speed;
  int delay;
  int shift;
  int radius;

  int w;
  int h;
  COLORREF *pal;
  int* wave_height;
  struct inter_source* source;
};

struct inter_context c;

void inter_init(HDC dpy, HWND win, struct inter_context* c) 
{
  double H[3], S[3], V[3];
  int i;
  unsigned long valmask = 0;

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

  c->count = count;
  c->grid_size = grid_size;
  c->colors = ncolours;
  c->speed = speed;
  c->shift = shift;
  c->radius = radius;

  c->pal = (COLORREF *)calloc(c->colors, sizeof(COLORREF));

      H[0] = (float)(rand() % 3600) / 10.0; 
      H[1] = H[0] + c->shift < 360.0 ? H[0]+c->shift : H[0] + c->shift-360.0; 
      H[2] = H[0];
      S[0] = S[1] = S[2] = 1.0;
      V[0] = V[1] = V[2] = 1.0;

    long loop;


    for (loop = 0; loop < c->colors; loop++)
    {
        double h, s, v;
        long r, g, b;

        if (loop < c->colors / 2)
        {
            if (H[1] > H[0])
            {	h = ((H[1] - H[0]) / (c->colors / 2) * loop) + H[0];
            }else{
                	h = ((H[1] - H[0] + 360.0) / (c->colors / 2) * loop) + H[0];
            }
            s = ((S[1] - S[0]) / (c->colors / 2) * loop) + S[0];
            v = ((V[1] - V[0]) / (c->colors / 2) * loop) + V[0];
        }else{
            if (H[2] > H[1])
            {
                h = ((H[2] - H[1]) / (c->colors / 2) * (loop - (c->colors / 2))) + H[1];
            }else{
                h = ((H[2] - H[1] + 360.0) / (c->colors / 2) * (loop - (c->colors / 2))) + H[1];
            }
            s = ((S[2] - S[1]) / (c->colors / 2) * (loop - (c->colors / 2))) + S[1];
            v = ((V[2] - V[1]) / (c->colors / 2) * (loop - (c->colors / 2))) + V[1];
        }
        while (h > 360.0) h -= 360.0;
        while (h < 0.0)   h += 360.0;

        if (h < 60.0){
            r = 255;
            g = (long)(h * 255.0 / 60.0);
            b = 0;
        }else if (h < 120.0){
            r = (long)((60.0 - (h - 60.0)) * 255.0 / 60.0);
            g = 255;
            b = 0;
        }else if (h < 180.0){
            r = 0;
            g = 255;
            b = (long)((h - 120.0) * 255.0 / 60.0);
        }else if (h < 240.0){
            r = 0;
            g = (long)((60.0 - (h - 180.0)) * 255.0 / 60.0);
            b = 255;
        }else if (h < 300.0){
            r = (long)((h - 240.0) * 255.0 / 60.0);
            g = 0;
            b = 255;
        }else{
            r = 255;
            g = 0;
            b = (long)((60.0 - (h - 300.0)) * 255.0 / 60.0);
        }

        if ((c->bits_per_pixel == 32) || (c->bits_per_pixel == 24))
        {
            c->pal[loop] = (b << 16) + (g << 8) + r;
        }
        else if (c->bits_per_pixel == 16)
        {
            c->pal[loop] = ((b >> 3) << 11) + ((g >> 2) << 5) + (r >> 3);
        }

        colours[loop] = c->pal[loop];
    } 
  c->wave_height = (int *)calloc(c->radius, sizeof(int));
  for(i = 0; i < c->radius; i++) {
    float max = 
      ((float)c->colors) * 
      ((float)c->radius - (float)i) /
      ((float)c->radius);
    c->wave_height[i] = 
      (int)
      ((max + max*cos((double)i/50.0)) / 2.0);
  }
  c->source = (struct inter_source *)calloc(c->count, sizeof(struct inter_source));
  for(i = 0; i < c->count; i++) {
    c->source[i].x_theta = (float)(rand() % 1000) / 500.0 * 3.14159;
    c->source[i].y_theta = (float)(rand() % 1000) / 500.0 * 3.14159;
  }
}

#define source_x(c, i) \
  (c->w * 0.5 + ((int)(cos(c->source[i].x_theta)*((float)c->w * 0.5))))
#define source_y(c, i) \
  (c->h * 0.5 + ((int)(cos(c->source[i].y_theta)*((float)c->h * 0.5))))

/*
 * this is rather suboptimal. the sqrt() doesn't seem to be a big
 * performance hit, but all those separate XFillRectangle()'s are.
 * hell, it's just a quick hack anyway -- if someone wishes to tune
 * it, go ahead! 
 */

void do_inter(struct inter_context* c) 
{
    int i, j, k;
    int result;
    int dist;
    int g;
    int dx, dy;

    for(i = 0; i < c->count; i++) 
    {
        c->source[i].x_theta += (c->speed * 0.001);
        if(c->source[i].x_theta > 6.28318)
        {
            c->source[i].x_theta -= 6.28318;
        }
        c->source[i].y_theta += (c->speed * 0.001);
        if(c->source[i].y_theta > 6.28318)
        {
            c->source[i].y_theta -= 6.28318;
        }
        c->source[i].x = (int)(source_x(c, i));
        c->source[i].y = (int)(source_y(c, i));
    }

    g = c->grid_size;

    for(j = 0; j < c->h/g; j++) 
    {
        for(i = 0; i < c->w/g; i++) 
        {
            result = 0;
            for(k = 0; k < c->count; k++) 
            {
                dx = i*g + (g >> 1) - c->source[k].x;
                dy = j*g + (g >> 1) - c->source[k].y;
                dist = (int)sqrt(dx*dx + dy*dy); 
                result += (dist >= c->radius ? 0 : c->wave_height[dist]);
            }
            result %= c->colors;
            if ((c->bits_per_pixel == 32)||(c->bits_per_pixel == 24))
            {
                // Fill in these `gridsize' horizontal bits in the scanline 
                for(k = 0; k < g; k++)
                {
                    ((unsigned long *)(c->bitmap_data))[g*(i+(j*c->w))+k] = colours[result];
                }
            }
            else if (c->bits_per_pixel == 16)
            {
                // Fill in these `gridsize' horizontal bits in the scanline
                for(k = 0; k < g; k++)
                {
                    ((unsigned short *)(c->bitmap_data))[g*(i+(j*c->w))+k] = (unsigned short)colours[result];
                }
            }
        }
        // Only the first scanline of the image has been filled in; clone that
        //    scanline to the rest of the `gridsize' lines in the ximage 
        
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

void screenhack_init (HDC hdc, HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	sh = rect.bottom;
	sw = rect.right;

	colours = colSpectrum(ncolours);
	inter_init(hdc, hwnd, &c);
	/* Set the screenhack timer */
    SetTimer(hwnd, 10, delay, NULL);
}

void screenhack_loop(HDC hdc, HWND hWnd)
{
    RECT client;
    GetClientRect(hWnd, &client);
	//FillRect(hdc, &client, (HBRUSH)GetStockObject(BLACK_BRUSH));
	do_inter(&c);
	write_label(hdc,sw,sh);
	//colours = colSpectrum(ncolours);

	
	
}
