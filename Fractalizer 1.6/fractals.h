
#ifndef FRACTALS_H
#define FRACTALS_H

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

//Dimensioni dell'immagine
int sw = 600;
int sh = 600;

float a,b;
float d; //per hopalong
//int inc; // per hopalong

float x,y,xnew,ynew;
int c;
int i=0;
int j=0;
bool done = false;

// DISTORSIONE
float k = 1.0 ;
float w = 1.0 ;
float v = 1.0 ;

bool keys[256];
char str1[15];
char str2[15];
char str3[15];
char str4[15];
char str5[15];
char str6[15];
char strs1[15];
char strs2[15];
char strs3[15];
char strs4[15];
char strs5[15];
char strs6[15];

//fattori spettrali
float cb,cg,cr;
float s1,s2,s3;

//selettori di algoritmi
enum { MANDEL, MANDELINV, JULIA, JULIAN, JULIA_CUBIC, NEWTON_RAPHSON, NEWTON_RAPHSON2, NEWTON_RAPHSON3, HOPALONG, QUADRUP2, THREEPLY };
int alg = MANDEL; // per default

// FATTORI
float zoom = 	0.6;
float a1 = 	-1.3; //  a destra<-- sposta --> a sinistra
float a2 = 	2.0;  //  si allarga<--orizzontale--> si stringe 
float b1 = 	0.9;  //  in alto<-- sposta --> in basso
float b2 = 	-2.0; //  si stringe<--verticale-->si allunga

HDC hdc;

int NC=52; //numero dei colori
COLORREF colours[256];

void redraw()
{
	done = false;
	i = 0; 
	j = 0;
}

float random()
{ return (rand()%100)*0.01; }

void getSpectrum(COLORREF* col)
{	
	int bb,rr,gg;

	s1 = random();
	s2 = random();
	s3 = random();

	cb = 1 + 3 * random();
	cg = 1 + 3 * random();	
	cr = 1 + 3 * random();
	for(int i = 0; i < NC; i++)
	{
		float f = i* 1.0/NC;
		bb = (int) (126 + 126 * sin(cb * 2 * M_PI * (f + s1)));	
		gg = (int) (126 + 126 * sin(cg * 2 * M_PI * (f + s2)));	
		rr = (int) (126 + 126 * sin(cr * 2 * M_PI * (f + s3)));
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


	if(alg == MANDELINV)
	{
		float def = 1 / (a*a + b*b);
		a = a * def;
		b = -b * def;
		x = a;
		y = b;
	}

	if(alg == JULIA || alg == JULIA_CUBIC)
	{
		a = 0.42;
		b = 0.23;	
	}
	
	if(alg == JULIAN )
	{
		a = w;
		b = v;
		w = 1;
		v = 1;	
	}

	for(c=0; c < NC; c++)
	{
		float denom = 0;
		switch(alg)
		{
			case JULIA_CUBIC:
				xnew = w * x * x * x - 3 * v * x * y * y + a;
				ynew = - w * y * y * y + 3 * v * x * x * y + b;
				break;
			case NEWTON_RAPHSON:
				denom = w*3*((x*x - y*y)*(x*x - y*y) + 4*x*x*y*y);
				if (denom == 0) denom = .0000001;
	 			xnew = k*0.6666667*x + (x*x - y*y)/denom;
	 			ynew = v*0.6666667*y - 2*x*y/denom;
				break;
			case NEWTON_RAPHSON2:
				denom = w*4*((x*x*x - 3*x*y*y)*(x*x*x - 3*x*y*y) + (-y*y*y+3*x*x*y)*(-y*y*y+3*x*x*y));
				if (denom == 0) denom = .0000001;
	 			xnew = k*0.75*x + (x*x*x - 3*x*y*y)/denom;
	 			ynew = v*0.75*y - (-y*y*y + 3*x*x*y)/denom;
				break;
			case NEWTON_RAPHSON3:
				denom = w*5*((x*x*x*x + y*y*y*y - 6*x*x*y*y)*(x*x*x*x + y*y*y*y - 6*x*x*y*y) + 16*(x*x*x*y-x*y*y*y)*(x*x*x*y-x*y*y*y));
				if (denom == 0) denom = .0000001;
	 			xnew = k*0.8*x + (x*x*x*x + y*y*y*y - 6*x*x*y*y)/denom;
	 			ynew = v*0.8*y - 4*(x*x*x*y - x*y*y*y)/denom;
				break;
			default:
				xnew = w * x * x - v * y * y + a;
				ynew = 2 * k * x * y + b;
				break;
		}

		if( xnew == x && ynew == y){
			if(alg != NEWTON_RAPHSON)
				if(alg != NEWTON_RAPHSON2)
					if(alg != NEWTON_RAPHSON3)
					c = NC;
			break;
		}
		x = xnew;
		y = ynew;
		if(alg != NEWTON_RAPHSON )
			if(alg != NEWTON_RAPHSON2)
				if(alg != NEWTON_RAPHSON3)
				if( x*x + y*y > 4)
					break;
	}	   
	SetPixel(hdc,i,j,colours[c]);

	if(alg == JULIAN)
	{
		w = a;
		v = b;	
	}	
}


void draw_orbital(HDC hdc,int& i, int& j)
{
	a = sw/2;
	b = sh/2;
	float f = zoom/2;

	xnew = (float) i;
	ynew = (float) j;
	int xpix,ypix;

	for(int pp=0; pp < 5000; pp++){
		x = xnew;
		y = ynew;
		if(alg == HOPALONG)
		{
			xnew = y + ((x<0) ? sqrt(fabs(w*x-v)) : -sqrt(fabs(w*x-v)));
			ynew = k - x;
		}
		if(alg == QUADRUP2)
		{	
			xnew = y + ((x<0) ? sin(log(fabs(w*x-v)))*atan(fabs(v*x-w)*fabs(v*x-w)) : -sin(log(fabs(w*x-v)))*atan(fabs(v*x-w)*fabs(v*x-w))) ;
			ynew = k - x;
		}
		if(alg == THREEPLY)
		{	
			xnew = y + ((x<0) ? fabs(sin(x)*cos(w)+v-x*sin(k+w+v)) : -fabs(sin(x)*cos(w)+v-x*sin(k+w+v)) ) ;
			ynew = k - x;
		}
		xpix = (int)(a + (xnew+ynew)*f - 10*a1*f);
		ypix = (int)(b - (xnew-ynew)*f + 10*b1*f);

		SetPixel(hdc,xpix,ypix,colours[c]);
	}

	i = (int)xnew;
	j = (int)ynew;
}


void drawParameters(HWND hDlg)
{
	// visualizza i parametri sulla finestra
 	sprintf(str1, "%f",k);
	sprintf(str2, "%f",w);
	sprintf(str3, "%f",v);
	sprintf(str4, "%f",zoom);
	sprintf(str5, "%f",a1);
	sprintf(str6, "%f",b1);

	SetDlgItemText(hDlg, IDD_TESTO_K,str1);
	SetDlgItemText(hDlg, IDD_TESTO_W,str2);
	SetDlgItemText(hDlg, IDD_TESTO_V,str3);
	SetDlgItemText(hDlg, IDD_TESTO_ZOOM,str4);
	SetDlgItemText(hDlg, IDD_TESTO_A,str5);
	SetDlgItemText(hDlg, IDD_TESTO_B,str6);

	//fattori spettrali
 	sprintf(strs1, "%f",s1);
	sprintf(strs2, "%f",s2);
	sprintf(strs3, "%f",s3);
	sprintf(strs4, "%f",cr);
	sprintf(strs5, "%f",cg);
	sprintf(strs6, "%f",cb);

	SetDlgItemText(hDlg, IDD_TESTO_S1,strs1);
	SetDlgItemText(hDlg, IDD_TESTO_S2,strs2);
	SetDlgItemText(hDlg, IDD_TESTO_S3,strs3);
	SetDlgItemText(hDlg, IDD_TESTO_CR,strs4);
	SetDlgItemText(hDlg, IDD_TESTO_CG,strs5);
	SetDlgItemText(hDlg, IDD_TESTO_CB,strs6);

}


void getParameters(HWND hDlg,COLORREF* col)
{
	int bb,rr,gg;

	GetDlgItemText(hDlg, IDD_TESTO_K,str1,8);
	GetDlgItemText(hDlg, IDD_TESTO_W,str2,8);
	GetDlgItemText(hDlg, IDD_TESTO_V,str3,8);
	GetDlgItemText(hDlg, IDD_TESTO_ZOOM,str4,8);
	GetDlgItemText(hDlg, IDD_TESTO_A,str5,8);
	GetDlgItemText(hDlg, IDD_TESTO_B,str6,8);

	k = atof(str1);
	w = atof(str2);
	v = atof(str3);
	zoom = atof(str4);
	a1 = atof(str5);
	b1 = atof(str6);
	
	//fattori spettrali	
	GetDlgItemText(hDlg, IDD_TESTO_S1,strs1,8);
	GetDlgItemText(hDlg, IDD_TESTO_S2,strs2,8);
	GetDlgItemText(hDlg, IDD_TESTO_S3,strs3,8);
	GetDlgItemText(hDlg, IDD_TESTO_CR,strs4,8);
	GetDlgItemText(hDlg, IDD_TESTO_CG,strs5,8);
	GetDlgItemText(hDlg, IDD_TESTO_CB,strs6,8);

	s1 = atof(strs1);
	s2 = atof(strs2);
	s3 = atof(strs3);
	cr = atof(strs4);
	cg = atof(strs5);
	cb = atof(strs6);

	for(int i = 0; i < NC; i++)
	{
		float f = i* 1.0/NC;
		bb = (int) (126 + 126 * sin(cb * 2 * M_PI * (f + s1)));	
		gg = (int) (126 + 126 * sin(cg * 2 * M_PI * (f + s2)));	
		rr = (int) (126 + 126 * sin(cr * 2 * M_PI * (f + s3)));
		col[i] = RGB(rr,gg,bb);
	}

}


void resetParameters()
{
	zoom = 	0.6;
	a1 = 	-1.3;
	a2 = 	2.0; 

	b1 = 	0.9;
	b2 = 	-2.0;
	k = 1.0 ;
	w = 1.0 ;
	v = 1.0 ;
}

void increaseK()
{ k += 0.1; }
void decreaseK()
{ k -= 0.1; }
void increaseW()
{ w += 0.1; }
void decreaseW()
{ w -= 0.1; }
void increaseV()
{ v += 0.1; }
void decreaseV()
{ v -= 0.1; }
void moveToLeft()
{ a1 += 0.1; }
void moveToRight()
{ a1 -= 0.1; }
void moveToUp()
{ b1 -= 0.1; }
void moveToDown()
{ b1 += 0.1; }
void zoomIn()
{ zoom += 0.2; }
void zoomOut()
{ zoom -= 0.2; }
void changeSpectrum()
{ getSpectrum(colours); }
					
void iter_tipo_mandel(HWND hWnd,HWND hWndTBX, int& i, int& j)
{
	i++;
	if(i == sw) {
		i = 0;
		j++;
		if(j == sh){
			done = true;
		}
	}
	if(!done)  {
		hdc = GetDC(hWnd);	
  		draw(hdc);
		//disegno solo una volta il parametro
		if(i == 1, j == 1)
			drawParameters(hWndTBX);
		ReleaseDC(hWnd,hdc);
 	}
}

int random100()
{
	return int(random()*1000);
}

#define NSTEPS 12
#define COUNT (1 << NSTEPS)

void iter_tipo_mandel2(HWND hWnd,HWND hWndTBX, int& i, int& j)
{
	int x,y,x1,x2,y1,y2;
	int xstep, ystep, xnextStep, ynextStep;
	int backwards = random100() & 1;
	xstep = (backwards ? -COUNT : COUNT);
	ystep = COUNT;
	x = i;
	y = j;
	for (int pp = 0; pp < NSTEPS; pp++)
	{
		xnextStep = xstep / 2;
		ynextStep = ystep / 2;
		for (x = (backwards ? sw-1 : 0);(backwards ? x >= 0 : x < sw);x += xstep)
		{
			x1 = x + xnextStep;
			if (x1 < 0)
				x1 = sw-1;
			else if (x1 >= sw)
				x1 = 0;

			x2 = x + xstep;
			if (x2 < 0)
				x2 = sw-1;
			else if (x2 >= sw)
				x2 = 0;

			for (y = 0; y < sh; y += ystep)
			{
				y1 = y + ynextStep;
				if (y1 < 0)
					y1 = sh-1;
				else if (y1 >= sh)
					y1 = 0;

				y2 = y + ystep;
				if (y2 < 0)
					y2 = sh-1;
				else if (y2 >= sh)
					y2 = 0;
				
				hdc = GetDC(hWnd);	

  				i = x1; j = y;
				draw(hdc);
  				i = x; j = y1;
				draw(hdc);
  				i = x1; j = y1;
				draw(hdc);
				i = x; j = y;
				//draw(hdc);
				ReleaseDC(hWnd,hdc);	
			}	
		}
	  
		xstep = xnextStep;
		ystep = ynextStep;
		}
		drawParameters(hWndTBX);
}

void clrScreen(HWND hwnd)
{
	HBRUSH black_brush = CreateSolidBrush(RGB(0,0,0));
	HDC hdc = GetDC(hwnd);
	SelectObject(hdc,black_brush);
	Rectangle(hdc,0,0,sw,sh);
  	DeleteObject(black_brush);
	ReleaseDC(hwnd,hdc);
}

void iter_tipo_orbital(HWND hWnd,HWND hWndTBX, int& i, int& j)
{
	HDC hdc = GetDC(hWnd);
	clrScreen(hWnd);
	for(c=0 ; c < NC; c++){
		draw_orbital(hdc,i,j);
	}
	ReleaseDC(hWnd,hdc);
	drawParameters(hWndTBX);
}


#endif
