
/*-
 * Copyright (c) 1991 by Patrick J. Naughton.
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
 * 10-Mar-2006: Ported on Windows XP by Fabio Nelli <fabio.nelli@virgilio.it>
 * 01-Nov-2000: Allocation checks
 * 24-Jun-1997: EJK and RR functions stolen from xmartin2.2
 *              Ed Kubaitis <ejk@ux2.cso.uiuc.edu> ejk functions and xmartin
 *              Renaldo Recuerdo rr function, generalized exponent version
 *              of the Barry Martin's square root function
 * 10-May-1997: Compatible with xscreensaver
 * 27-Jul-1995: added Peter de Jong's hop from Scientific American
 *              July 87 p. 111.  Sometimes they are amazing but there are a
 *              few duds (I did not see a pattern in the parameters).
 * 29-Mar-1995: changed name from hopalong to hop
 * 09-Dec-1994: added Barry Martin's sine hop
 * Changes in original xlock
 * 29-Oct-1990: fix bad (int) cast.
 * 29-Jul-1990: support for multiple screens.
 * 08-Jul-1990: new timing and colors and new algorithm for fractals.
 * 15-Dec-1989: Fix for proper skipping of {White,Black}Pixel() in colors.
 * 08-Oct-1989: Fixed long standing typo bug in RandomInitHop();
 *	            Fixed bug in memory allocation in init_hop();
 *	            Moved seconds() to an extern.
 *	            Got rid of the % mod since .mod is slow on a sparc.
 * 20-Sep-1989: Lint.
 * 31-Aug-1988: Forked from xlock.c for modularity.
 * 23-Mar-1988: Coded HOPALONG routines from Scientific American Sept. 86 p. 14.
 *              Hopalong was attributed to Barry Martin of Aston University
 *              (Birmingham, England)
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
long count;
long mode;

OPTION options[] =
{
/*	 Type            variable     control              default minimum maximum name */
    {LONG_EDIT_TYPE, &delay,      IDC_DELAY_EDIT,      200,    1,		10000,    "Delay"},
    {LONG_EDIT_TYPE, &ncolours,   IDC_COLOURS_EDIT,    256,     1,      256,    "Colours"},
    {LONG_EDIT_TYPE, &count,	  IDC_COUNT_EDIT,      5000,    100,    50000,     "Count"},
    {LONG_EDIT_TYPE, &mode,		  IDC_MODE_EDIT,       0,       0,		11,		  "Mode"},
	{-1, NULL, -1, -1, -1, -1, ""}
};

OPTIONS_STRUCT screenhack_options = 
{
    "Hopalong", options
};

int SW,SH;
COLORREF* colore;
//int counter;
int col;
// Parameter MODE
#define MARTIN 0
#define POPCORN 7
#define SINE 8
#define EJK1 1
#define EJK2 2
#define EJK3 9
#define EJK4 3
#define EJK5 4
#define EJK6 10
#define RR 5
#define JONG 6
#define OPS 11

typedef struct {
	int         centerx, centery;	/* center of the screen */
	double      a, b, c, d;
	double      i, j;				/* hopalong parameters */
	int         inc;
	int         op;
	int         count;
	int			x;
	int			y;
} hopstruct;

//static hopstruct *hops = (hopstruct *) NULL;
hopstruct hop;

void init_hop(hopstruct* hp)
{
	double      range;

	if (hp == NULL) {
		if ((hp = (hopstruct *) calloc(1,
						 sizeof (hopstruct))) == NULL)
			return;
	}

	hp->centerx = SW / 2;
	hp->centery = SH / 2;
	/* Make the other operations less common since they are less interesting */
	hp->op = (int) mode;

	range = sqrt((double) hp->centerx * hp->centerx +
	     (double) hp->centery * hp->centery) / (1.0 + LRAND() / MAXRAND);
	hp->i = hp->j = 0.0;
	hp->inc = (int) ((LRAND() / MAXRAND) * 200) - 100;

	switch (hp->op) {
		case MARTIN:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0; 
			hp->b = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0; 
			if (LRAND() & 1)
				hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0; 
			else
				hp->c = 0.0;
			break;
		case EJK1:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0; 
			hp->b = (LRAND() / MAXRAND) * 0.4;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 40.0;
			break;
		case EJK2:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0;
			hp->b = pow(10.0, 6.0 + (LRAND() / MAXRAND) * 24.0);
			if (LRAND() & 1)
				hp->b = -hp->b;
			hp->c = pow(10.0, (LRAND() / MAXRAND) * 9.0);
			if (LRAND() & 1)
				hp->c = -hp->c;
			break;
		case EJK3:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0;
			hp->b = (LRAND() / MAXRAND) * 0.35 + 0.5;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 70.0;
			break;
		case EJK4:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 2.0;
			hp->b = (LRAND() / MAXRAND) * 9.0 + 1.0;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 200.0;
			break;
		case EJK5:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 2.0;
			hp->b = (LRAND() / MAXRAND) * 0.3 + 0.1;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 200.0;
			break;
		case EJK6:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0;
			hp->b = (LRAND() / MAXRAND) + 0.5;
			break;
		case RR:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 40.0;
			hp->b = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 200.0;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0;
			hp->d = (LRAND() / MAXRAND) * 0.9;
			break;
		case POPCORN:
			hp->a = 0.0;
			hp->b = 0.0;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * 0.24 + 0.25;
			hp->inc = 100;
			break;
		case JONG:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			hp->b = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			hp->d = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			break;
		case SINE:	/* MARTIN2 */
			hp->a = M_PI + ((LRAND() / MAXRAND) * 2.0 - 1.0) * 0.7;
			break;
	}
	hp->count = 0;
}


void draw_hop(HDC hdc,hopstruct* hp)
{
	double      oldj, oldi;
	int			x;
	int			y;

	if (hp == NULL)
		return;
	hp->inc++;


	for(int i = 0; i < count; i++)
	{
		oldj = hp->j;
		switch (hp->op) {
			case MARTIN:	/* SQRT, MARTIN1 */
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj + ((hp->i < 0)
						? sqrt(fabs(hp->b * oldi - hp->c))
					: -sqrt(fabs(hp->b * oldi - hp->c)));
				x = hp->centerx + (int) (hp->i + hp->j)/8;
				y = hp->centery - (int) (hp->i - hp->j)/8;
				break;
			case EJK1:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? (hp->b * oldi - hp->c) :
							- (hp->b * oldi - hp->c));
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK2:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i < 0) ? log(fabs(hp->b * oldi - hp->c)) :
						-log(fabs(hp->b * oldi - hp->c)));
				x = hp->centerx + (int) (hp->i + hp->j)/2;
				y = hp->centery - (int) (hp->i - hp->j)/2;
				break;
			case EJK3:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? sin(hp->b * oldi) - hp->c :
						-sin(hp->b * oldi) - hp->c);
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK4:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? sin(hp->b * oldi) - hp->c :
						-sqrt(fabs(hp->b * oldi - hp->c)));
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK5:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? sin(hp->b * oldi) - hp->c :
						-(hp->b * oldi - hp->c));
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK6:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - asin((hp->b * oldi) - (long) (hp->b * oldi));
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case RR:	/* RR1 */
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i < 0) ? -pow(fabs(hp->b * oldi - hp->c), hp->d) :
						pow(fabs(hp->b * oldi - hp->c), hp->d));
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case POPCORN:
	#define HVAL 0.05
	#define INCVAL 50
				{
					double      tempi, tempj;

					if (hp->inc >= 100)
						hp->inc = 0;
					if (hp->inc == 0) {
						if (hp->a++ >= INCVAL) {
							hp->a = 0;
							if (hp->b++ >= INCVAL)
								hp->b = 0;
						}
						hp->i = (-hp->c * INCVAL / 2 + hp->c * hp->a) * M_PI / 180.0;
						hp->j = (-hp->c * INCVAL / 2 + hp->c * hp->b) * M_PI / 180.0;
					}
					tempi = hp->i - HVAL * sin(hp->j + tan(3.0 * hp->j));
					tempj = hp->j - HVAL * sin(hp->i + tan(3.0 * hp->i));
					x = hp->centerx + (int) (SW / 40 * tempi);
					y = hp->centery + (int) (SH / 40 * tempj);
					hp->i = tempi;
					hp->j = tempj;
				}
				break;
			case JONG:
				if (hp->centerx > 0)
					oldi = hp->i + 4 * hp->inc / hp->centerx;
				else
					oldi = hp->i;
				hp->j = sin(hp->c * hp->i) - cos(hp->d * hp->j);
				hp->i = sin(hp->a * oldj) - cos(hp->b * oldi);
				x = hp->centerx + (int) (hp->centerx * (hp->i + hp->j) / 4.0);
				y = hp->centery - (int) (hp->centery * (hp->i - hp->j) / 4.0);
				break;
			case SINE:	/* MARTIN2 */
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - sin(oldi);
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
		}
		SetPixel(hdc, x, y, colore[col]);
	}
}

void release_hop(hopstruct* hp)
{
	if (hp != NULL) {
		int         screen;	
		(void) free((void *) hp);
		hp = (hopstruct *) NULL;
	}
}
void screenhack_init (HDC hdc, HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	//FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

	SH = rect.bottom;
	SW = rect.right;
	colore = colSpectrum(ncolours);
    init_hop(&hop);

    /* Set the screenhack timer */
    SetTimer(hwnd, 10, delay, NULL);
}

void screenhack_loop(HDC hdc, HWND hwnd)
{
    RECT client;
    GetClientRect(hwnd, &client);
    FillRect(hdc, &client, (HBRUSH)GetStockObject(BLACK_BRUSH));
	write_label(GetDC(hwnd),SW,SH);

	colore = colSpectrum(ncolours);
    for(col = 0; col <ncolours; col++)
			draw_hop(hdc,&hop);

	init_hop(&hop);
	Sleep(2000);
}
