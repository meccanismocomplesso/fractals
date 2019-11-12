#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "ico_res.h"

//Dimensioni della WINDOW
int sw = 450;
int sh = 450;

int c;
int i;

#define SCALE 100
//PARAMETERS
#define FX 12.0
#define FX2 1.25


int dots = 100000;
HDC hdc;
bool done = false;

#define M 3
double a[M];
double b[M];


int rand100()
{
	return rand()%100;
}

void init()
{
	for(i=0; i<M; i++)
	{
		a[i] = cos(2*M_PI*i/(double)M);
		b[i] = sin(2*M_PI*i/(double)M);
	}
}

void draw(HDC hdc)
{
    double r = sqrt(FX2*M);
	double x1,y1,x,y,ra;
	int drawx,drawy;

    for (i = 1; i <= dots; i++) {

		c = rand100() % M;
		ra = sqrt(FX*(x*x+y*y));
		x1 = -x/r + y/(ra*r) + a[c];
		y1 = -x/(ra+r)- y/r + b[c];
		x = x1;
		y = y1;

		drawx = (int)(x* SCALE + sw/2);
		drawy = (int)(y* SCALE + sh/2);

		SetPixel(hdc, drawx, drawy,RGB(255,255,255));
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
						init();
						return 0;
                        break;
               
                case WM_DESTROY:
                        PostQuitMessage (0);
                        return 0;
                        break;
        
				case WM_PAINT:
					if(!done){
						hdc = GetDC(hwnd);	
  						draw(hdc);
						done = true;
					}
						//ReleaseDC(hwnd,hdc);
						return 0;
						break;
        }
        return DefWindowProc (hwnd, nMsg, wParam, lParam);
}

int STDCALL
WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
        HWND         hwndMain;        
        MSG          msg;             
        WNDCLASSEX   wndclass;        
        char*        szMainWndClass = "WinTestWin";                                

        memset (&wndclass, 0, sizeof(WNDCLASSEX));
        wndclass.lpszClassName = szMainWndClass;
        wndclass.cbSize = sizeof(WNDCLASSEX);
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = MainWndProc;
        wndclass.hInstance = hInst;
        wndclass.hIcon = LoadIcon (hInst, MAKEINTRESOURCE(IDI_RESFUND));
        wndclass.hIconSm = LoadIcon (hInst, MAKEINTRESOURCE(IDI_RESFUND));
        wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
        RegisterClassEx (&wndclass);
    
        hwndMain = CreateWindow (
                szMainWndClass,         /* Class name */
                "Quadrup Two Orbit - The Blue Ant",   /* Caption */
                WS_OVERLAPPEDWINDOW,    /* Style */
                CW_USEDEFAULT,          /* Initial x (use default) */
                CW_USEDEFAULT,          /* Initial y (use default) */
                sw,              	/* Initial x size */
                sh+50,              	/* Initial y size */
                NULL,                   /* No parent window */
                NULL,                   /* No menu */
                hInst,                  /* This program instance */
                NULL                    /* Creation parameters */
                );
        
        ShowWindow (hwndMain, nShow);
        UpdateWindow (hwndMain);

        while (GetMessage (&msg, NULL, 0, 0))
        {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
        }
        return msg.wParam;
}
