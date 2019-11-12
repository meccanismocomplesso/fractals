#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "ico_res.h"

float a[12];
//Dimensioni della WINDOW
int sw = 500;
int sh = 500;
int tsw =(int) (sw*1.1); //fattore correttivo
int tsh =(int) (sh*1.1); //fattore correttivo
int nmax = 100000; //numero massimo di iterazioni 

#define NC 256 //numero dei colori
COLORREF colours[NC];

float x,y,z;
float xe,ye,ze;
float xnew,ynew,znew;
float xp,yp;

float xz,yz;
float xl,xh,yl,yh,zl,zh; 

int n,nl;
float lsum;
int ns; //numero dei settori
float of; //fattore di sovrapposizione (overlap factor)
int counter = 0;

/* Generatore di numeri casuali da 0 a 1 */
float random()
{
	return (rand()%100)*0.01;
}

void getSpectrum(COLORREF* col)
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

void init_scale()
{
	xl = 1000000;
	xh = -xl;
	yl = xl;
	yh = xh;
	zl = xl;
	zh = xh;	
}

void init()
{
	x = 0.05;
	y = 0.05;
	z = 0.05;
	xe = x + 0.000001;
	ye = y;
	ze = z;
	lsum = 0;
	n = 0;
	nl = 0;

	of = 0.5 + 1.5 * random();
	ns = (int) (2 + 8*random());

	for(int i=0; i < 12; i++)
     	{
        	a[i] = ((int)(25 * random()) -12)*0.1;
     	}
}     

void iterate()
{
	xnew = a[0] + x*(a[1]+a[2]*x+a[3]*y)+y*(a[4]+a[5]*y);
	ynew = a[6] + x*(a[7]+a[8]*x+a[9]*y)+y*(a[10]+a[11]*y);
	znew = x * x + y * y;
}

void rescaling()
{
	if ( x < xl) xl = x;
	if ( x > xh) xh = x;
	if ( y < yl) yl = y;
	if ( y > yh) yh = y;
	if ( z < zl) zl = z;
	if ( z > zh) zh = z;
}

void predraw()
{
	xz = 0.05 * sw / (zh - zl);
	yz = 0.05 * sh / (zh - zl);
}

void draw(HDC hdc)
{
	// E' necessario calcolare il numero del pixel in base al
	// valore massimo trovato e poi convertito in un intero
	if ( xh > xl )
	{
		xp = sw * (x -xl) / (xh - xl);
	}
	if (yh > yl )
	{ 
		yp = sh * (yh -y) / (yh -yl);
	}
	int s = (int) (ns * random()); //scelta random del settore
	float th = 2 * M_PI *(of * yp / sh + s)/ns;
	if ((ns % 2)== 0 && (s % 2)== 0)
		th = 2 * M_PI / ns - th;
	yp =  0.5 * sh * ( 1 + xp * cos(th) / sw);
	xp =  0.5 * sh * ( 1 + xp * sin(th) / sw);
	int draw_x = (int) xp;
	int draw_y = (int) yp; 
	
	// il colore corrisponde alla profondità z
	int c = 2 + ((int)(NC*(z - zl)/(zh-zl) + NC)) % NC;	//colore
	SetPixel(hdc, draw_x, draw_y, colours[c]);
}

int test()
{
	float l = 0; //esponente di Lyapunov
        if( (abs((int)xnew) + abs((int)ynew) + abs((int)znew)) > 1000000 ) return 1;
	float xsave = xnew;
	float ysave = ynew;
	float zsave = znew;
	x = xe;
	y = ye;
	z = ze;
	iterate();
	float dlx = xnew - xsave;
	float dly = ynew - ysave;
	float dlz = znew - zsave;
	double dl2 = dlx * dlx + dly * dly + dlz * dlz;
	if ( dl2 != 0)
	{
		double df = 100000000 * dl2 * 1000;
		double rs = 1 / sqrt(df);

		xe = xsave + rs * dlx;
		xnew = xsave;
		ye = ysave + rs * dly;
		ynew = ysave;
		ze = zsave + rs * dlz;
		znew = zsave;
		lsum = lsum + log(df);
		nl++;
		l = 0.721347 * lsum / nl;
	}
	if( n > 100 && l < 0.005 ) return 1;
	if( n > nmax ) return 1;
	if(xnew > 1000000 || ynew > 1000000 || znew > 1000000 ) return 1;
	if(xnew < -1000000 || ynew < -1000000 || znew < -1000000 ) return 1;

	x = xnew;
	y = ynew;
	z = znew;

	return 0;
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
			init_scale();
			init();
			getSpectrum(colours);
			return 0;
                        break;
               
                case WM_DESTROY:
                        PostQuitMessage (0);
                        return 0;
                        break;

                case WM_PAINT:
			iterate();
			if ( n > 6000 && n < 7000)
			    rescaling();
			if (n == 7000)
			{
			    predraw();
			    counter++;
			    if(counter > 4)
			    {   //ogni 5 disegni riparti da capo
				Sleep(2000);				
				HBRUSH black_brush = CreateSolidBrush(RGB(0,0,0));
				HDC hdc = GetDC(hwnd);
				SelectObject(hdc,black_brush);
				Rectangle(hdc,0,0,tsw,tsh);
  				DeleteObject(black_brush);
				ReleaseDC(hwnd,hdc);
				counter = 0;
				init_scale();
				getSpectrum(colours);
			    }
			}
			if( n > 7000)
			{
			    HDC hdc2 = GetDC(hwnd);
			    draw(hdc2);
			    ReleaseDC(hwnd,hdc2);
			}
			n++;
			if( n == 50000 || test()==1 ){
				init();
			}		
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
                "Strange Flowers - The Blue Ant",/* Caption */
                WS_OVERLAPPEDWINDOW,    /* Style */
                CW_USEDEFAULT,          /* Initial x (use default) */
                CW_USEDEFAULT,          /* Initial y (use default) */
                tsw,              	/* Initial x size */
                tsh,              	/* Initial y size */
                NULL,                   /* No parent window */
                NULL,                   /* No menu */
                hInst,                  /* This program instance */
                NULL                    /* Creation parameters */
                );
        
        /*
         * Display the window which we just created (using the nShow
         * passed by the OS, which allows for start minimized and that
         * sort of thing).
         */
        ShowWindow (hwndMain, nShow);
        UpdateWindow (hwndMain);

        while (GetMessage (&msg, NULL, 0, 0))
        {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
        }
        return msg.wParam;
}
