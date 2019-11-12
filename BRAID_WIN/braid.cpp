#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ico_res.h"

#define MAXLENGTH  20		/* the maximum length of a braid word */
#define MINLENGTH  15		/* the minimum length of a braid word */
#define MAXSTRANDS  15		/* the maximum number of strands in the braid */
#define MINSTRANDS  5		/* the minimum number of strands in the braid */
#define SPINRATE  2.0		/* the rate at which the colors spin */

//Dimensioni della WINDOW
int sw = 600;
int sh = 600;
HPEN penna;
#define NC 256
COLORREF colore[NC];

#define ABS(a) ((a)<0 ? -(a):(a))
#define COSF(n) ((float)cos((double)(n)))
#define SINF(n) ((float)sin((double)(n)))
#define FABSF(n) ((float)fabs((double)(n)))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define LRAND() ((long)(rand() & 0x7fffffff))
#define NRAND(n) ((int)(LRAND()%(n)))

#define INTRAND(min,max) (NRAND((max+1)-(min))+(min))
#define FLOATRAND(min,max) ((min)+((double) LRAND()/((double) MAXRAND))*((max)-(min)))

#define MI_NPIXELS 250  //variazione di colore
#define MI_NUM_SCREENS 1

/* Generatore di numeri casuali da 0 a 1 */
float random()
{
	return (rand()%100)*0.01;
}

void clrScreen(HWND hwnd)
{
	HBRUSH black_brush = CreateSolidBrush(RGB(0,0,0));
	HDC hdc = GetDC(hwnd);
	SelectObject(hdc,black_brush);
	Rectangle(hdc,0,0,sw,sh);
  	DeleteObject(black_brush);
	ReleaseDC(hwnd,hdc);
}

void colSpectrum2(COLORREF* col)
{	
	int bb,rr,gg;
	float cb,cg,cr;
	float a1,a2,a3;

	a1 = random();
	a2 = random();
	a3 = random();

	cb = 1 + 3 * random();
	cg = 1 + 3 * random();	
	cr = 1 + 3 * random();
	for(int i = 0; i < NC; i++)
	{
		float f = i* 1.0/NC;
		
		bb = (int) (126 + 126 * sin(cb * 2 * M_PI * (f + a1)));	
		gg = (int) (126 + 126 * sin(cg * 2 * M_PI * (f + a2)));	
		rr = (int) (126 + 126 * sin(cr * 2 * M_PI * (f + a3)));
		col[i] = RGB(rr,gg,bb);
	}
}

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

	//if (MI_COUNT(mi) < MINSTRANDS)
	//	braid->nstrands = MINSTRANDS;
	//else
		braid->nstrands = INTRAND(MINSTRANDS,
				       MAX(MIN(MIN(MAXSTRANDS, 4),
					       (int) ((braid->max_radius - braid->min_radius) / 5.0)), MINSTRANDS));
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
						penna = CreatePen(PS_SOLID,5,colore[indc]);
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
						penna = CreatePen(PS_SOLID,5,colore[indc]);
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
					penna = CreatePen(PS_SOLID,5,colore[indc]);
					SelectObject(hdc,penna);
					MoveToEx(hdc,(int)x_1,(int)y_1,NULL);
					LineTo(hdc,(int)x_2,(int)y_2);
					DeleteObject(penna);
				}
			}
		}
	}

}

/* ********************************************************* */
/* ********************************************************* */
LRESULT CALLBACK
MainWndProc (HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
        switch (nMsg)
        {
                case WM_CREATE:
				srand( (unsigned)time( NULL ) );
				colSpectrum2(colore);
				init_braid(&braid);
				return 0;
                        break;
               
                case WM_DESTROY:
                        PostQuitMessage (0);
                        return 0; 
                        break;

                case WM_PAINT:
				clrScreen(hwnd);
				HDC hdc = GetDC(hwnd);
				draw(hdc,&braid);
				ReleaseDC(hwnd,hdc);
			      Sleep(2000);
				colSpectrum2(colore);
				init_braid(&braid);
				return 0;	
				break;
        }
        return DefWindowProc (hwnd, nMsg, wParam, lParam);
}

int STDCALL
WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
        HWND         hwndMain;        /* Handle for the main window. */
        MSG          msg;             /* A Win32 message structure. */
        WNDCLASSEX   wndclass;        /* A window class structure. */
        char*        szMainWndClass = "WinTestWin";                                

        /* Initialize the entire structure to zero. */
        memset (&wndclass, 0, sizeof(WNDCLASSEX));

        /* This class is called WinTestWin */
        wndclass.lpszClassName = szMainWndClass;

        /* the size of the structure for extensibility. */
        wndclass.cbSize = sizeof(WNDCLASSEX);

        /* All windows of this class redraw when resized. */
        wndclass.style = CS_HREDRAW | CS_VREDRAW;

        /* All windows of this class use the MainWndProc */
        wndclass.lpfnWndProc = MainWndProc;

        /* This class is used with the current program instance. */
        wndclass.hInstance = hInst;

        /* Use standard icon and arrow cursor provided by the OS */
        wndclass.hIcon = LoadIcon (hInst, MAKEINTRESOURCE(IDI_RESFUND));
        wndclass.hIconSm = LoadIcon (hInst, MAKEINTRESOURCE(IDI_RESFUND));
        wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);

        /* Color the background white */
        wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);

        /* Now register the window class for use. */
        RegisterClassEx (&wndclass);

        /* Create our main window using that window class */
        hwndMain = CreateWindow (
                szMainWndClass,         /* Class name */
                "Braids - The Blue Ant",/* Caption */
                WS_OVERLAPPEDWINDOW,    /* Style */
                CW_USEDEFAULT,          /* Initial x (use default) */
                CW_USEDEFAULT,          /* Initial y (use default) */
                sw,              	/* Initial x size */
                sh,              	/* Initial y size */
                NULL,                   /* No parent window */
                NULL,                   /* No menu */
                hInst,                  /* This program instance */
                NULL                    /* Creation parameters */
                );
        
        /* Display the window which we just created */
        ShowWindow (hwndMain, nShow);
        UpdateWindow (hwndMain);

        while (GetMessage (&msg, NULL, 0, 0))
        {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
        }
        return msg.wParam;
}


