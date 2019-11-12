
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <scrnsave.h>
#include <time.h>
#include "utils.h"
#include "ico_res.h"

long delay = 200;
long ncolours = 256;
long count = 5000;
long mode = 0;
#define TIMER 1

int sh = 600;
int sw = 600;
COLORREF* colore;
//int counter;
int col;
// Parameter MODE
#define MARTIN 0
#define POPCORN 7
#define SINE 8
#define EJK1 1
#define EJK2 2
#define EJK3 9
#define EJK4 3
#define EJK5 4
#define EJK6 10
#define RR 5
#define JONG 6
#define OPS 11

typedef struct {
	int         centerx, centery;	/* center of the screen */
	double      a, b, c, d;
	double      i, j;				/* hopalong parameters */
	int         inc;
	int         op;
	int         count;
	int			x;
	int			y;
} hopstruct;

//static hopstruct *hops = (hopstruct *) NULL;
hopstruct hop;

void init_hop(hopstruct* hp)
{
	double      range;

	if (hp == NULL) {
		if ((hp = (hopstruct *) calloc(1,
						 sizeof (hopstruct))) == NULL)
			return;
	}

	hp->centerx = sw / 2;
	hp->centery = sh / 2;
	/* Make the other operations less common since they are less interesting */
	hp->op = (int) mode;

	range = sqrt((double) hp->centerx * hp->centerx +
	     (double) hp->centery * hp->centery) / (1.0 + LRAND() / MAXRAND);
	hp->i = hp->j = 0.0;
	hp->inc = (int) ((LRAND() / MAXRAND) * 200) - 100;

	switch (hp->op) {
		case MARTIN:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0; 
			hp->b = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0; 
			if (LRAND() & 1)
				hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0; 
			else
				hp->c = 0.0;
			break;
		case EJK1:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0; 
			hp->b = (LRAND() / MAXRAND) * 0.4;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 40.0;
			break;
		case EJK2:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0;
			hp->b = pow(10.0, 6.0 + (LRAND() / MAXRAND) * 24.0);
			if (LRAND() & 1)
				hp->b = -hp->b;
			hp->c = pow(10.0, (LRAND() / MAXRAND) * 9.0);
			if (LRAND() & 1)
				hp->c = -hp->c;
			break;
		case EJK3:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0;
			hp->b = (LRAND() / MAXRAND) * 0.35 + 0.5;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 70.0;
			break;
		case EJK4:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 2.0;
			hp->b = (LRAND() / MAXRAND) * 9.0 + 1.0;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 200.0;
			break;
		case EJK5:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 2.0;
			hp->b = (LRAND() / MAXRAND) * 0.3 + 0.1;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 200.0;
			break;
		case EJK6:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0;
			hp->b = (LRAND() / MAXRAND) + 0.5;
			break;
		case RR:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 40.0;
			hp->b = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 200.0;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0;
			hp->d = (LRAND() / MAXRAND) * 0.9;
			break;
		case POPCORN:
			hp->a = 0.0;
			hp->b = 0.0;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * 0.24 + 0.25;
			hp->inc = 100;
			break;
		case JONG:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			hp->b = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			hp->d = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			break;
		case SINE:	/* MARTIN2 */
			hp->a = M_PI + ((LRAND() / MAXRAND) * 2.0 - 1.0) * 0.7;
			break;
	}
	hp->count = 0;
}


void draw_hop(HDC hdc,hopstruct* hp)
{
	double      oldj, oldi;
	int			x;
	int			y;

	if (hp == NULL)
		return;
	hp->inc++;


	for(int i = 0; i < count; i++)
	{
		oldj = hp->j;
		switch (hp->op) {
			case MARTIN:	/* SQRT, MARTIN1 */
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj + ((hp->i < 0)
						? sqrt(fabs(hp->b * oldi - hp->c))
					: -sqrt(fabs(hp->b * oldi - hp->c)));
				x = hp->centerx + (int) (hp->i + hp->j)/8;
				y = hp->centery - (int) (hp->i - hp->j)/8;
				break;
			case EJK1:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? (hp->b * oldi - hp->c) :
							- (hp->b * oldi - hp->c));
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK2:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i < 0) ? log(fabs(hp->b * oldi - hp->c)) :
						-log(fabs(hp->b * oldi - hp->c)));
				x = hp->centerx + (int) (hp->i + hp->j)/2;
				y = hp->centery - (int) (hp->i - hp->j)/2;
				break;
			case EJK3:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? sin(hp->b * oldi) - hp->c :
						-sin(hp->b * oldi) - hp->c);
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK4:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? sin(hp->b * oldi) - hp->c :
						-sqrt(fabs(hp->b * oldi - hp->c)));
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK5:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? sin(hp->b * oldi) - hp->c :
						-(hp->b * oldi - hp->c));
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK6:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - asin((hp->b * oldi) - (long) (hp->b * oldi));
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case RR:	/* RR1 */
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i < 0) ? -pow(fabs(hp->b * oldi - hp->c), hp->d) :
						pow(fabs(hp->b * oldi - hp->c), hp->d));
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
			case POPCORN:
	#define HVAL 0.05
	#define INCVAL 50
				{
					double      tempi, tempj;

					if (hp->inc >= 100)
						hp->inc = 0;
					if (hp->inc == 0) {
						if (hp->a++ >= INCVAL) {
							hp->a = 0;
							if (hp->b++ >= INCVAL)
								hp->b = 0;
						}
						hp->i = (-hp->c * INCVAL / 2 + hp->c * hp->a) * M_PI / 180.0;
						hp->j = (-hp->c * INCVAL / 2 + hp->c * hp->b) * M_PI / 180.0;
					}
					tempi = hp->i - HVAL * sin(hp->j + tan(3.0 * hp->j));
					tempj = hp->j - HVAL * sin(hp->i + tan(3.0 * hp->i));
					x = hp->centerx + (int) (sw / 40 * tempi);
					y = hp->centery + (int) (sh / 40 * tempj);
					hp->i = tempi;
					hp->j = tempj;
				}
				break;
			case JONG:
				if (hp->centerx > 0)
					oldi = hp->i + 4 * hp->inc / hp->centerx;
				else
					oldi = hp->i;
				hp->j = sin(hp->c * hp->i) - cos(hp->d * hp->j);
				hp->i = sin(hp->a * oldj) - cos(hp->b * oldi);
				x = hp->centerx + (int) (hp->centerx * (hp->i + hp->j) / 4.0);
				y = hp->centery - (int) (hp->centery * (hp->i - hp->j) / 4.0);
				break;
			case SINE:	/* MARTIN2 */
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - sin(oldi);
				x = hp->centerx + (int) (hp->i + hp->j);
				y = hp->centery - (int) (hp->i - hp->j);
				break;
		}
		SetPixel(hdc, x, y, colore[col]);
	}
}

void release_hop(hopstruct* hp)
{
	if (hp != NULL) {
		int         screen;	
		(void) free((void *) hp);
		hp = (hopstruct *) NULL;
	}
}

/* *************** MAIN WINDOW ************************* */
LRESULT WINAPI MainWndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	static RECT rect;
	switch(nMsg)
	{
		case WM_CREATE:
			srand(time(NULL));
    		colore = colSpectrum(ncolours);
			init_hop(&hop);
			SetTimer(hwnd, TIMER, 10, NULL);
			return 0;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			KillTimer(hwnd,TIMER);
			return 0;
			break;
	
		case WM_TIMER:
			return 0;
			break;
		
		case WM_PAINT:
			RECT client;
			GetClientRect(hwnd, &client);
			FillRect(GetDC(hwnd), &client, (HBRUSH)GetStockObject(BLACK_BRUSH));

			colore = colSpectrum(ncolours);
			for(col = 0; col <ncolours; col++)
					draw_hop(GetDC(hwnd),&hop);

			init_hop(&hop);
			Sleep(2000);		
			return 0;
			break;
	}

        return DefWindowProc(hwnd,nMsg,wParam,lParam);
}

int STDCALL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow)
{
        HWND    hwndMain;
        MSG     msg;
        WNDCLASSEX      wndclass;
        char*   szMainWndClass = "WinTestWin";

        memset (&wndclass, 0, sizeof(WNDCLASSEX));
        wndclass.lpszClassName = szMainWndClass;
        wndclass.cbSize = sizeof(WNDCLASSEX);
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = MainWndProc;
        wndclass.hInstance = hInst;
        wndclass.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_RESFUND));
        wndclass.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_RESFUND));
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);

        RegisterClassEx(&wndclass);
        hwndMain = CreateWindow(
                szMainWndClass,
                "Hopalong - The Blue Ant",
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                sh,
                sw,
                NULL,
                NULL,
                hInst,
                NULL);

        ShowWindow(hwndMain, nShow);
        UpdateWindow(hwndMain);

        while(GetMessage(&msg, NULL, 0,0))
        {
                TranslateMessage (&msg);
                DispatchMessage(&msg);
        }
        return msg.wParam;
}

