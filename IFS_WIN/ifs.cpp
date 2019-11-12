
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "ico_res.h"

float a[12];
//Dimensioni della WINDOW
int sh = 400;
int sw = 400;
COLORREF colore = RGB(255,255,255);

int nmax = 20000;
float xnew,ynew;
float jj0,jj1;
float p;
float lsum;
int n,n1,n2,nl;
float xl,xh,yl,yh;
int deltax,deltay; 
float xp,yp;
float x,y,xe,ye;

float random()
{
     return (rand()%100)*0.01;
}

void init_scale()
{
	xl = 1000000;
	xh = -xl;
	yl = xl;
	yh = xh;
}

void init()
{
     x = 0.05;
     y = 0.05;
     xe = x + 0.000001;
     ye = y; 
     lsum = 0;
     n = 0;
     nl = 0;

     n1 = 0;
     n2 = 0;

     for(int i=0; i < 12; i++)
     {
        a[i] = ((int)(25 * random()) -12)*0.1;
     }

     jj0 = a[0] * a[3] - a[1] * a[2];
     jj1 = a[6] * a[9] - a[7] * a[8];

     if((jj0+jj1) == 0 || jj0 > 1 || jj1 > 1)  
		init();

     p = jj0 / (jj0+jj1); 
}     

void iterate()
{
	int r;
	if(random() > p)
		r = 6;
	else
		r = 0;
	xnew = a[0+r]*x + a[1+r]*y + a[5+r];
	ynew = a[2+r]*x + a[3+r]*y + a[5+r];
}

void rescaling()
{
	   if(x < xl) xl = x;
	   if(x > xh) xh = x;
	   if(y < yl) yl = y;
	   if(y > yh) yh = y;
}

void draw(HDC hdc)
{
	if ( xh > xl )
	{
		xp = sw * (x -xl) / (xh - xl);
	}
	if (yh > yl )
	{ 
		yp = sh * (yh -y) / (yh -yl);
	}	
	int draw_x = (int) xp;
	int draw_y = (int) yp;

	SetPixel(hdc,draw_x,draw_y,colore); 
}


float calc_dimension()
{
	float f = 0.0;
	float xs,ys;
	float dx,dy,d2,d2max;
	if (n >= 200)
	{
	   if( n == 200) 
		d2max = (xh -xl)*(xh -xl) + (yh-yl)*(yh-yl);
	   if( n == 200 || random() < 0.02 )
	   {
		xs = x;
		ys = y;
	   }
           dx = xnew - xs;
	   dy = ynew - ys;
	   d2 = dx * dx + dy * dy;
	   if ( d2 < (0.001*d2max) )
		n2 = n2 +1;
	   if ( d2 < (0.00001 * d2max) )
	   {
		n1 = n1 + 1;	
		f = 0.434294 * log ( n2/n1);
           }
	}
	return f;
}

int test()
{	
	float l = 0; //esponente di Lyapunov
        if( (abs((int)xnew) + abs((int)ynew)) > 1000000 ) return 1;
	float xsave = xnew;
	float ysave = ynew;
	x = xe;
	y = ye;
	iterate();
	float dlx = xnew - xsave;
	float dly = ynew - ysave;
	double dl2 = dlx * dlx + dly * dly;
	if ( dl2 != 0)
	{
		double df = 1000000000 * dl2 * 1000;
		double rs = 1 / sqrt(df);
		xe = xsave + rs * dlx;
		xnew = xsave;
		ye = ysave + rs * dly;
		ynew = ysave;
		lsum = lsum + log(df);
		nl++;
		l = 0.721347 * lsum / nl;
	}
	//******** CONTROLLO SULLA DIMENSIONE ******** 
	//float dd = calc_dimension();
	//if ( n > 1000  && l < -0.2 && dd > 1) return 1;
	//***********************************************
	if ( n > 1000  && l < -0.2) return 1;
	if( n > nmax ) return 1;
	if(xnew > 1000000 || ynew > 1000000 ) return 1;
	if(xnew < -1000000 || ynew < -1000000) return 1;
	if(xnew < 0.000000001 && xnew > -0.000000001) return 1;
	if(ynew < 0.000000001 && ynew > -0.000000001) return 1;
	x = xnew;
	y = ynew;
	return 0;
}

/* ***************************************************************** */
/* ***************************************************************** */

LRESULT CALLBACK
MainWndProc (HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
        switch (nMsg)
        {
                case WM_CREATE:
			srand((unsigned)time(NULL));
			init_scale();
			init();
			return 0;
                        break;

                case WM_DESTROY:
                        PostQuitMessage (0);
                        return 0;
                        break;

                case WM_PAINT:
			iterate();
			if( n == 1800) init_scale();
			if( n > 1800 && n < 2000)
				rescaling();
			if(n == 2000){
  			    Sleep(2000);				
			    HBRUSH black_brush = CreateSolidBrush(RGB(0,0,0));
			    HDC hdc = GetDC(hwnd);
			    SelectObject(hdc,black_brush);
			    Rectangle(hdc,0,0,sw,sh);
  			    DeleteObject(black_brush);
			    ReleaseDC(hwnd,hdc);
			}
			if ( n > 2000)
			{
				HDC hdc = GetDC(hwnd);
				draw(hdc);
				ReleaseDC(hwnd,hdc);
			}
			n++;
			if(n > nmax || test() ==1)
				init();
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
                szMainWndClass,            
                "IFS - The Blue Ant",                 
                WS_OVERLAPPEDWINDOW,        
                CW_USEDEFAULT,              
                CW_USEDEFAULT,              
                sh,              
                sw,              
                NULL,                       
                NULL,                       
                hInst,                      
                NULL                       
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
