
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <scrnsave.h>
#include <time.h>
#include "ico_res.h"

#define TIMER 1

long delay = 5;
long count = 3;
long grid_size = 2;
long colours = 256;
long speed = 20;
long shift = 60;
long radius = 800;

int sh = 600;
int sw = 600;

COLORREF *colour_array;

struct inter_source {
  int x; 
  int y;
  double x_theta;
  double y_theta;
};

struct inter_context {

  HDC dpy;
  HDC mem_dc;
  HWND   win;
  HBITMAP bitmap;
  unsigned char *bitmap_data;
  long bits_per_pixel;

  int count;
  int grid_size;
  int colors;
  int speed;
  int delay;
  int shift;
  int radius;

  int w;
  int h;
  COLORREF *pal;
  int* wave_height;
  struct inter_source* source;
};

struct inter_context c;

void inter_init(HDC dpy, HWND win, struct inter_context* c) 
{
  double H[3], S[3], V[3];
  int i;
  unsigned long valmask = 0;

  memset (c, 0, sizeof(*c));

  c->dpy = dpy;
  c->win = win;

  RECT client;
  GetClientRect(win, &client);
  c->w = client.right - client.left;
  c->h = client.bottom - client.top;

  c->mem_dc = CreateCompatibleDC(c->dpy);
  c->bits_per_pixel = GetDeviceCaps(c->dpy, BITSPIXEL);
  
  /* Allocate enough space for the worst case of 32 bits per pixel */
  c->bitmap_data = (unsigned char *)malloc(c->w * c->h * 4);

  c->count = count;
  c->grid_size = grid_size;
  c->colors = colours;
  c->speed = speed;
  c->shift = shift;
  c->radius = radius;

  c->pal = (COLORREF *)calloc(c->colors, sizeof(COLORREF));

      H[0] = (float)(rand() % 3600) / 10.0; 
      H[1] = H[0] + c->shift < 360.0 ? H[0]+c->shift : H[0] + c->shift-360.0; 
      H[2] = H[0];
      S[0] = S[1] = S[2] = 1.0;
      V[0] = V[1] = V[2] = 1.0;

    long loop;

    colour_array = (COLORREF *)malloc(colours * sizeof(COLORREF));

    for (loop = 0; loop < c->colors; loop++)
    {
        double h, s, v;
        long r, g, b;

        if (loop < c->colors / 2)
        {
            if (H[1] > H[0])
            {	h = ((H[1] - H[0]) / (c->colors / 2) * loop) + H[0];
            }else{
                	h = ((H[1] - H[0] + 360.0) / (c->colors / 2) * loop) + H[0];
            }
            s = ((S[1] - S[0]) / (c->colors / 2) * loop) + S[0];
            v = ((V[1] - V[0]) / (c->colors / 2) * loop) + V[0];
        }else{
            if (H[2] > H[1])
            {
                h = ((H[2] - H[1]) / (c->colors / 2) * (loop - (c->colors / 2))) + H[1];
            }else{
                h = ((H[2] - H[1] + 360.0) / (c->colors / 2) * (loop - (c->colors / 2))) + H[1];
            }
            s = ((S[2] - S[1]) / (c->colors / 2) * (loop - (c->colors / 2))) + S[1];
            v = ((V[2] - V[1]) / (c->colors / 2) * (loop - (c->colors / 2))) + V[1];
        }
        while (h > 360.0) h -= 360.0;
        while (h < 0.0)   h += 360.0;

        if (h < 60.0){
            r = 255;
            g = (long)(h * 255.0 / 60.0);
            b = 0;
        }else if (h < 120.0){
            r = (long)((60.0 - (h - 60.0)) * 255.0 / 60.0);
            g = 255;
            b = 0;
        }else if (h < 180.0){
            r = 0;
            g = 255;
            b = (long)((h - 120.0) * 255.0 / 60.0);
        }else if (h < 240.0){
            r = 0;
            g = (long)((60.0 - (h - 180.0)) * 255.0 / 60.0);
            b = 255;
        }else if (h < 300.0){
            r = (long)((h - 240.0) * 255.0 / 60.0);
            g = 0;
            b = 255;
        }else{
            r = 255;
            g = 0;
            b = (long)((60.0 - (h - 300.0)) * 255.0 / 60.0);
        }

        if ((c->bits_per_pixel == 32) || (c->bits_per_pixel == 24))
        {
            c->pal[loop] = (b << 16) + (g << 8) + r;
        }
        else if (c->bits_per_pixel == 16)
        {
            c->pal[loop] = ((b >> 3) << 11) + ((g >> 2) << 5) + (r >> 3);
        }

        colour_array[loop] = c->pal[loop];
    } 
  c->wave_height = (int *)calloc(c->radius, sizeof(int));
  for(i = 0; i < c->radius; i++) {
    float max = 
      ((float)c->colors) * 
      ((float)c->radius - (float)i) /
      ((float)c->radius);
    c->wave_height[i] = 
      (int)
      ((max + max*cos((double)i/50.0)) / 2.0);
  }
  c->source = (struct inter_source *)calloc(c->count, sizeof(struct inter_source));
  for(i = 0; i < c->count; i++) {
    c->source[i].x_theta = (float)(rand() % 1000) / 500.0 * 3.14159;
    c->source[i].y_theta = (float)(rand() % 1000) / 500.0 * 3.14159;
  }
}

#define source_x(c, i) \
  (c->w * 0.5 + ((int)(cos(c->source[i].x_theta)*((float)c->w * 0.5))))
#define source_y(c, i) \
  (c->h * 0.5 + ((int)(cos(c->source[i].y_theta)*((float)c->h * 0.5))))

/*
 * this is rather suboptimal. the sqrt() doesn't seem to be a big
 * performance hit, but all those separate XFillRectangle()'s are.
 * hell, it's just a quick hack anyway -- if someone wishes to tune
 * it, go ahead! 
 */

void do_inter(struct inter_context* c) 
{
    int i, j, k;
    int result;
    int dist;
    int g;
    int dx, dy;

    for(i = 0; i < c->count; i++) 
    {
        c->source[i].x_theta += (c->speed * 0.001);
        if(c->source[i].x_theta > 6.28318)
        {
            c->source[i].x_theta -= 6.28318;
        }
        c->source[i].y_theta += (c->speed * 0.001);
        if(c->source[i].y_theta > 6.28318)
        {
            c->source[i].y_theta -= 6.28318;
        }
        c->source[i].x = (int)(source_x(c, i));
        c->source[i].y = (int)(source_y(c, i));
    }

    g = c->grid_size;

    for(j = 0; j < c->h/g; j++) 
    {
        for(i = 0; i < c->w/g; i++) 
        {
            result = 0;
            for(k = 0; k < c->count; k++) 
            {
                dx = i*g + (g >> 1) - c->source[k].x;
                dy = j*g + (g >> 1) - c->source[k].y;
                dist = (int)sqrt(dx*dx + dy*dy); 
                result += (dist >= c->radius ? 0 : c->wave_height[dist]);
            }
            result %= c->colors;
            if ((c->bits_per_pixel == 32)||(c->bits_per_pixel == 24))
            {
                // Fill in these `gridsize' horizontal bits in the scanline 
                for(k = 0; k < g; k++)
                {
                    ((unsigned long *)(c->bitmap_data))[g*(i+(j*c->w))+k] = colour_array[result];
                }
            }
            else if (c->bits_per_pixel == 16)
            {
                // Fill in these `gridsize' horizontal bits in the scanline
                for(k = 0; k < g; k++)
                {
                    ((unsigned short *)(c->bitmap_data))[g*(i+(j*c->w))+k] = (unsigned short)colour_array[result];
                }
            }
        }
        // Only the first scanline of the image has been filled in; clone that
        //    scanline to the rest of the `gridsize' lines in the ximage 
        
	  for(k = 1; k < g; k++)
        {
            if ((c->bits_per_pixel == 32)||(c->bits_per_pixel == 24))
            {
                memcpy(((unsigned long *)(c->bitmap_data)) + (c->w * ((g*j)+k)),
                        ((unsigned long *)(c->bitmap_data)) + (c->w * (g*j)),
                        c->w * 4);
            }
            else if (c->bits_per_pixel == 16)
            {
                memcpy(((unsigned short *)(c->bitmap_data)) + (c->w * ((g*j)+k)),
                        ((unsigned short *)(c->bitmap_data)) + (c->w * (g*j)),
                        c->w * 2);
            }
        }
    }

  c->bitmap = CreateBitmap(c->w, c->h, 1, c->bits_per_pixel, c->bitmap_data);
  HBITMAP old_bmp;
  if ((old_bmp = (HBITMAP)SelectObject(c->mem_dc, c->bitmap)) == NULL)
  {
      MessageBox(c->win, "Error selecting bitmap", "Error", MB_OK);
  }
  BitBlt(c->dpy, 0, 0, c->w, c->h, c->mem_dc, 0, 0, SRCCOPY);
  SelectObject(c->mem_dc, old_bmp);
  DeleteObject(c->bitmap);
}


/* *************** MAIN WINDOW ************************* */
LRESULT WINAPI MainWndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	static RECT rect;
	switch(nMsg)
	{
		case WM_CREATE:
    			inter_init(GetDC(hwnd), hwnd, &c);
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
    			do_inter(&c);	
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
                "Interferences - The Blue Ant",
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

