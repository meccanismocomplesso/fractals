#include <stdio.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include "ico_res.h"

#define NSTEPS 7
#define COUNT (1 << NSTEPS)

//OPTIONS 

#define NC			256
#define ITERATIONS	7
#define MODE		0
// 0   h = hue
// 1   s = saturation
// 2   v = value	
// 3   r = random

//Dimensioni della WINDOW
#define SW 600
#define SH 600
COLORREF colore[NC];
#define CELL(c, r) cell[((unsigned int)(c)) + ((unsigned int) (r)) * SW]

static int extra_krinkly_p;
static signed char *cell = NULL;


float random()
{
	return (rand()%100)*0.01;
}

int random100()
{
	return int(random()*1000);
}

void clrScreen(HWND hwnd)
{
	HBRUSH black_brush = CreateSolidBrush(RGB(0,0,0));
	HDC hdc = GetDC(hwnd);
	SelectObject(hdc,black_brush);
	Rectangle(hdc,0,0,SW,SH);
  	DeleteObject(black_brush);
	ReleaseDC(hwnd,hdc);
}

void colSpectrum(COLORREF* col)
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


void init_map ()
{
  unsigned long fg_pixel = 0, bg_pixel = 0;
  int fg_h, bg_h;
  double fg_s, fg_v, bg_s, bg_v;
  int this_mode = MODE;	

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

      switch (this_mode)
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
	    ? NC - ((-(height)) % NC)		\
	    : 0)					\
	 : ((height) >= NC				\
	    ? (extra_krinkly_p				\
	       ? (height) % NC			\
	       : NC-1)				\
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


/* ********************************************************* */
/* ********************************************************* */
LRESULT CALLBACK
MainWndProc (HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
        switch (nMsg)
        {
                case WM_CREATE:
				srand( (unsigned)time( NULL ) );
				colSpectrum(colore);
				init_map();
				return 0;
                        break;
               
                case WM_DESTROY:
                        PostQuitMessage (0);
                        return 0; 
                        break;

                case WM_PAINT:
				clrScreen(hwnd);
				HDC hdc = GetDC(hwnd);
				draw_map(hdc);
				ReleaseDC(hwnd,hdc);
			    Sleep(2000);
				colSpectrum(colore);
				init_map();
				return 0;	
				break;
        }
        return DefWindowProc (hwnd, nMsg, wParam, lParam);
}

int STDCALL WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
        HWND         hwndMain;        /* Handle for the main window. */
        MSG          msg;             /* A Win32 message structure. */
        WNDCLASSEX   wndclass;        /* A window class structure. */
        char*        szMainWndClass = "TheAntWin";                                

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
                "IMSMAP - The Blue Ant",/* Caption */
                WS_OVERLAPPEDWINDOW,    /* Style */
                CW_USEDEFAULT,          /* Initial x (use default) */
                CW_USEDEFAULT,          /* Initial y (use default) */
                SW,              	/* Initial x size */
                SH,              	/* Initial y size */
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


