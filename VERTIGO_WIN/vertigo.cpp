
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <scrnsave.h>
#include <time.h>
#include "utils.h"
#include "ico_res.h"

long delay = 200;
long ncolours = 256;
#define GRID_SIZE 2
#define TIMER 1

int sh = 600;
int sw = 600;


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


/* *************** MAIN WINDOW ************************* */
LRESULT WINAPI MainWndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	static RECT rect;
	switch(nMsg)
	{
		case WM_CREATE:
			srand(time(NULL));
    		init(GetDC(hwnd), hwnd, &c);
			SetTimer(hwnd, TIMER, 10, NULL);
			return 0;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			KillTimer(hwnd,TIMER);
			return 0;
			break;
	
		case WM_TIMER:
			return 0;
			break;
		
		case WM_PAINT:
			if(counter < 500){
				draw(&c);
				if(!done)
					counter++;
				else
					counter--;
				if((counter % 50) == 0)
					init(GetDC(hwnd), hwnd, &c);
			}
			if(counter == 500){
				done = true;
				counter = 499;
			}
			if(counter == 0){
				counter = 1;
				done = false;
			}		
			return 0;
			break;
	}

        return DefWindowProc(hwnd,nMsg,wParam,lParam);
}

int STDCALL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
        HWND    hwndMain;
        MSG     msg;
        WNDCLASSEX      wndclass;
        char*   szMainWndClass = "WinTestWin";

        memset (&wndclass, 0, sizeof(WNDCLASSEX));
        wndclass.lpszClassName = szMainWndClass;
        wndclass.cbSize = sizeof(WNDCLASSEX);
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = MainWndProc;
        wndclass.hInstance = hInst;
        wndclass.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_RESFUND));
        wndclass.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_RESFUND));
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);

        RegisterClassEx(&wndclass);
        hwndMain = CreateWindow(
                szMainWndClass,
                "Vertigo - The Blue Ant",
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                sh,
                sw,
                NULL,
                NULL,
                hInst,
                NULL);

        ShowWindow(hwndMain, nShow);
        UpdateWindow(hwndMain);

        while(GetMessage(&msg, NULL, 0,0))
        {
                TranslateMessage (&msg);
                DispatchMessage(&msg);
        }
        return msg.wParam;
}

