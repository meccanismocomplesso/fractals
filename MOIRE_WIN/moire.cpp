#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "ico_res.h"

#define NC 256
COLORREF colors[NC];

//Dimensioni della WINDOW
int sw = 300;
int sh = 300;

int factor;
int xo,yo;
int r1,g1,b1;
int r2,g2,b2;
int rt,gt,bt;
int xx,yy;

void init()
{
	xo = (rand() % sw + 1) - (int) sw/2;
	yo = (rand() % sh + 1 ) - (int) sh/2;
    	r1 = 254 * rand() + 1;
    	g1 = 254 * rand() + 1;
    	b1 = 254 * rand() + 1;
    	r2 = 254 * rand() + 1;
    	g2 = 254 * rand() + 1;
    	b2 = 254 * rand() + 1;

	for(int i=1; i <= NC; i++)
	{
	   rt = r1 + (r2 - r1) * i / NC;
           gt = g1 + (g2 - g1) * i / NC;
           bt = b1 + (b2 - b1) * i / NC;
	   colors[i-1] = RGB(rt,gt,bt);
	}
	factor = 1;
}

void draw(HDC hdc)
{
	for(int i=1; i <= sh; i++)
	{
	   for(int j=1; j <= sw; j++)
	   {
		xx = i + xo;
                yy = j + yo;
		int c =(int) (((xx * xx) + (yy * yy)) / (factor*0.1))%NC;
		SetPixel(hdc,i,j,colors[c]);
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
			init();
			return 0;
                        break;
               
                case WM_DESTROY:
                        PostQuitMessage (0);
                        return 0;
                        break;

                case WM_PAINT:
			draw(GetDC(hwnd));
			factor++;
			if(factor > 300)
				init();		
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
                "Moire - The Blue Ant",        /* Caption */
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
        
        ShowWindow (hwndMain, nShow);
        UpdateWindow (hwndMain);

        while (GetMessage (&msg, NULL, 0, 0))
        {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
        }
        return msg.wParam;
}
