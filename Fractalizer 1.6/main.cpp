#include <math.h>
#include <stdio.h>

#define STEP 100

//Dimensioni dell'immagine
int sw = 600;
int sh = 600;

float a,b;
float d;
float x,y,xnew,ynew;
int c;
int i=0;
int j=0;
bool done = false;

void draw_orbital()
{
	a = 2.5;
	b = 0.5;
	d = 3.2;
	int xpix,ypix;

	for(int pp=0; pp < 1000; pp++){
		x = xnew;
		y = ynew+STEP;
		xnew = x + ((x<0) ? sqrt(fabs(b*x-d)) : -sqrt(fabs(b*x-d)));
		ynew = a - x;

		xpix = sh/2 + (int)(xnew+ynew);
		ypix = sw/2 + (int)(xnew-ynew);

		//SetPixel(hdc,i,j,colours[c]);
	}
}

int main()
{
	printf("
	return 0;
}
