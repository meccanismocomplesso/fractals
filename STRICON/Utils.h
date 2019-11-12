#ifndef H_UTILS
#define H_UTILS

/* September 2002: Craig Wilkie <craig.wilkie@bigfoot.com>
               General Utilities for Windows Screensavers */

//#define M_PI        3.14159
#define LRAND()     rand()
#define NRAND(n)    (rand() % n)
#define MIN(x, y)   ((x < y)? x : y)
#define MAX(x, y)   ((x > y)? x : y)

COLORREF *colSpectrum(long ncolours);
COLORREF *colSpectrumCoherent(long ncolours);
COLORREF *create_random_colorrefs(long count);
COLORREF get_random_color(void);
COLORREF get_random_bright_color(void);
void get_window_sizes(HWND window, long *width, long *height);
long check_message_queue(HWND window);
float random();
int random100();

#endif


