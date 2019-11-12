/* September 2002: Craig Wilkie <craig.wilkie@bigfoot.com> General Utilities for Windows Screensavers */
/* Febrary 2006: The-BlueAnt : Added and Modified some functions */

#include <windows.h>
#include <scrnsave.h>
#include <stdio.h>
#include <math.h>
#include "utils.h"


float random()
{
	return (rand()%100)*0.01;
}

int random100()
{
	return int(random()*1000);
}

void write_label(HDC hdc, int sw, int sh)
{
	char str[20];
	sprintf(str,"www.the-blueant.com");
	SetTextColor(hdc,RGB(255,255,255));
	SetBkColor(hdc,RGB(0,0,0));
	TextOut(hdc,(int)(0.8*sw),(int)(0.9*sh),str,19);

}

/* Allocate a spectrum of ncolours on a COLORREF array */
COLORREF *colSpectrum(long ncolours)
{	
	COLORREF *array;
	int bb,rr,gg;
	float cb,cg,cr;
	float a1,a2,a3;

	array = (COLORREF *)malloc(ncolours * sizeof(COLORREF));
    
	a1 = random();
	a2 = random();
	a3 = random();

	cb = 1 + 3 * random();
	cg = 1 + 3 * random();	
	cr = 1 + 3 * random();
	for(int i = 0; i < ncolours; i++)
	{
		float f = i* 1.0/ncolours;
		
		bb = (int) (127.5 + 127.5 * sin(cb * 2 * M_PI * (f + a1)));	
		gg = (int) (127.5 + 127.5 * sin(cg * 2 * M_PI * (f + a2)));	
		rr = (int) (127.5 + 127.5 * sin(cr * 2 * M_PI * (f + a3)));
		array[i] = RGB(rr,gg,bb);
	}

	return array;
}

/* questo tipo di spettro è coerente, cioè inizia e finisce con lo stesso colore 
	Dimezzo lo spettro lo faccio andare fino a metà della palette e poi lo ripeto sull'altra metà
	a rovescio. 
	Si ha però l'effetto di colori duplicati
 */
COLORREF *colSpectrumCoherent(long ncolours)
{	
	COLORREF *array;
	int bb,rr,gg;
	float cb,cg,cr;
	float a1,a2,a3;
	int halfsp = ncolours/2;

	array = (COLORREF *)malloc(ncolours * sizeof(COLORREF));
    
	a1 = random();
	a2 = random();
	a3 = random();

	cb = 1 + 3 * random();
	cg = 1 + 3 * random();	
	cr = 1 + 3 * random();
	
	for(int i = 0; i <= halfsp; i++)
	{
		float f = i* 1.0/ncolours;
		
		bb = (int) (127.5 + 127.5 * sin(cb * 2 * M_PI * (f + a1)));	
		gg = (int) (127.5 + 127.5 * sin(cg * 2 * M_PI * (f + a2)));	
		rr = (int) (127.5 + 127.5 * sin(cr * 2 * M_PI * (f + a3)));
		array[i] = RGB(rr,gg,bb);
	}
	for(int i = 1; i < halfsp; i++)
	{
		array[halfsp+i] = array[halfsp-i];
	}



	return array;
}


/* Allocate an array full of random colours */
COLORREF *create_random_colorrefs(long count)
{
    COLORREF *array;
    long loop;

    array = (COLORREF *)malloc(count * sizeof(COLORREF));
    
    for (loop = 0; loop < count; loop++)
    {
        array[loop] = get_random_color();
    }

    return array;
}

/* Get the height and width of a window */
void get_window_sizes(HWND window, long *width, long *height)
{
    RECT client;

    GetClientRect(window, &client);

    *width = client.right - client.left;
    *height = client.bottom - client.top;
}

/* Get a random colour, 0 - 255 */
COLORREF get_random_color(void)
{
    return ((rand() % 256) << 16) + ((rand() % 256) << 8) + (rand() % 256);
}

/* Get a random colour 128 - 255 */
COLORREF get_random_bright_color(void)
{
    return ((rand() % 128) << 17) + ((rand() % 128) << 9) + ((rand() % 128) << 1);
}

/* Check the window message queue for relevant messages */
long check_message_queue(HWND window)
{
    MSG msg;
	static long mouse_move_messages = 0;

    /* Check if the screensaver should terminate */
    if (PeekMessage(&msg, window, 0, 0, PM_NOREMOVE) == TRUE)
    {
        /* Absorb a few mouse move messages to avoid spurious exits */
		if (msg.message == WM_MOUSEMOVE)
		{
			PeekMessage(&msg, window, 0, 0, PM_REMOVE);

			mouse_move_messages++;

			if (mouse_move_messages == 10)
			{
				return 1;
			}
		}
		else if ((msg.message == WM_LBUTTONDOWN)||
            (msg.message == WM_RBUTTONDOWN)||
            (msg.message == WM_MBUTTONDOWN)||
            (msg.message == WM_KEYDOWN)||
            (msg.message == WM_KEYUP)||
            (msg.message == WM_ACTIVATE)||
            (msg.message == WM_ACTIVATEAPP)||
            (msg.message == WM_NCACTIVATE)||
            (msg.message == WM_SETCURSOR)||
            (msg.message == WM_DESTROY)||
            (msg.message == WM_SYSCOMMAND)||
            (msg.message == WM_CLOSE))
        {
			return 1;
        }
    }
    return 0;
}

