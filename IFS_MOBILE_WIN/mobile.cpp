// 27 Marzo 2006 by The Blue Ant <http:\\www.the-blueant.com> <fabio.nelli@virgilio.it
// IFS NAVIGATOR

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "ico_res.h"

//Dimensioni della WINDOW
int sw = 450;
int sh = 450;

bool keys[256];

int m = 0,n = 0; // indici di matrice M[m][n]
float x,y,xnew,ynew;
int c;
int i,j,k;
double eps = 0.001;
double f1 = 0.0;

#define LINES 4
//PARAMETERS
int dots = 100000;
HDC hdc;
bool done = false;

HPEN penna;
char str1[15];

static double map[LINES][7] = {
	{	0.0,	0.0,	0.0,	0.16,	0.0,	0.0,	0.01},
	{	0.2,	-0.26,	0.23,	0.22,	0.0,	1.6,	0.07},
	{	-0.15,	0.28,	0.26,	0.24,	0.0,	0.44,	0.07},
	{	0.75,	0.04,	-0.04,	0.85,	0.0,	1.6,	0.85}
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
		int drawx = (int)(u*centerx*-0.4);
		int drawy = (int)(v*centery*-0.2);

		SetPixel(hdc, drawx+centerx, drawy+sh,RGB(255,255,255));
	}
	// visualizza i parametri sulla finestra
 	sprintf(str1, "[%u][%u]",m,n);
	SetTextColor(hdc,RGB(255,255,255));
	SetBkColor(hdc,RGB(0,0,0));
	TextOut(hdc,200,10,str1,9);
}

void iter()
{
	map[m][n] -= eps;
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
						hdc = GetDC(hwnd);	
  						draw(hdc);
						//Sleep(1000);
						iter();
						RECT client;
						GetClientRect(hwnd, &client);
						FillRect(hdc, &client, (HBRUSH)GetStockObject(BLACK_BRUSH));
						ReleaseDC(hwnd,hdc);
						return 0;
						break;
				case WM_KEYDOWN:				
					{
						switch(wParam)
						{
							case VK_RIGHT:
								//increase m
								if( m < LINES-1)
									m += 1;
							break;
							case VK_LEFT:
								//decrease m
								if( m > 0)
									m -= 1;
							break;
							case VK_UP:
								//increase m
								if( n < 6)
									n += 1;
							break;
							case VK_DOWN:
								//decrease m
								if( n > 0)
									n -= 1;
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
                "IFS Navigator - The Blue Ant",   /* Caption */
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
