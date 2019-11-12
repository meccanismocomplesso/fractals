#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "ico_res.h"

//Dimensioni della WINDOW
int sw = 450;
int sh = 450;

float x,y,xnew,ynew;
int c;
int i,j,k;

#define LINES 4
//PARAMETERS
int dots = 100000;
HDC hdc;
bool done = false;

static double map[LINES][7] = {
	{	0.14,	0.01,	0.00,	0.51,	-0.08,	-1.31,	0.10},
	{	0.43,	0.52,	-0.45,	0.50,	1.49,	0.75,	0.25},
	{	0.45,	-0.49,	0.47,	0.47,	-1.62,	-0.74,	0.25},
	{	0.49,	0.00,	0.00,	0.51,	0.02,	1.62,	0.40}

};


float random()
{
	return (rand()%100)*0.01;
}

void draw(HDC hdc)
{
    long i;
  
    double u = 0.0, v = 0.0, newu, newv, sum = 0.0, rnd;
    int l = 0;
	c = 0;
	int centerx = (int)(sw *0.5);
	int centery = (int)(sh *0.5);

    for (i = 1; i <= dots; i++) {
      rnd = random();
      l = 0; sum = map[l][6];
      while ( (rnd > sum) && (l < LINES) ) {
        l++;
        sum += map[l][6];
      }
      if (l < LINES) {
        newu = map[l][0] * u + map[l][1] * v;
        newv = map[l][2] * u + map[l][3] * v;
        u = newu + map[l][4];
        v = newv + map[l][5];
      }
		int drawx = (int)(u*centerx*-0.2);
		int drawy = (int)(v*centery*-0.2);

		SetPixel(hdc, drawx+centerx, drawy+centery,RGB(255,255,255));
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
                "IFS Leaf - The Blue Ant",   /* Caption */
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
