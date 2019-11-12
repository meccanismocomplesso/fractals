#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "ico_res.h"

//Dimensioni della WINDOW
int sw = 100;
int sh = 100;

int c;
int i,j,k;
float x,y,xnew,ynew;
//PARAMETERS
float h = 0.05;
double t = 8.5, b = -4.5, l = -4.5, r = 8.5;
int dots = 1000;
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


void draw()
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
		//SetPixel(hdc,drawx, drawy,colours[c]);
		printf("X = %d Y = %d C = %u\n",drawx,drawy,c);
		if(k % factor == 0)
			c++;
	}
}

int main()
{
	printf("inizio");
	getSpectrum(colours);
	draw();
	return 0;
}
