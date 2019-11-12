#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "ico_res.h"

//Dimensioni della WINDOW
int sw = 500;
int sh = 470;

float a,b;
float x,y,xnew,ynew;
int c;
int i=0;
int j=0;

// DISTORSIONE
float k = 1.0 ;
float w = 1.0 ;
float v = 1.0 ;

bool keys[256];
char str1[15];
char str2[15];
char str3[15];

// FATTORI
float zoom = 	0.6;
float a1 = 	-1.3; //  a destra<-- sposta --> a sinistra
float a2 = 	2.0;  //  si allarga<--orizzontale--> si stringe 
float b1 = 	0.9;  //  in alto<-- sposta --> in basso
float b2 = 	-2.0; //  si stringe<--verticale-->si allunga

int done = 0;
HDC hdc;

#define NC 128 //numero dei colori
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

void draw(HDC hdc)
{
	float f = 1/zoom;
	a = (a1 + a2 * i /sh)*f;
    b = (b1 + b2 * j /sw)*f;
	x = a;
	y = b;
	float denom = 0;

	for(c=0; c < NC; c++)
	{
		denom = w*3*((x*x - y*y)*(x*x - y*y) + 4*x*x*y*y);
		if (denom == 0) denom = .0000001;
		xnew = k*0.6666667*x + (x*x - y*y)/denom;
		ynew = v*0.6666667*y - 2*x*y/denom;
		
		if( xnew == x && ynew == y){
			break;
		}
		x = xnew;
		y = ynew;
	}
	//if(c != 0 && c != NC)	   
		SetPixel(hdc,i,j,colours[c]);


}

void drawParameters(HDC hdc)
{
	// visualizza i parametri sulla finestra
 	sprintf(str1, "k = %f",k);
	sprintf(str2, "w = %f",w);
	sprintf(str3, "v = %f",v);
	SetTextColor(hdc,RGB(255,255,255));
	SetBkColor(hdc,RGB(0,0,0));
	TextOut(hdc,10,10,str1,15);
	TextOut(hdc,10,30,str2,15);
	TextOut(hdc,10,50,str3,15);
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
		
		case WM_KEYDOWN:				
		{
			switch(wParam)
			{
				case VK_RIGHT:
					//increase k
					k += 0.1;
					done = 0;
					i = 0; j = 0;
				break;
				case VK_LEFT:
					//decrease k
					k -= 0.1;
					done = 0;
					i = 0; j = 0;
				break;
				case VK_UP:
					//increase w
					w += 0.1;
					done = 0;
					i = 0; j = 0;
				break;
				case VK_DOWN:
					//decrease w
					w -= 0.1;
					done = 0;
					i = 0; j = 0;
				break;				
				case VK_F1:
					//zoom in
					zoom += 0.1;
					done = 0;
					i = 0; j = 0;
				break;
				case VK_F2:
					//zoom out
					zoom -= 0.1;
					done = 0;
					i = 0; j = 0;
				break;
				case VK_TAB:
					//increase v
					v += 0.1;
					done = 0;
					i = 0; j = 0;
				break;
				case VK_SHIFT:
					//decrease v
					v -= 0.1;
					done = 0;
					i = 0; j = 0;
				break;
				case VK_SPACE:
					//cambia lo spettro
					getSpectrum(colours);
					done = 0;
					i= 0; j = 0;
				break;
				default:
					keys[wParam] = TRUE;
				break;
			}		
			return 0;				
		}

		case WM_KEYUP:				
		{
			keys[wParam] = FALSE;		
			return 0;				
		}
                case WM_PAINT:
			i++;
			if(i == sw) {
				i = 0;
				j++;
				if(j == sh){
					done = 1;
				}
			}
			if(done == 0)  {
			    hdc = GetDC(hwnd);	
  			    draw(hdc);
			    //disegno solo una volta il parametro
			    if(i == 1, j == 1)
				drawParameters(hdc);
			    ReleaseDC(hwnd,hdc);		   
 			}
			
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
                "Mandelbrot - Fabio",   /* Caption */
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
