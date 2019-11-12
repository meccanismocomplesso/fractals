#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "ico_res.h"

//Dimensioni della WINDOW
int sw = 450;
int sh = 450;

int i;
int c;

#define SCALE 100
#define SQRT3           1.7320508075688772935

int dots = 100000;
HDC hdc;
bool done = false;



void draw(HDC hdc)
{
    double x=0.5,y=0.75,r=2,r0,w;
	int drawx,drawy;


    for (i = 1; i <= dots; i++) {


		r0 = sqrt(x*x+y*y);
		switch ((int)(rand()%4)) {
      case 0:
         w = atan2(y,x-1);
         y = -r0 * cos(w) / r + 1;
         x = -r0 * sin(w) / r;
         break;
      case 1:
         w = atan2(y-SQRT3/2,x+0.5);
         y = -r0 * cos(w) / r - 0.5;
         x = -r0 * sin(w) / r + SQRT3/2;
		 break;
      case 2:
         w = atan2(y+SQRT3/2,x+0.5);
         y = -r0 * cos(w) / r - 0.5;
         x = -r0 * sin(w) / r - SQRT3/2;
         break;
      case 3:
         w = atan2(y,x);
         y = -r0 * cos(w) / r;
         x = -r0 * sin(w) / r;
         break;
      }

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
