
/*-
 * Copyright (c) 1995 by John Neil.
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
 * Revision History:
 * 01-Sep-2005: Ported on Windows XP by Fabio Nelli <fabio.nelli@virgilio.it>
 * 01-Nov-2000: Allocation checks
 * 10-May-1997: Jamie Zawinski <jwz@jwz.org> compatible with xscreensaver
 * 01-Sep-1995: color knotted components differently, J. Neil.
 * 29-Aug-1995: Written.  John Neil <neil@math.idbsu.edu>
 */


#include <windows.h>
#include <scrnsave.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "resource.h"
#include "options.h"
#include "utils.h"

long delay;
long ncolours;
long nstrands;
long pen1;
long pen2;

OPTION options[] =
{
/*	 Type            variable     control              default minimum maximum name */
    {LONG_EDIT_TYPE, &delay,      IDC_DELAY_EDIT,      200,    1,      10000,    "Delay"},
    {LONG_EDIT_TYPE, &ncolours,   IDC_COLOURS_EDIT,    256,     1,      256,    "Colours"},
	{LONG_EDIT_TYPE, &nstrands,   IDC_NSTRANDS_EDIT,    5,     3,      10,    "Strands"},
    {LONG_EDIT_TYPE, &pen1,		  IDC_PEN1_EDIT,    10,    1,      25,    "Pen1"},
	{LONG_EDIT_TYPE, &pen2,		  IDC_PEN2_EDIT,   10,     1,       25,    "Pen2"},
	{-1, NULL, -1, -1, -1, -1, ""}
};

OPTIONS_STRUCT screenhack_options = 
{
    "Braid", options
};

int sw; 
int sh; 
HPEN penna;

#define MAXLENGTH  30		/* the maximum length of a braid word */
#define MINLENGTH  25		/* the minimum length of a braid word */
#define MAXSTRANDS  15		/* the maximum number of strands in the braid */
#define SPINRATE  1.0		/* the rate at which the colors spin */

COLORREF* colore;

#define MI_NPIXELS 250  //variazione di colore
#define MI_NUM_SCREENS 1

typedef struct {
	int         linewidth;
	int         braidword[MAXLENGTH];
	int         components[MAXSTRANDS];
	int         startcomp[MAXLENGTH][MAXSTRANDS];
	int         nstrands;
	int         braidlength;
	float       startcolor;
	int         center_x;
	int         center_y;
	float       min_radius;
	float       max_radius;
	float       top, bottom, left, right;
	int         age;
	int         color_direction;
} braidtype;

//static braidtype *braids = (braidtype *) NULL;
braidtype braid;

static int applyword(braidtype * braid, int string, int position)
{
	int         i, c;
	c = string;
	for (i = position; i < braid->braidlength; i++) {
		if (c == ABS(braid->braidword[i]))
			c--;
		else if (c == ABS(braid->braidword[i]) - 1)
			c++;
	}
	for (i = 0; i < position; i++) {
		if (c == ABS(braid->braidword[i]))
			c--;
		else if (c == ABS(braid->braidword[i]) - 1)
			c++;
	}
	return c;
}

static int applywordbackto(braidtype * braid, int string, int position)
{
	int         i, c;
	c = string;
	for (i = position - 1; i >= 0; i--) {
		if (c == ABS(braid->braidword[i])) {
			c--;
		} else if (c == ABS(braid->braidword[i]) - 1) {
			c++;
		}
	}
	return c;
}

void init_braid(braidtype* braid)
{
	//braidtype  *braid;
	int         used[MAXSTRANDS];
	int         i, count, comp, c;
	float       min_length;

	if (braid == NULL) {
		if ((braid = (braidtype *) calloc(MI_NUM_SCREENS,
						sizeof (braidtype))) == NULL)
			return;
	}

	braid->center_x = sw / 2;
	braid->center_y = sh / 2;
	braid->age = 0;

	/* jwz: go in the other direction sometimes. */
	braid->color_direction = ((LRAND() & 1) ? 1 : -1);

	min_length = (braid->center_x > braid->center_y) ? braid->center_y : braid->center_x;
	braid->min_radius = min_length * 0.30;
	braid->max_radius = min_length * 0.90;

	braid->nstrands = INTRAND(nstrands,MAX(MIN(MIN(MAXSTRANDS, 4),
					       (int) ((braid->max_radius - braid->min_radius) / 5.0)), nstrands));
	
	braid->braidlength = INTRAND(MINLENGTH, MIN(MAXLENGTH, braid->nstrands * 6));

	for (i = 0; i < braid->braidlength; i++) {
		braid->braidword[i] =
			INTRAND(1, braid->nstrands - 1) * (INTRAND(1, 2) * 2 - 3);
		if (i > 0)
			while (braid->braidword[i] == -braid->braidword[i - 1])
				braid->braidword[i] = INTRAND(1, braid->nstrands - 1) * (INTRAND(1, 2) * 2 - 3);
	}

	while (braid->braidword[0] == -braid->braidword[braid->braidlength - 1])
		braid->braidword[braid->braidlength - 1] =
			INTRAND(1, braid->nstrands - 1) * (INTRAND(1, 2) * 2 - 3);

	do {
		(void) memset((char *) used, 0, sizeof (used));
		count = 0;
		for (i = 0; i < braid->braidlength; i++)
			used[ABS(braid->braidword[i])]++;
		for (i = 0; i < braid->nstrands; i++)
			count += (used[i] > 0) ? 1 : 0;
		if (count < braid->nstrands - 1) {
			braid->braidword[braid->braidlength] =
				INTRAND(1, braid->nstrands - 1) * (INTRAND(1, 2) * 2 - 3);
			while (braid->braidword[braid->braidlength] ==
			       -braid->braidword[braid->braidlength - 1] &&
			       braid->braidword[0] == -braid->braidword[braid->braidlength])
				braid->braidword[braid->braidlength] =
					INTRAND(1, braid->nstrands - 1) * (INTRAND(1, 2) * 2 - 3);
			braid->braidlength++;
		}
	} while (count < braid->nstrands - 1 && braid->braidlength < MAXLENGTH);

	braid->startcolor = (MI_NPIXELS > 2) ? (float) NRAND(MI_NPIXELS) : 0.0;

	(void) memset((char *) braid->components, 0, sizeof (braid->components));
	c = 1;
	comp = 0;
	braid->components[0] = 1;
	do {
		i = comp;
		do {
			i = applyword(braid, i, 0);
			braid->components[i] = braid->components[comp];
		} while (i != comp);
		count = 0;
		for (i = 0; i < braid->nstrands; i++)
			if (braid->components[i] == 0)
				count++;
		if (count > 0) {
			for (comp = 0; braid->components[comp] != 0; comp++);
			braid->components[comp] = ++c;
		}
	} while (count > 0);

	braid->linewidth = 1;

	if (braid->linewidth < 0)
		braid->linewidth = NRAND(-braid->linewidth) + 1;
      if (braid->linewidth * braid->linewidth * 8 > MIN(sw, sh))
		 braid->linewidth = MIN(1, (int) sqrt((double) MIN(sw, sh) / 8));
	for (i = 0; i < braid->nstrands; i++)
		if (!(braid->components[i] & 1))
			braid->components[i] *= -1;
}

void draw(HDC hdc,braidtype* braid)
{
	int         num_points = 500;
	float       t_inc;
	float       theta, psi;
	float       t, r_diff;
	int         i, s;
	float       x_1, y_1, x_2, y_2, r1, r2;
	float       color, color_use = 0.0, color_inc;
	int 		indc;

	theta = (2.0 * M_PI) / (float) (braid->braidlength);
	t_inc = (2.0 * M_PI) / (float) num_points;


	color_inc = (float) MI_NPIXELS * braid->color_direction / (float) num_points;
	braid->startcolor += SPINRATE * color_inc;
	if (((int) braid->startcolor) >= MI_NPIXELS)
		braid->startcolor = 0.0;
	
	r_diff = (braid->max_radius - braid->min_radius) / (float) (braid->nstrands);
	color = braid->startcolor;
	psi = 0.0;
	for (i = 0; i < braid->braidlength; i++) {
		psi += theta;
		for (t = 0.0; t < theta; t += t_inc) {
			color += color_inc;
			if (((int) color) >= MI_NPIXELS)
				color = 0.0;
			color_use = color;

			for (s = 0; s < braid->nstrands; s++) {
				if (ABS(braid->braidword[i]) == s)
					continue;
				if (ABS(braid->braidword[i]) - 1 == s) {

					if (MI_NPIXELS > 2) {
						color_use += SPINRATE * color_inc;
						while (((int) color_use) >= MI_NPIXELS)
							color_use -= (float) MI_NPIXELS;
					}
				
					r1 = braid->min_radius + r_diff * (float) (s);
					r2 = braid->min_radius + r_diff * (float) (s + 1);
					if (braid->braidword[i] > 0 ||
					    (FABSF(t - theta / 2.0) > theta / 7.0)) {
						x_1 = ((0.5 * (1.0 + SINF(t / theta * M_PI - M_PI_2)) * r2 +
							0.5 * (1.0 + SINF((theta - t) / theta * M_PI - M_PI_2)) * r1)) *
							COSF(t + psi) + braid->center_x;
						y_1 = ((0.5 * (1.0 + SINF(t / theta * M_PI - M_PI_2)) * r2 +
							0.5 * (1.0 + SINF((theta - t) / theta * M_PI - M_PI_2)) * r1)) *
							SINF(t + psi) + braid->center_y;
						x_2 = ((0.5 * (1.0 + SINF((t + t_inc) / theta * M_PI - M_PI_2)) * r2 +
							0.5 * (1.0 + SINF((theta - t - t_inc) / theta * M_PI - M_PI_2)) * r1)) *
							COSF(t + t_inc + psi) + braid->center_x;
						y_2 = ((0.5 * (1.0 + SINF((t + t_inc) / theta * M_PI - M_PI_2)) * r2 +
							0.5 * (1.0 + SINF((theta - t - t_inc) / theta * M_PI - M_PI_2)) * r1)) *
							SINF(t + t_inc + psi) + braid->center_y;

						indc = (int)ABS(color_use);
						penna = CreatePen(PS_SOLID,pen2,colore[indc]);
						SelectObject(hdc,penna);
						MoveToEx(hdc,(int)x_1,(int)y_1,NULL);
						LineTo(hdc,(int)x_2,(int)y_2);
						DeleteObject(penna);
					}

					if (braid->braidword[i] < 0 ||
					    (FABSF(t - theta / 2.0) > theta / 7.0)) {
						x_1 = ((0.5 * (1.0 + SINF(t / theta * M_PI - M_PI_2)) * r1 +
							0.5 * (1.0 + SINF((theta - t) / theta * M_PI - M_PI_2)) * r2)) *
							COSF(t + psi) + braid->center_x;
						y_1 = ((0.5 * (1.0 + SINF(t / theta * M_PI - M_PI_2)) * r1 +
							0.5 * (1.0 + SINF((theta - t) / theta * M_PI - M_PI_2)) * r2)) *
							SINF(t + psi) + braid->center_y;
						x_2 = ((0.5 * (1.0 + SINF((t + t_inc) / theta * M_PI - M_PI_2)) * r1 +
							0.5 * (1.0 + SINF((theta - t - t_inc) / theta * M_PI - M_PI_2)) * r2)) *
							COSF(t + t_inc + psi) + braid->center_x;
						y_2 = ((0.5 * (1.0 + SINF((t + t_inc) / theta * M_PI - M_PI_2)) * r1 +
							0.5 * (1.0 + SINF((theta - t - t_inc) / theta * M_PI - M_PI_2)) * r2)) *
							SINF(t + t_inc + psi) + braid->center_y;

						indc = (int)ABS(color_use);
						penna = CreatePen(PS_SOLID,pen2,colore[indc]);
						SelectObject(hdc,penna);
						MoveToEx(hdc,(int)x_1,(int)y_1,NULL);
						LineTo(hdc,(int)x_2,(int)y_2);
						DeleteObject(penna);
					}
				} else {

					if (MI_NPIXELS > 2) {
						color_use += SPINRATE * color_inc;
						while (((int) color_use) >= MI_NPIXELS)
							color_use -= (float) MI_NPIXELS;
					}

					r1 = braid->min_radius + r_diff * (float) (s);
					x_1 = r1 * COSF(t + psi) + braid->center_x;
					y_1 = r1 * SINF(t + psi) + braid->center_y;
					x_2 = r1 * COSF(t + t_inc + psi) + braid->center_x;
					y_2 = r1 * SINF(t + t_inc + psi) + braid->center_y;

					indc = (int)ABS(color_use);
					penna = CreatePen(PS_SOLID,pen1,colore[indc]);
					SelectObject(hdc,penna);
					MoveToEx(hdc,(int)x_1,(int)y_1,NULL);
					LineTo(hdc,(int)x_2,(int)y_2);
					DeleteObject(penna);
				}
			}
		}
	}

}

void screenhack_init (HDC hdc, HWND hwnd)
{
	RECT client;
	GetClientRect(hwnd, &client);
	sh = client.bottom;
	sw = client.right;
	
	colore = colSpectrum(ncolours);
	init_braid(&braid);
	
	/* Set the screenhack timer */
    SetTimer(hwnd, 10, delay, NULL);
}

void screenhack_loop(HDC hdc, HWND hWnd)
{
    RECT client;
    GetClientRect(hWnd, &client);
	FillRect(hdc, &client, (HBRUSH)GetStockObject(BLACK_BRUSH));
	write_label(GetDC(hWnd),sw,sh);
	draw(hdc,&braid);
	
	Sleep(2000);
	colore = colSpectrumCoherent(ncolours);
	init_braid(&braid);
	
}
