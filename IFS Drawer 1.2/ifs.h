
#ifndef IFS_H
#define IFS_H

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "PaintDc.h"
#include "Color.h"
#include "DibSection.h"

//Dimensioni della WINDOW
int sw = 450;
int sh = 450;

float x,y,xnew,ynew;
//int k;
//int i= 0;
//int j= 0;
bool colore_ON = false;

#define LINES 5
//PARAMETERS
int dots = 100000;
//HDC hdc;

//SIERPINSKY
double map[LINES][7];

//PARAMETERS

float fx,fy;
int	centerx,centery;
float fcx = 0.5;
float fcy = 0.5;

bool keys[256];
char str[LINES][7][10];
char str_fx[5];
char str_fy[5];


float random()
{ return (rand()%100)*0.01; }


void drawParameters(HWND hDlg)
{
	// visualizza i parametri sulla finestra
 	sprintf(str[0][0], "%.5f",map[0][0]);
	sprintf(str[0][1], "%.5f",map[0][1]);
	sprintf(str[0][2], "%.5f",map[0][2]);
	sprintf(str[0][3], "%.5f",map[0][3]);
	sprintf(str[0][4], "%.5f",map[0][4]);
	sprintf(str[0][5], "%.5f",map[0][5]);
	sprintf(str[0][6], "%.5f",map[0][6]);

	SetDlgItemText(hDlg, IDD_A1,str[0][0]);
	SetDlgItemText(hDlg, IDD_B1,str[0][1]);
	SetDlgItemText(hDlg, IDD_C1,str[0][2]);
	SetDlgItemText(hDlg, IDD_D1,str[0][3]);
	SetDlgItemText(hDlg, IDD_E1,str[0][4]);
	SetDlgItemText(hDlg, IDD_F1,str[0][5]);
	SetDlgItemText(hDlg, IDD_G1,str[0][6]);

 	sprintf(str[1][0], "%.5f",map[1][0]);
	sprintf(str[1][1], "%.5f",map[1][1]);
	sprintf(str[1][2], "%.5f",map[1][2]);
	sprintf(str[1][3], "%.5f",map[1][3]);
	sprintf(str[1][4], "%.5f",map[1][4]);
	sprintf(str[1][5], "%.5f",map[1][5]);
	sprintf(str[1][6], "%.5f",map[1][6]);

	SetDlgItemText(hDlg, IDD_A2,str[1][0]);
	SetDlgItemText(hDlg, IDD_B2,str[1][1]);
	SetDlgItemText(hDlg, IDD_C2,str[1][2]);
	SetDlgItemText(hDlg, IDD_D2,str[1][3]);
	SetDlgItemText(hDlg, IDD_E2,str[1][4]);
	SetDlgItemText(hDlg, IDD_F2,str[1][5]);
	SetDlgItemText(hDlg, IDD_G2,str[1][6]);

 	sprintf(str[2][0], "%.5f",map[2][0]);
	sprintf(str[2][1], "%.5f",map[2][1]);
	sprintf(str[2][2], "%.5f",map[2][2]);
	sprintf(str[2][3], "%.5f",map[2][3]);
	sprintf(str[2][4], "%.5f",map[2][4]);
	sprintf(str[2][5], "%.5f",map[2][5]);
	sprintf(str[2][6], "%.5f",map[2][6]);

	SetDlgItemText(hDlg, IDD_A3,str[2][0]);
	SetDlgItemText(hDlg, IDD_B3,str[2][1]);
	SetDlgItemText(hDlg, IDD_C3,str[2][2]);
	SetDlgItemText(hDlg, IDD_D3,str[2][3]);
	SetDlgItemText(hDlg, IDD_E3,str[2][4]);
	SetDlgItemText(hDlg, IDD_F3,str[2][5]);
	SetDlgItemText(hDlg, IDD_G3,str[2][6]);

 	sprintf(str[3][0], "%.5f",map[3][0]);
	sprintf(str[3][1], "%.5f",map[3][1]);
	sprintf(str[3][2], "%.5f",map[3][2]);
	sprintf(str[3][3], "%.5f",map[3][3]);
	sprintf(str[3][4], "%.5f",map[3][4]);
	sprintf(str[3][5], "%.5f",map[3][5]);
	sprintf(str[3][6], "%.5f",map[3][6]);

	SetDlgItemText(hDlg, IDD_A4,str[3][0]);
	SetDlgItemText(hDlg, IDD_B4,str[3][1]);
	SetDlgItemText(hDlg, IDD_C4,str[3][2]);
	SetDlgItemText(hDlg, IDD_D4,str[3][3]);
	SetDlgItemText(hDlg, IDD_E4,str[3][4]);
	SetDlgItemText(hDlg, IDD_F4,str[3][5]);
	SetDlgItemText(hDlg, IDD_G4,str[3][6]);

 	sprintf(str[4][0], "%.5f",map[4][0]);
	sprintf(str[4][1], "%.5f",map[4][1]);
	sprintf(str[4][2], "%.5f",map[4][2]);
	sprintf(str[4][3], "%.5f",map[4][3]);
	sprintf(str[4][4], "%.5f",map[4][4]);
	sprintf(str[4][5], "%.5f",map[4][5]);
	sprintf(str[4][6], "%.5f",map[4][6]);

	SetDlgItemText(hDlg, IDD_A5,str[4][0]);
	SetDlgItemText(hDlg, IDD_B5,str[4][1]);
	SetDlgItemText(hDlg, IDD_C5,str[4][2]);
	SetDlgItemText(hDlg, IDD_D5,str[4][3]);
	SetDlgItemText(hDlg, IDD_E5,str[4][4]);
	SetDlgItemText(hDlg, IDD_F5,str[4][5]);
	SetDlgItemText(hDlg, IDD_G5,str[4][6]);

	sprintf(str_fy, "%.1f",fy);
	sprintf(str_fx, "%.1f",fx);
	SetDlgItemText(hDlg, IDD_FX,str_fx);
	SetDlgItemText(hDlg, IDD_FY,str_fy);

}


void getParameters(HWND hDlg)
{
	GetDlgItemText(hDlg, IDD_A1,str[0][0],8);
	GetDlgItemText(hDlg, IDD_B1,str[0][1],8);
	GetDlgItemText(hDlg, IDD_C1,str[0][2],8);
	GetDlgItemText(hDlg, IDD_D1,str[0][3],8);
	GetDlgItemText(hDlg, IDD_E1,str[0][4],8);
	GetDlgItemText(hDlg, IDD_F1,str[0][5],8);
	GetDlgItemText(hDlg, IDD_G1,str[0][6],8);

	map[0][0] = atof(str[0][0]);
	map[0][1] = atof(str[0][1]);
	map[0][2] = atof(str[0][2]);
	map[0][3] = atof(str[0][3]);
	map[0][4] = atof(str[0][4]);
	map[0][5] = atof(str[0][5]);
	map[0][6] = atof(str[0][6]);

	GetDlgItemText(hDlg, IDD_A2,str[1][0],8);
	GetDlgItemText(hDlg, IDD_B2,str[1][1],8);
	GetDlgItemText(hDlg, IDD_C2,str[1][2],8);
	GetDlgItemText(hDlg, IDD_D2,str[1][3],8);
	GetDlgItemText(hDlg, IDD_E2,str[1][4],8);
	GetDlgItemText(hDlg, IDD_F2,str[1][5],8);
	GetDlgItemText(hDlg, IDD_G2,str[1][6],8);

	map[1][0] = atof(str[1][0]);
	map[1][1] = atof(str[1][1]);
	map[1][2] = atof(str[1][2]);
	map[1][3] = atof(str[1][3]);
	map[1][4] = atof(str[1][4]);
	map[1][5] = atof(str[1][5]);
	map[1][6] = atof(str[1][6]);

	GetDlgItemText(hDlg, IDD_A3,str[2][0],8);
	GetDlgItemText(hDlg, IDD_B3,str[2][1],8);
	GetDlgItemText(hDlg, IDD_C3,str[2][2],8);
	GetDlgItemText(hDlg, IDD_D3,str[2][3],8);
	GetDlgItemText(hDlg, IDD_E3,str[2][4],8);
	GetDlgItemText(hDlg, IDD_F3,str[2][5],8);
	GetDlgItemText(hDlg, IDD_G3,str[2][6],8);

	map[2][0] = atof(str[2][0]);
	map[2][1] = atof(str[2][1]);
	map[2][2] = atof(str[2][2]);
	map[2][3] = atof(str[2][3]);
	map[2][4] = atof(str[2][4]);
	map[2][5] = atof(str[2][5]);
	map[2][6] = atof(str[2][6]);

	GetDlgItemText(hDlg, IDD_A4,str[3][0],8);
	GetDlgItemText(hDlg, IDD_B4,str[3][1],8);
	GetDlgItemText(hDlg, IDD_C4,str[3][2],8);
	GetDlgItemText(hDlg, IDD_D4,str[3][3],8);
	GetDlgItemText(hDlg, IDD_E4,str[3][4],8);
	GetDlgItemText(hDlg, IDD_F4,str[3][5],8);
	GetDlgItemText(hDlg, IDD_G4,str[3][6],8);

	map[3][0] = atof(str[3][0]);
	map[3][1] = atof(str[3][1]);
	map[3][2] = atof(str[3][2]);
	map[3][3] = atof(str[3][3]);
	map[3][4] = atof(str[3][4]);
	map[3][5] = atof(str[3][5]);
	map[3][6] = atof(str[3][6]);

	GetDlgItemText(hDlg, IDD_A5,str[4][0],8);
	GetDlgItemText(hDlg, IDD_B5,str[4][1],8);
	GetDlgItemText(hDlg, IDD_C5,str[4][2],8);
	GetDlgItemText(hDlg, IDD_D5,str[4][3],8);
	GetDlgItemText(hDlg, IDD_E5,str[4][4],8);
	GetDlgItemText(hDlg, IDD_F5,str[4][5],8);
	GetDlgItemText(hDlg, IDD_G5,str[4][6],8);

	map[4][0] = atof(str[4][0]);
	map[4][1] = atof(str[4][1]);
	map[4][2] = atof(str[4][2]);
	map[4][3] = atof(str[4][3]);
	map[4][4] = atof(str[4][4]);
	map[4][5] = atof(str[4][5]);
	map[4][6] = atof(str[4][6]);

	GetDlgItemText(hDlg, IDD_FX,str_fx,5);
	GetDlgItemText(hDlg, IDD_FY,str_fy,5);
	fx = atof(str_fx);
	fy = atof(str_fy);

}


void setParameters_Fern()
{
	map[0][0] = 0.0;	map[0][1] = 0.0;	map[0][2] = 0.0;	map[0][3]=0.16;		map[0][4]= 0.0;		map[0][5]=0.0;	map[0][6]=0.01;
	map[1][0] = 0.2;	map[1][1] = -0.26;	map[1][2] = 0.23;	map[1][3]=0.22;		map[1][4]= 0.0;		map[1][5]=1.6;	map[1][6]=0.07;
	map[2][0] = -0.15;	map[2][1] = 0.28;	map[2][2] = 0.26;	map[2][3]=0.24;		map[2][4]= 0.0;		map[2][5]=0.44;	map[2][6]=0.07;
	map[3][0] = 0.75;	map[3][1] = 0.04;	map[3][2] = -0.04;	map[3][3]=0.85;		map[3][4]= 0.0;		map[3][5]=1.6;	map[3][6]=0.85;
	map[4][0] = 0.0;	map[4][1] = 0.0;	map[4][2] = 0.0;	map[4][3]=0.0;		map[4][4]= 0.0;		map[4][5]=0.0;	map[4][6]=0.0;

	fx = -0.4; //-0.4
	fy = -0.2; //-0.2
	fcx = 0.5;
	fcy = 1.0;
	//centerx = int(sw * 0.5);
	//centery = sh;
}

void setParameters_Dragon()
{
	map[0][0] = 0.824074;	map[0][1] = 0.281428;	map[0][2] = -0.212346;	map[0][3]=0.864198;		map[0][4]= -1.882290;		map[0][5]=-0.110607;	map[0][6]=0.8;
	map[1][0] = 0.088272;	map[1][1] = 0.520988;	map[1][2] = -0.463889;	map[1][3]=-0.377778;	map[1][4]= 0.785360;		map[1][5]=8.095795;		map[1][6]=0.2;
	map[2][0] = 0.0;	map[2][1] = 0.0;	map[2][2] = 0.0;	map[2][3]=0.0;		map[2][4]= 0.0;		map[2][5]=0.0;	map[2][6]=0.0;
	map[3][0] = 0.0;	map[3][1] = 0.0;	map[3][2] = 0.0;	map[3][3]=0.0;		map[3][4]= 0.0;		map[3][5]=0.0;	map[3][6]=0.0;
	map[4][0] = 0.0;	map[4][1] = 0.0;	map[4][2] = 0.0;	map[4][3]=0.0;		map[4][4]= 0.0;		map[4][5]=0.0;	map[4][6]=0.0;

	fx = -0.1;
	fy = -0.2; //-0.2
	fcx = 0.5;
	fcy = 0.5;
	fcy = 1.0;
	//centerx = int(sw * 0.5);
	//centery = int(sh * 1.0);
}
void setParameters_Sierpinsky()
{
	map[0][0] = 0.5;	map[0][1] = 0.0;	map[0][2] = 0.0;	map[0][3]=0.5;		map[0][4]= -0.5;	map[0][5]=0.5;	map[0][6]=0.333;
	map[1][0] = 0.5;	map[1][1] = 0.0;	map[1][2] = 0.0;	map[1][3]=0.5;		map[1][4]= -0.5;	map[1][5]=-0.5;	map[1][6]=0.333;
	map[2][0] = 0.5;	map[2][1] = 0.0;	map[2][2] = 0.0;	map[2][3]=0.5;		map[2][4]= 0.5;		map[2][5]=-0.5;	map[2][6]=0.334;
	map[3][0] = 0.0;	map[3][1] = 0.0;	map[3][2] = 0.0;	map[3][3]=0.0;		map[3][4]= 0.0;		map[3][5]=0.0;	map[3][6]=0.0;
	map[4][0] = 0.0;	map[4][1] = 0.0;	map[4][2] = 0.0;	map[4][3]=0.0;		map[4][4]= 0.0;		map[4][5]=0.0;	map[4][6]=0.0;

	fx = 1.0;
	fy = 1.0;
	fcx = 0.5;
	fcy = 0.5;
	//centerx = int(sw * 0.5);
	//centery = int(sh * 0.5);
}

void setParameters_Spiral()
{
	map[0][0] = 0.787878;	map[0][1] = -0.424242;	map[0][2] = 0.242424;	map[0][3]=0.859848;		map[0][4]= 1.758647;	map[0][5]=1.408065;	map[0][6]=0.9;
	map[1][0] = -0.121212;	map[1][1] = 0.257576;	map[1][2] = 0.151515;	map[1][3]=0.053030;		map[1][4]= -6.721654;	map[1][5]=1.377236;	map[1][6]=0.05;
	map[2][0] = 0.181818;	map[2][1] = -0.136364;	map[2][2] = 0.090909;	map[2][3]=0.181818;		map[2][4]= 6.086107;	map[2][5]=1.568035;	map[2][6]=0.05;
	map[3][0] = 0.0;	map[3][1] = 0.0;	map[3][2] = 0.0;	map[3][3]=0.0;		map[3][4]= 0.0;		map[3][5]=0.0;	map[3][6]=0.0;
	map[4][0] = 0.0;	map[4][1] = 0.0;	map[4][2] = 0.0;	map[4][3]=0.0;		map[4][4]= 0.0;		map[4][5]=0.0;	map[4][6]=0.0;

	fx = 0.1;
	fy = 0.2;
	fcx = 0.5;
	fcy = 0.0;
	//centerx = int(sw * 0.5);
	//centery = int(sh * 0.0);
}

void setParameters_Leaf()
{
	map[0][0] = 0.14;	map[0][1] = 0.01;	map[0][2] = 0.0;	map[0][3]=0.51;		map[0][4]= -0.08;	map[0][5]=-1.31;	map[0][6]=0.25;
	map[1][0] = 0.43;	map[1][1] = 0.52;	map[1][2] = -0.45;	map[1][3]=0.50;		map[1][4]= 1.49;	map[1][5]=-0.75;	map[1][6]=0.25;
	map[2][0] = 0.45;	map[2][1] = -0.49;	map[2][2] = 0.47;	map[2][3]=0.47;		map[2][4]= -1.62;	map[2][5]=-0.74;	map[2][6]=0.25;
	map[3][0] = 0.49;	map[3][1] = 0.0;	map[3][2] = 0.0;	map[3][3]=0.51;		map[3][4]= 0.02;	map[3][5]=1.62;		map[3][6]=0.25;
	map[4][0] = 0.0;	map[4][1] = 0.0;	map[4][2] = 0.0;	map[4][3]=0.0;		map[4][4]= 0.0;		map[4][5]=0.0;		map[4][6]=0.0;

	fx = 1.0;
	fy = 1.0;
	fcx = 0.5;
	fcy = 0.0;
	//centerx = int(sw * 0.5);
	//centery = int(sh * 0.0);
}

void setIFSOrigine(float fcx, float fcy){
	centerx = int(sw * fcx);
	centery = int(sh * fcy);
}

int random100()
{
	return int(random()*1000);
}
void draw(HWND hWnd,HWND hWndToolbox)
{
	DibSection dibsection;
	dibsection.Size(sw,sh);
    long i;

    double u = 0.0, v = 0.0, newu, newv, sum = 0.0, rnd;
    int l = 0;
	COLORREF colore;

	int orig_x = (int)(sw * 0.5);
	int orig_y = (int)(sh * 0.5);

    for (i = 1; i <= dots; i++) {
      rnd = random();
      l = 0; sum = map[l][6];
      while ( (rnd > sum) && (l < LINES) ) {
        l++;
        sum += map[l][6];
      }
	  if(colore_ON)
	  switch(l){
		  case 0:
			  colore = RGB(255,0,0);
			  break;
		  case 1:
			  colore = RGB(0,255,0);
			  break;
		  case 2:
			  colore = RGB(0,0,255);
			  break;
		  case 3:
			  colore = RGB(255,255,0);
			  break;
		  case 4:
			  colore = RGB(0,255,255);
			  break;
		  default:
			  colore = RGB(255,255,255);
			  break;
	  }
	  else
		  colore = RGB(255,255,255);
      if (l < LINES) {
        newu = map[l][0] * u + map[l][1] * v;
        newv = map[l][2] * u + map[l][3] * v;
        u = newu + map[l][4];
        v = newv + map[l][5];
      }
		int drawx = (int)(u*orig_x*fx);
		int drawy = (int)(v*orig_y*fy);

		//SetPixel(hdc, drawx+centerx, drawy+centery,colore);
		//BITMAP VALUES MUST BE BETWEEN 0 and SH/SW!!!!
		if(drawx+centerx>0 && drawy+centery >0)
		if(drawx+centerx< sw && drawy+centery < sh)
			dibsection.PixelAt(drawx+centerx,drawy+centery,colore);
	}
	drawParameters(hWndToolbox);
	PaintDc pdc(hWnd);
	dibsection.BitBltTo(pdc,0,0);
}

void OnPaint(HWND hWnd,HWND hWndToolbox)
{
		RECT rctMainWnd;
		GetClientRect(hWnd, &rctMainWnd);
		sw = rctMainWnd.right - rctMainWnd.left;
		sh = rctMainWnd.bottom - rctMainWnd.top;
		setIFSOrigine(fcx,fcy);
		draw(hWnd,hWndToolbox);

}

#endif
