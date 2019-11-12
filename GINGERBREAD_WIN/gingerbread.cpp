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

//PARAMETERS
double t = 8.5, b = -4.5, l = -4.5, r = 8.5;
int dots = 10000;
HDC hdc;
bool done = false;


#define NC 52 //numero dei colori
COLORREF colours[NC];

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

#define FABSF(n) ((float)fabs((double)(n)))

void draw(HDC hdc)
{
	int factor = (int)dots/NC;
	factor += 1;
	x = -0.1; 
	y = 0.0;
	c = 0;
	for(k = 0; k < dots; k++)
	{
		xnew = 1 - y + FABSF(x);
		ynew = x;
		x = xnew;
		y = ynew;
		int drawx = (int)((double)(x - l) / (r - l) * sw);
		int drawy = (int)((double)(y - t) / (b - t) * sh);
		SetPixel(hdc,drawx, drawy,colours[c]);
		if(k % factor == 0)
			c++;
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
						getSpectrum(colours);
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
                "PopCorn - The Blue Ant",   /* Caption */
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
