#include <windows.h>
#include <scrnsave.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <time.h>
#include "resource.h"
#include "options.h"
#include "utils.h"
#include "spline.h"

long delay;
long ncolours;
long count;
int fg_index; //indice del colore

#define BLOB false
#define THICKNESS 0
#define ROTATION -1
#define CYCLE true

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
    "Starfish", options
};

int sw = 400; 
int sh = 400; 

COLORREF* colore;
#define SCALE        1000	/* fixed-point math, for sub-pixel motion */

enum starfish_mode {
  pulse,
  zoom
};
  
typedef struct {
  enum starfish_mode mode;
  bool blob_p;
  int skip;
  long x, y;		/* position of midpoint */
  double th;		/* angle of rotation */
  double rotv;		/* rotational velocity */
  double rota;		/* rotational acceleration */
  long elasticity;	/* how fast it deforms: radial velocity */
  double rot_max;
  long min_r, max_r;	/* radius range */
  int npoints;		/* control points */
  long *r;		/* radii */
  SPLINE *spline;
  POINT *prev;
  int n_prev;
} starfishtype;

static starfishtype sf;

static void free_starfish (starfishtype *s)
{
  if (s->r) free(s->r);
  if (s->prev) free(s->prev);
  if (s->spline)
    {
      if (s->spline->control_x) free(s->spline->control_x);
      if (s->spline->control_y) free(s->spline->control_y);
      if (s->spline->points) free(s->spline->points);
      free (s->spline);
    }
  free (s);
}


static void throb_starfish (starfishtype *s)
{
  int i;
  double frac = ((M_PI+M_PI) / s->npoints);

  for (i = 0; i < s->npoints; i++)
    {
      long r = s->r[i];
      long ra = (r > 0 ? r : -r);
      double th = (s->th > 0 ? s->th : -s->th);
      long x, y;
      long elasticity = s->elasticity;

      /* place control points evenly around perimiter, shifted by theta */
      x = s->x + long(ra * cos (i * frac + th));
      y = s->y + long(ra * sin (i * frac + th));

      s->spline->control_x[i] = x / SCALE;
      s->spline->control_y[i] = y / SCALE;

      if (s->mode == zoom && ((i % s->skip) == 0))
		continue;

      /* Slow down near the end points: move fastest in the middle. */
      {
		double ratio = (double)ra / (double)(s->max_r - s->min_r);
		if (ratio > 0.5) ratio = 1-ratio;	/* flip */
		ratio *= 2;							/* normalize */
		ratio = (ratio * 0.9) + 0.1;		/* fudge */
		elasticity *= long(ratio);
      }


      /* Increase/decrease radius by elasticity */
      ra += (r >= 0 ? elasticity : -elasticity);
      if ((i % s->skip) == 0)
		ra += (elasticity / 2);

      r = ra * (r >= 0 ? 1 : -1);

      /* If we've reached the end (too long or too short) reverse direction. */
      if ((ra > s->max_r && r >= 0) || (ra < s->min_r && r < 0))
			r = -r;
      s->r[i] = r;
    }
}


static void spin_starfish (starfishtype *s)
{
  double th = s->th;
  if (th < 0)
    th = -(th + s->rotv);
  else
    th += s->rotv;

  if (th > (M_PI+M_PI))
    th -= (M_PI+M_PI);
  else if (th < 0)
    th += (M_PI+M_PI);

  s->th = (s->th > 0 ? th : -th);

  s->rotv += s->rota;

  if (s->rotv > s->rot_max || 
      s->rotv < -s->rot_max)
    {
      s->rota = -s->rota;
    }
  /* If it stops, start it going in the other direction. */
  else if (s->rotv < 0)
    {
      if (rand() & 1)
	{
	  /* keep going in the same direction */
	  s->rotv = 0;
	  if (s->rota < 0)
	    s->rota = -s->rota;
	}
      else
	{
	  /* reverse gears */
	  s->rotv = -s->rotv;
	  s->rota = -s->rota;
	  s->th = -s->th;
	}
    }
  /* Alter direction of rotational acceleration randomly. */
  if (! (rand() % 120))
    s->rota = -s->rota;

  /* Change acceleration very occasionally. */
  if (! (rand() % 200))
    {
      if (rand() & 1)
	s->rota *= 1.2;
      else
	s->rota *= 0.8;
    }
}


static void draw_starfish (starfishtype *s)
{ 

  compute_closed_spline (s->spline);

  if (s->prev)
    {
      POINT *points = (POINT *) malloc (sizeof(POINT) * (s->n_prev + s->spline->n_points + 2));
      int i = s->spline->n_points;
      int j = s->n_prev;
      memcpy (points, s->spline->points, (i * sizeof(*points)));
      memcpy (points+i, s->prev, (j * sizeof(*points)));

	  if (s->blob_p){
		//XClearWindow (dpy, drawable);
	    RECT client;
		GetClientRect(hwnd, &client);
		FillRect(hdc, &client, (HBRUSH)GetStockObject(BLACK_BRUSH));
	  }
	  SelectObject(hdc,CreateSolidBrush(colore[fg_index]));
	  Polygon(hdc,points,i+j);
	  //XFillPolygon (dpy, drawable, gc, points, i+j, Complex, CoordModeOrigin);
      free (points);

      free (s->prev);
      s->prev = 0;
    }

  s->prev = (POINT *) malloc (s->spline->n_points * sizeof(POINT));
  memcpy (s->prev, s->spline->points, s->spline->n_points * sizeof(POINT));
  s->n_prev = s->spline->n_points;
  
}


void init_starfish (starfishtype *s)
{
	double sized;
	bool blob_p = BLOB;
	sized = (sw < sh ? sw : sh);
	if (blob_p) 
		sized /= 2;
	else sized *= 1.3;
	printf("INIT_SF() sw=%u sh=%u size=%u\n",sh,sw,(int)sized);

	int maxx = sw; 
	int maxy = sh; 
	int size = size;

    //s = (starfishtype *) calloc(1, sizeof(*s));
	int i;
	int mid;

	s->blob_p = BLOB;
	s->elasticity = SCALE * THICKNESS;

	if (s->elasticity == 0)
    /* bell curve from 0-15, avg 7.5 */
		s->elasticity = RAND(5*SCALE) + RAND(5*SCALE) + RAND(5*SCALE);

	s->rotv =ROTATION;
	if (s->rotv == -1)
    /* bell curve from 0-12 degrees, avg 6 */
		s->rotv = FRAND(4) + FRAND(4) + FRAND(4);

	s->rotv /= 360;  /* convert degrees to ratio */

	if (s->blob_p)
    {
      s->elasticity *= 3;
      s->rotv *= 3;
    }

	s->rot_max = s->rotv * 2;
	s->rota = 0.0004 + FRAND(0.0002);


	if (! (rand() % 20))
		size *= (int)(FRAND(0.35) + FRAND(0.35) + 0.3);

	{
		static char skips[] = { 2, 2, 2, 2,3, 3, 3,6, 6,12 };
		s->skip = skips[rand() % sizeof(skips)];
	}

	if (! (rand() % (s->skip == 2 ? 3 : 12)))
		s->mode = zoom;
	else
		s->mode = pulse;

	maxx *= SCALE;
	maxy *= SCALE;
	size *= SCALE;

	s->max_r = size;
	s->min_r = 0;

	if (s->min_r < (5*SCALE)) s->min_r = (5*SCALE);
	mid = ((s->min_r + s->max_r) / 2);

	s->x = maxx/2;
	s->y = maxy/2;

	s->th = FRAND(M_PI+M_PI) * RANDSIGN();
	printf("FRAND(M_PI+M_PI)= %f\n", FRAND(M_PI+M_PI));
	printf("RANDSIGN() = %d\n", RANDSIGN());

	{
		static char sizes[] = { 3, 3, 3, 3, 3,
			    4, 4, 4, 4,
			    5, 5, 5, 5, 5, 5,
			    8, 8, 8,
			    10,
			    35 };
		int nsizes = sizeof(sizes);
		if (s->skip > 3)
		nsizes -= 4;
		s->npoints = s->skip * sizes[rand() % nsizes];
	}
	printf("INIT_SF() s->npoints=%u s->skip=%u \n",s->npoints,s->skip);
 	printf("INIT_SF() s->x=%u s->y=%u \n",s->x,s->y);
 	printf("INIT_SF() s->elas=%u s->th=%f \n",s->elasticity,s->th);
	s->spline = make_spline (s->npoints);
	printf("INIT_SPLINE allocated_points %u\n",s->spline->allocated_points);
	printf("INIT_SPLINE n_points %u\n",s->spline->n_points);
	printf("INIT_SPLINE n_controls %u\n",s->spline->n_controls);

	s->r = (long *) malloc (sizeof(*s->r) * s->npoints);
	for (i = 0; i < s->npoints; i++)
		s->r[i] = ((i % s->skip) == 0) ? 0 : size;
  
}

static void run_starfish (starfishtype *s)
{
  throb_starfish (s);
  spin_starfish (s);
  draw_starfish (s);

  printf("FG INDEX %u\n",fg_index);
  printf("colours %u\n",ncolours);
  fg_index = (fg_index + 1) % ncolours;
  printf("FG INDEX %u\n",fg_index);

}

int main()
{
	srand(time(NULL));
	ncolours = 256;
	/*
	starfishtype sf;
	init_starfish(&sf);
	printf("POST INIT sf.npoints=%u sf.skip=%u \n",sf.npoints,sf.skip);
 	printf("POST INIT sf.x=%u sf.y=%u \n",sf.x,sf.y);
 	printf("POST INIT sf.elas=%u sf.th=%f \n",sf.elasticity,sf.th);
	printf("POST INIT allocated_points %u\n",sf.spline->allocated_points);
	printf("POST INIT n_points %u\n",sf.spline->n_points);
	printf("POST INIT n_controls %u\n",sf.spline->n_controls);
	run_starfish(&sf);
	*/
	starfishtype* sf;
	sf = (starfishtype *) calloc(1, sizeof(*sf));
	init_starfish(sf);
	printf("POST INIT sf.npoints=%u sf.skip=%u \n",sf->npoints,sf->skip);
 	printf("POST INIT sf.x=%u sf.y=%u \n",sf->x,sf->y);
 	printf("POST INIT sf.elas=%u sf.th=%f \n",sf->elasticity,sf->th);
	printf("POST INIT allocated_points %u\n",sf->spline->allocated_points);
	printf("POST INIT n_points %u\n",sf->spline->n_points);
	printf("POST INIT n_controls %u\n",sf->spline->n_controls);
	run_starfish(sf);
	
	return 0;
}
