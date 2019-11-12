#ifndef H_UTILS
#define H_UTILS

/* September 2002: Craig Wilkie <craig.wilkie@bigfoot.com>
               General Utilities for Windows Screensavers */

//#define M_PI        3.14159
#define MAXRAND			(2147483648.0)
#define ABS(a) ((a)<0 ? -(a):(a))
#define COSF(n) ((float)cos((double)(n)))
#define SINF(n) ((float)sin((double)(n)))
#define FABSF(n) ((float)fabs((double)(n)))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define LRAND() ((long)(rand() & 0x7fffffff))
#define NRAND(n) ((int)(LRAND()%(n)))
#define INTRAND(min,max) (NRAND((max+1)-(min))+(min))
#define FLOATRAND(min,max) ((min)+((double) LRAND()/((double) MAXRAND))*((max)-(min)))
#define RAND(n) ((long) ((rand() & 0x7fffffff) % ((long) (n))))
#define RANDSIGN() ((rand() & 1) ? 1 : -1)
#define FRAND(f) ({ double tmp = ((((double) random()) * ((double) (f))) /		\
				((double) ((unsigned int)~0)));			\
				tmp < 0 ? (-tmp) : tmp; })

COLORREF *colSpectrum(long ncolours);
COLORREF *colSpectrumCoherent(long ncolours);
COLORREF *create_random_colorrefs(long count);
COLORREF get_random_color(void);
COLORREF get_random_bright_color(void);
void get_window_sizes(HWND window, long *width, long *height);
long check_message_queue(HWND window);
float random();
int random100();


void DrawPoints(HDC hdc, POINT* p, int npoints, COLORREF colore);



#endif


