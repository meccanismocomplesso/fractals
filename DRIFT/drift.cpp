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
long count;

OPTION options[] =
{
/*	 Type            variable     control              default minimum maximum name */
    {LONG_EDIT_TYPE, &delay,      IDC_DELAY_EDIT,		200,    1,      10000,    "Delay"},
    {LONG_EDIT_TYPE, &ncolours,   IDC_COLOURS_EDIT,		256,	1,		256,    "Colours"},
	{LONG_EDIT_TYPE, &count,	  IDC_COUNT_EDIT,		500,	1,		5000,	"Count"},
	{-1, NULL, -1, -1, -1, -1, ""}
};

OPTIONS_STRUCT screenhack_options = 
{
    "Drift", options
};

int sw; 
int sh; 
bool done;

COLORREF* colore;

#define MAXBATCH1	200	/* mono */
#define MAXBATCH2	20	/* color */
#define FUSE		10	/* discard this many initial iterations */
#define NMAJORVARS	7
#define MAXLEV 10
#define DISTRIB_A (halfrandom(dp, 7000) + 9000)
#define DISTRIB_B ((frandom(dp, 3) + 1) * (frandom(dp, 3) + 1) * 120000)
#define LEN(x) (sizeof(x)/sizeof((x)[0]))
#define VARIATION_LEN 14
#define XFORM_LEN 9

typedef struct {
	/* shape of current flame */
	int         nxforms;
	double      f[2][3][MAXLEV];	/* a bunch of non-homogeneous xforms */
	int         variation[10];	/* for each xform */

	/* Animation */
	double      df[2][3][MAXLEV];

	/* high-level control */
	int         mode;	/* 0->slow/single 1->fast/many */
	int         nfractals;	/* draw this many fractals */
	int         major_variation;
	int         fractal_len;	/* pts/fractal */
	int         color;
	int         rainbow;	/* more than one color per fractal 1-> computed by adding dimension to fractal */
	int         width, height;	/* of window */
	int         timer;

	/* draw info about current flame */
	int         fuse;	/* iterate this many before drawing */
	int         total_points;	/* draw this many pts before fractal ends */
	int         npoints;	/* how many we've computed but not drawn */
	
	//XPoint      pts[MAXBATCH1];	/* here they are */
	POINT		pts[MAXBATCH1];

	//unsigned long pixcol; /* when drawing in color, we have a buffer per color */
	int			pixcol;  // created by me
	int         *ncpoints;
	//XPoint     *cpts;
	POINT		*cpts;

	double      x, y, c;
	int         liss_time;
	bool        grow, liss;

	short       lasthalf;
	long        saved_random_bits;
	int         nbits;
} driftstruct;

//static driftstruct *drifts = (driftstruct *) NULL;
driftstruct drifts;


static short halfrandom(driftstruct * dp, int mv)
{
	unsigned long r;

	if (dp->lasthalf) {
		r = dp->lasthalf;
		dp->lasthalf = 0;
	} else {
		r = LRAND();
		dp->lasthalf = (short) (r >> 16);
	}
	r = r % mv;
	return r;
}

static int frandom(driftstruct * dp, int n)
{
	int         result;

	if (3 > dp->nbits) {
		dp->saved_random_bits = LRAND();
		dp->nbits = 31;
	}
	switch (n) {
		case 2:
			result = (int) (dp->saved_random_bits & 1);
			dp->saved_random_bits >>= 1;
			dp->nbits -= 1;
			return result;

		case 3:
			result = (int) (dp->saved_random_bits & 3);
			dp->saved_random_bits >>= 2;
			dp->nbits -= 2;
			if (3 == result)
				return frandom(dp, 3);
			return result;

		case 4:
			result = (int) (dp->saved_random_bits & 3);
			dp->saved_random_bits >>= 2;
			dp->nbits -= 2;
			return result;

		case 5:
			result = (int) (dp->saved_random_bits & 7);
			dp->saved_random_bits >>= 3;
			dp->nbits -= 3;
			if (4 < result)
				return frandom(dp, 5);
			return result;
		default:
			(void) fprintf(stderr, "bad arg to frandom\n");
	}
	return 0;
}


static void initmode(int mode, driftstruct* dp)
{
	dp->mode = mode;
	dp->major_variation = ((dp->major_variation >= VARIATION_LEN >> 1) &&
			       (dp->major_variation < VARIATION_LEN - 1)) ?
		(dp->major_variation + 1) >> 1 : dp->major_variation >> 1;

	if (dp->grow) {
		dp->rainbow = 0;
		if (mode) {
			if (!dp->color || halfrandom(dp, 8)) {
				dp->nfractals = halfrandom(dp, 30) + 5;
				dp->fractal_len = DISTRIB_A;
			} else {
				dp->nfractals = halfrandom(dp, 5) + 5;
				dp->fractal_len = DISTRIB_B;
			}
		} else {
			dp->rainbow = dp->color;
			dp->nfractals = 1;
			dp->fractal_len = DISTRIB_B;
		}
	} else {
		dp->nfractals = 1;
		dp->rainbow = dp->color;
		dp->fractal_len = 2000000;
	}
	dp->fractal_len = (dp->fractal_len * count) / 20;
}

static void pick_df_coefs(driftstruct* dp)
{
	//driftstruct *dp = &drifts[0];
	int         i, j, k;
	double      r;

	for (i = 0; i < dp->nxforms; i++) {
		r = 1e-6;
		for (j = 0; j < 2; j++)
			for (k = 0; k < 3; k++) {
				dp->df[j][k][i] = ((double) halfrandom(dp, 1000) / 500.0 - 1.0);
				r += dp->df[j][k][i] * dp->df[j][k][i];
			}
		r = (3 + halfrandom(dp, 5)) * 0.01 / sqrt(r);
		for (j = 0; j < 2; j++)
			for (k = 0; k < 3; k++)
				dp->df[j][k][i] *= r;
	}
}

static void free_drift(driftstruct *dp)
{
	if (dp->ncpoints != NULL) {
		(void) free((void *) dp->ncpoints);
		dp->ncpoints = (int *) NULL;
	}
	if (dp->cpts != NULL) {
		(void) free((void *) dp->cpts);
		dp->cpts = (POINT *) NULL;
	}
}

static void initfractal(driftstruct* dp)
{
	//driftstruct *dp = &drifts[0];
	int         i, j, k;

	dp->fuse = FUSE;
	dp->total_points = 0;

	if (!dp->ncpoints) {
		if ((dp->ncpoints = (int *) malloc(sizeof (int) * ncolours)) == NULL) {
			free_drift(dp);
			return;
		}
	}
	if (!dp->cpts) {
		if ((dp->cpts = (POINT *) malloc(MAXBATCH2 * sizeof (POINT) * ncolours)) == NULL) {
			free_drift(dp);
			return;
		}
	}
	if (dp->rainbow)
		for (i = 0; i < ncolours; i++)
			dp->ncpoints[i] = 0;
	else
		dp->npoints = 0;

	dp->nxforms = (dp->nxforms >= XFORM_LEN - 1) + dp->nxforms / 3 + 2;

	dp->c = dp->x = dp->y = 0.0;
	if (dp->liss && !halfrandom(dp, 10)) {
		dp->liss_time = 0;
	}
	if (!dp->grow)
		pick_df_coefs(dp);
	for (i = 0; i < dp->nxforms; i++) {
		if (NMAJORVARS == dp->major_variation)
			dp->variation[i] = halfrandom(dp, NMAJORVARS);
		else
			dp->variation[i] = dp->major_variation;
		for (j = 0; j < 2; j++)
			for (k = 0; k < 3; k++) {
				if (dp->liss)
					dp->f[j][k][i] = sin(dp->liss_time * dp->df[j][k][i]);
				else
					dp->f[j][k][i] = ((double) halfrandom(dp, 1000) / 500.0 - 1.0);
			}
	}
	dp->pixcol = halfrandom(dp, ncolours);

}


void init_drift(driftstruct* dp)
{
	//driftstruct *dp;
	if (dp == NULL) {
		if ((dp = (driftstruct *) calloc(1, sizeof (driftstruct))) == NULL)
			return;
	}
	//dp = &drifts[0];
	dp->width = sw;
	dp->height = sh;
	dp->color = ncolours > 2; 

	if (NRAND(3) == 0)
		dp->grow = true;
	else {
		dp->grow = false;
		dp->liss = (bool) (LRAND() & 1);
	}
	initmode(1,dp);
	initfractal(dp);
}

static void iter(driftstruct * dp)
{
	int         i = frandom(dp, dp->nxforms);
	double      nx, ny, nc;

	if (i)
		nc = (dp->c + 1.0) / 2.0;
	else
		nc = dp->c / 2.0;

	nx = dp->f[0][0][i] * dp->x + dp->f[0][1][i] * dp->y + dp->f[0][2][i];
	ny = dp->f[1][0][i] * dp->x + dp->f[1][1][i] * dp->y + dp->f[1][2][i];

	switch (dp->variation[i]) {
		case 1:
			/* sinusoidal */
			nx = sin(nx);
			ny = sin(ny);
			break;
		case 2:
			{
				/* complex */
				double      r2 = nx * nx + ny * ny + 1e-6;

				nx = nx / r2;
				ny = ny / r2;
				break;
			}
		case 3:
			/* bent */
			if (nx < 0.0)
				nx = nx * 2.0;
			if (ny < 0.0)
				ny = ny / 2.0;
			break;
		case 4:
			{
				/* swirl */

				double      r = (nx * nx + ny * ny);	/* times k here is fun */
				double      c1 = sin(r);
				double      c2 = cos(r);
				double      t = nx;

				if (nx > 1e4 || nx < -1e4 || ny > 1e4 || ny < -1e4)
					ny = 1e4;
				else
					ny = c2 * t + c1 * ny;
				nx = c1 * nx - c2 * ny;
				break;
			}
		case 5:
			{
				/* horseshoe */
				double      r, c1, c2, t;

				/* Avoid atan2: DOMAIN error message */
				if (nx == 0.0 && ny == 0.0)
					r = 0.0;
				else
					r = atan2(nx, ny);	/* times k here is fun */
				c1 = sin(r);
				c2 = cos(r);
				t = nx;

				nx = c1 * nx - c2 * ny;
				ny = c2 * t + c1 * ny;
				break;
			}
		case 6:
			{
				/* drape */
				double      t;

				/* Avoid atan2: DOMAIN error message */
				if (nx == 0.0 && ny == 0.0)
					t = 0.0;
				else
					t = atan2(nx, ny) / M_PI;

				if (nx > 1e4 || nx < -1e4 || ny > 1e4 || ny < -1e4)
					ny = 1e4;
				else
					ny = sqrt(nx * nx + ny * ny) - 1.0;
				nx = t;
				break;
			}
	}

#if 0
	/* here are some others */
	{
		/* broken */
		if (nx > 1.0)
			nx = nx - 1.0;
		if (nx < -1.0)
			nx = nx + 1.0;
		if (ny > 1.0)
			ny = ny - 1.0;
		if (ny < -1.0)
			ny = ny + 1.0;
		break;
	}
	{
		/* complex sine */
		double      u = nx, v = ny;
		double      ev = exp(v);
		double      emv = exp(-v);

		nx = (ev + emv) * sin(u) / 2.0;
		ny = (ev - emv) * cos(u) / 2.0;
	}
	{

		/* polynomial */
		if (nx < 0)
			nx = -nx * nx;
		else
			nx = nx * nx;

		if (ny < 0)
			ny = -ny * ny;
		else
			ny = ny * ny;
	}
	{
		/* spherical */
		double      r = 0.5 + sqrt(nx * nx + ny * ny + 1e-6);

		nx = nx / r;
		ny = ny / r;
	}
	{
		nx = atan(nx) / M_PI_2
			ny = atan(ny) / M_PI_2
	}
#endif

	/* how to check nan too?  some machines don't have finite().
	   don't need to check ny, it'll propogate */
	if (nx > 1e4 || nx < -1e4) {
		nx = halfrandom(dp, 1000) / 500.0 - 1.0;
		ny = halfrandom(dp, 1000) / 500.0 - 1.0;
		dp->fuse = FUSE;
	}
	dp->x = nx;
	dp->y = ny;
	dp->c = nc;

}

static void draw(HDC hdc, driftstruct * dp)
{
	double      x = dp->x;
	double      y = dp->y;
	int         fixed_x, fixed_y, npix, c, n;

	if (dp->fuse) {
		dp->fuse--;
		return;
	}
	if (!(x > -1.0 && x < 1.0 && y > -1.0 && y < 1.0))
		return;

	fixed_x = (int) ((dp->width / 2) * (x + 1.0));
	fixed_y = (int) ((dp->height / 2) * (y + 1.0));

	if (!dp->rainbow) {

		dp->pts[dp->npoints].x = fixed_x;
		dp->pts[dp->npoints].y = fixed_y;
		dp->npoints++;
		if (dp->npoints == MAXBATCH1) {		
			DrawPoints(hdc,dp->pts,dp->npoints,colore[dp->pixcol]);
			dp->npoints = 0;
		}
	} else {

		npix = ncolours;
		c = (int) (dp->c * npix);

		if (c < 0)
			c = 0;
		if (c >= npix)
			c = npix - 1;
		n = dp->ncpoints[c];
		dp->cpts[c * MAXBATCH2 + n].x = fixed_x;
		dp->cpts[c * MAXBATCH2 + n].y = fixed_y;
		if (++dp->ncpoints[c] == MAXBATCH2) {
			DrawPoints(hdc,&(dp->cpts[c * MAXBATCH2]),dp->ncpoints[c],colore[dp->pixcol]);
			dp->ncpoints[c] = 0;
		}
	}

}

static void draw_flush(HDC hdc, driftstruct * dp)
{
	if (dp->rainbow) {
		for (int i = 0; i < ncolours; i++) {
			if (dp->ncpoints[i]) {
				DrawPoints(hdc,&(dp->cpts[i * MAXBATCH2]),dp->ncpoints[i],colore[dp->pixcol]);
				dp->ncpoints[i] = 0;
			}
		}
	} else {
		if (dp->npoints)
			DrawPoints(hdc,dp->pts,dp->npoints,colore[dp->pixcol]);
		dp->npoints = 0;
	}
}


void draw_drift(HDC hdc, driftstruct* dp)
{
	
	if (dp == NULL)
		return;
	//dp = &drifts[MI_SCREEN(mi)];
	if (dp->ncpoints == NULL)
		return;

	dp->timer = 3000;
	
	while (dp->timer) {
		iter(dp);
		draw(hdc, dp);
		if (dp->total_points++ > dp->fractal_len) {
			draw_flush(hdc,dp);
			if (0 == --dp->nfractals) {
				initmode(frandom(dp, 2),dp);
			}
			initfractal(dp);
		}
		dp->timer--;
	}
	
	if (!dp->grow) {
		int         i, j, k;

		draw_flush(hdc,dp);
		if (dp->liss)
			dp->liss_time++;
		for (i = 0; i < dp->nxforms; i++)
			for (j = 0; j < 2; j++)
				for (k = 0; k < 3; k++) {
					if (dp->liss)
						dp->f[j][k][i] = sin(dp->liss_time * dp->df[j][k][i]);
					else {
						double      t = dp->f[j][k][i] += dp->df[j][k][i];

						if (t < -1.0 || 1.0 < t)
							dp->df[j][k][i] *= -1.0;
					}
				}
	}
	
}

void release_drift(driftstruct* dp)
{
	if (dp != NULL) {
		free_drift(dp);
		(void) free((void *) dp);
		dp = (driftstruct *) NULL;
	}
}

void screenhack_init (HDC hdc, HWND hwnd)
{
	RECT client;
	GetClientRect(hwnd, &client);
	sh = client.bottom;
	sw = client.right;
	
	colore = colSpectrum(ncolours);
	init_drift(&drifts);
	done = false;
	
	/* Set the screenhack timer */
    SetTimer(hwnd, 10, delay, NULL);
}

void screenhack_loop(HDC hdc, HWND hWnd)
{
    RECT client;
    GetClientRect(hWnd, &client);
	if(!done){
		FillRect(hdc, &client, (HBRUSH)GetStockObject(BLACK_BRUSH));
		done = true;
	}
	draw_drift(hdc,&drifts);
	
	//Sleep(2000);
	init_drift(&drifts);
	
}
