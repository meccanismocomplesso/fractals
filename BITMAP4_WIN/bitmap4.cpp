#include <windows.h>
#include <windowsx.h>
#include "ico_res.h"
#include "PaintDc.h"
#include "Color.h"
#include "DibSection.h"

//Dimensioni della WINDOW
int sw = 450;
int sh = 450;
int colore = 0;

DibSection dibsection;

bool onCreate(HWND hwnd)
{
	dibsection.Size(sw,sh);
	for(int i=0; i<256;i++)
		dibsection.PixelAt(i,i,RGB(255,colore,colore));
	return true;
}

void onDestroy(HWND hwnd)
{
}

void iter()
{
	if(colore < 255)
		colore++;
	else
		colore =0;
	for(int i=0; i<256;i++)
		dibsection.PixelAt(i,i,RGB(255,colore,colore));
}

void onPaint(HWND hwnd)
{
	PaintDc pdc(hwnd);
	dibsection.BitBltTo(pdc,0,0);
}

/* ********************************************************* */
/* ********************************************************* */
LRESULT CALLBACK
MainWndProc (HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
        switch (nMsg)
        {
                case WM_CREATE:
						onCreate(hwnd);
						return 0;
                        break;
               
                case WM_DESTROY:
						onDestroy(hwnd);
                        PostQuitMessage (0);
                        return 0;
                        break;
        
				case WM_PAINT:
					{
						iter();
						onPaint(hwnd);
						return 0;
						break;
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
                "Bitmap4 - The Blue Ant",   /* Caption */
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
