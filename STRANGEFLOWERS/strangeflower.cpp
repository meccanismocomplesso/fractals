#include <windows.h>
#include <scrnsave.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <time.h>
#include "resource.h"
#include "options.h"
#include "utils.h"

long delay;
long ncolours;

OPTION options[] =
{
/*	 Type            variable     control              default minimum maximum name */
    {LONG_EDIT_TYPE, &delay,      IDC_DELAY_EDIT,      200,    1,      10000,    "Delay"},
    {LONG_EDIT_TYPE, &ncolours,   IDC_COLOURS_EDIT,    256,     1,      256,    "Colours"},
   {-1, NULL, -1, -1, -1, -1, ""}
};

OPTIONS_STRUCT screenhack_options = 
{
    "StrangeIcon", options
};

float a[12];
//Dimensioni della WINDOW
int sw;
int sh;
int nmax = 100000; //numero massimo di iterazioni 


#define NC 256 //numero dei colori
//COLORREF colore = RGB(255,255,255);
//COLORREF colours[NC];
COLORREF* colours;

float x,y,z;
float xe,ye,ze;
float xnew,ynew,znew;
float xp,yp;

float xz,yz;
float xl,xh,yl,yh,zl,zh; 

int n,nl;
float lsum;
int ns; //numero dei settori
float of; //fattore di sovrapposizione (overlap factor)

// ****** VARIABLES *****************
int s;
float th;
int draw_x,draw_y;
float l;
float xsave,ysave,zsave;
float dlx,dly,dlz;
double dl2,df,rs;
int counter = 0;


void init_scale()
{
	xl = 1000000;
	xh = -xl;
	yl = xl;
	yh = xh;
	zl = xl;
	zh = xh;
}


void init()
{
	x = 0.05;
	y = 0.05;
	z = 0.05;
	xe = x + 0.000001;
	ye = y;
	ze = z;
	lsum = 0;
	n = 0;
	nl = 0;

	of = 0.5 + 1.5 * random();
	ns = (int) (2 + 8*random());

	for(int i=0; i < 12; i++)
   	{
       	a[i] = ((int)(25 * random()) -12)*0.1;
   	}

}     

void iterate()
{
	xnew = a[0] + x*(a[1]+a[2]*x+a[3]*y)+y*(a[4]+a[5]*y);
	ynew = a[6] + x*(a[7]+a[8]*x+a[9]*y)+y*(a[10]+a[11]*y);
	znew = x * x + y * y;
}

void rescaling()
{
	if ( x < xl) xl = x;
	if ( x > xh) xh = x;
	if ( y < yl) yl = y;
	if ( y > yh) yh = y;
	if ( z < zl) zl = z;
	if ( z > zh) zh = z;
}

void predraw()
{
	xz = 0.05 * sw / (zh - zl);
	yz = 0.05 * sh / (zh - zl);

}

void draw(HDC hdc)
{
		// E' necessario calcolare il numero del pixel in base al
		// valore massimo trovato e poi convertito in un intero
		if ( xh > xl )
		{
			xp = sw * (x -xl) / (xh - xl);
			//xp = sw * x / (xh - xl);
		}
		if (yh > yl )
		{ 
			yp = sh * (yh -y) / (yh -yl);
			//yp = sh * y / (yh -yl);
		}
		s = (int) (ns * random()); //scelta random del settore
		th = 2 * M_PI *(of * yp / sh + s)/ns;
		if ((ns % 2)== 0 && (s % 2)== 0)
			th = 2 * M_PI / ns - th;
		yp =  0.5 * sh * ( 1 + xp * cos(th) / sw);
		xp =  0.5 * sh * ( 1 + xp * sin(th) / sw);

		//xp = xp + xz*(z -zl);
		//yp = yp + yz*(z -zl);

		draw_x = (int) (xp + sw/7);
		draw_y = (int) yp; 
		
		// il colore corrisponde alla profondità
		int c = 2 + ((int)(NC*(z - zl)/(zh-zl) + NC)) % NC;	//colore
		SetPixel(hdc, draw_x, draw_y, colours[c]);
		

}

int test()
{
	l = 0; //esponente di Lyapunov
        if( (abs((int)xnew) + abs((int)ynew) + abs((int)znew)) > 1000000 ) return 1;
	xsave = xnew;
	ysave = ynew;
	zsave = znew;
	x = xe;
	y = ye;
	z = ze;
	//n--;
	iterate();
	//n++;
	dlx = xnew - xsave;
	dly = ynew - ysave;
	dlz = znew - zsave;
	dl2 = dlx * dlx + dly * dly + dlz * dlz;
	if ( dl2 != 0)
	{
		df = 1000000000 * dl2 * 1000;

		rs = 1 / sqrt(df);

		xe = xsave + rs * dlx;
		xnew = xsave;
		ye = ysave + rs * dly;
		ynew = ysave;
		ze = zsave + rs * dlz;
		znew = zsave;
		lsum = lsum + log(df);
		nl++;
		l = 0.721347 * lsum / nl;
	}


	if( n > 100 && l < 0.005 ) return 1;
	if( n > nmax ) return 1;
	if(xnew > 1000000 || ynew > 1000000 || znew > 1000000 ) return 1;
	if(xnew < -1000000 || ynew < -1000000 || znew < -1000000 ) return 1;

	x = xnew;
	y = ynew;
	z = znew;

	return 0;
}

void stricon_draw(HDC hdc, HWND hwnd)
{

	RECT client;
	GetClientRect(hwnd, &client);
				
	while(n< 50000 && counter < 5)
	{
		iterate();

		if ( n > 6000 && n < 7000)
			rescaling();
		if (n == 7000)
		{

			predraw();
			counter++;
			if(counter > 4)
			{

			}

		}
		if( n > 7000)
		{
			//draw(GetDC(hwnd));
			HDC hdc2 = GetDC(hwnd);
			draw(hdc2);
			ReleaseDC(hwnd,hdc2);
		}
		n++;
		if( n == 50000 || test()==1 ){
			 init();
		}		

	}

	Sleep(2000);				
	FillRect(hdc, &client, (HBRUSH)GetStockObject(BLACK_BRUSH));
	write_label(hdc,sw,sh);

	counter = 0;
	init_scale();
}

void screenhack_init (HDC hdc, HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	sh = rect.bottom;
	sw = rect.right;
	init_scale();
	init();
	colours = colSpectrum(256);
	
	/* Set the screenhack timer */
    SetTimer(hwnd, 10, delay, NULL);
}

void screenhack_loop(HDC hdc, HWND hWnd)
{
    RECT client;
    GetClientRect(hWnd, &client);
	if (n < 1){
		FillRect(hdc, &client, (HBRUSH)GetStockObject(BLACK_BRUSH));
		write_label(hdc,sw,sh);
	}
	colours = colSpectrum(ncolours);
	stricon_draw(hdc,hWnd);
	
	
}
