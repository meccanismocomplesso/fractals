
#define DEF_MARTIN "False"
#define DEF_POPCORN "False"
#define DEF_EJK1 "False"
#define DEF_EJK2 "False"
#define DEF_EJK3 "False"
#define DEF_EJK4 "False"
#define DEF_EJK5 "False"
#define DEF_EJK6 "False"
#define DEF_RR "False"
#define DEF_JONG "False"
#define DEF_SINE "False"

static Bool martin;
static Bool popcorn;
static Bool ejk1;
static Bool ejk2;
static Bool ejk3;
static Bool ejk4;
static Bool ejk5;
static Bool ejk6;
static Bool rr;
static Bool jong;
static Bool sine;
/*

static XrmOptionDescRec opts[] =
{
	{"-martin", ".hop.martin", XrmoptionNoArg, "on"},
	{"+martin", ".hop.martin", XrmoptionNoArg, "off"},
	{"-popcorn", ".hop.popcorn", XrmoptionNoArg, "on"},
	{"+popcorn", ".hop.popcorn", XrmoptionNoArg, "off"},
	{"-ejk1", ".hop.ejk1", XrmoptionNoArg, "on"},
	{"+ejk1", ".hop.ejk1", XrmoptionNoArg, "off"},
	{"-ejk2", ".hop.ejk2", XrmoptionNoArg, "on"},
	{"+ejk2", ".hop.ejk2", XrmoptionNoArg, "off"},
	{"-ejk3", ".hop.ejk3", XrmoptionNoArg, "on"},
	{"+ejk3", ".hop.ejk3", XrmoptionNoArg, "off"},
	{"-ejk4", ".hop.ejk4", XrmoptionNoArg, "on"},
	{"+ejk4", ".hop.ejk4", XrmoptionNoArg, "off"},
	{"-ejk5", ".hop.ejk5", XrmoptionNoArg, "on"},
	{"+ejk5", ".hop.ejk5", XrmoptionNoArg, "off"},
	{"-ejk6", ".hop.ejk6", XrmoptionNoArg, "on"},
	{"+ejk6", ".hop.ejk6", XrmoptionNoArg, "off"},
	{"-rr", ".hop.rr", XrmoptionNoArg, "on"},
	{"+rr", ".hop.rr", XrmoptionNoArg, "off"},
	{"-jong", ".hop.jong", XrmoptionNoArg, "on"},
	{"+jong", ".hop.jong", XrmoptionNoArg, "off"},
	{"-sine", ".hop.sine", XrmoptionNoArg, "on"},
	{"+sine", ".hop.sine", XrmoptionNoArg, "off"}
};
static argtype vars[] =
{
	{&martin,  "martin",  "Martin",  DEF_MARTIN,  t_Bool},
	{&popcorn, "popcorn", "Popcorn", DEF_POPCORN, t_Bool},
	{&ejk1, "ejk1", "EJK1", DEF_EJK1, t_Bool},
	{&ejk2, "ejk2", "EJK2", DEF_EJK2, t_Bool},
	{&ejk3, "ejk3", "EJK3", DEF_EJK3, t_Bool},
	{&ejk4, "ejk4", "EJK4", DEF_EJK4, t_Bool},
	{&ejk5, "ejk5", "EJK5", DEF_EJK5, t_Bool},
	{&ejk6, "ejk6", "EJK6", DEF_EJK6, t_Bool},
	{&rr,   "rr",   "RR",   DEF_RR,   t_Bool},
	{&jong, "jong", "Jong", DEF_JONG, t_Bool},
	{&sine, "sine", "Sine", DEF_SINE, t_Bool}
};
static OptionStruct desc[] =
{
	{"-/+martin", "turn on/off sqrt format"},
	{"-/+popcorn", "turn on/off Clifford A. Pickover's popcorn format"},
	{"-/+ejk1", "turn on/off ejk1 format"},
	{"-/+ejk2", "turn on/off ejk2 format"},
	{"-/+ejk3", "turn on/off ejk3 format"},
	{"-/+ejk4", "turn on/off ejk4 format"},
	{"-/+ejk5", "turn on/off ejk5 format"},
	{"-/+ejk6", "turn on/off ejk6 format"},
	{"-/+rr",   "turn on/off rr format"},
	{"-/+jong", "turn on/off jong format"},
	{"-/+sine", "turn on/off sine format"}
};

*/

martin = 1;

#define MARTIN 0
#define POPCORN 7
#define SINE 8
#define EJK1 1
#define EJK2 2
#define EJK3 9
#define EJK4 3
#define EJK5 4
#define EJK6 10
#define RR 5
#define JONG 6
#define OPS 11


typedef struct {
	int         centerx, centery;	/* center of the screen */
	double      a, b, c, d;
	double      i, j;				/* hopalong parameters */
	int         inc;
	int         pix;
	int         op;
	int         count;
	int         bufsize;
	XPoint     *pointBuffer;	/* pointer for XDrawPoints */
} hopstruct;

static hopstruct *hops = (hopstruct *) NULL;

void
init_hop(HDC hdc, HWND hwnd)
{
	//Display    *display = MI_DISPLAY(mi);
	//GC          gc = MI_GC(mi);
	double      range;
	hopstruct  *hp;

	//if (hops == NULL) {
	//	if ((hops = (hopstruct *) calloc(MI_NUM_SCREENS(mi),
	//					 sizeof (hopstruct))) == NULL)
	//		return;
	//}
	//hp = &hops[MI_SCREEN(mi)];

	hp->centerx = SW / 2;
	hp->centery = SH / 2;
	/* Make the other operations less common since they are less interesting */
	//if (MI_IS_FULLRANDOM(mi)) {
	//	hp->op = NRAND(OPS);
	//} else {
		if (martin)
			hp->op = MARTIN;
		else if (popcorn)
			hp->op = POPCORN;
		else if (ejk1)
			hp->op = EJK1;
		else if (ejk2)
			hp->op = EJK2;
		else if (ejk3)
			hp->op = EJK3;
		else if (ejk4)
			hp->op = EJK4;
		else if (ejk5)
			hp->op = EJK5;
		else if (ejk6)
			hp->op = EJK6;
		else if (rr)
			hp->op = RR;
		else if (jong)
			hp->op = JONG;
		else if (sine)
			hp->op = SINE;
		else
			hp->op = NRAND(OPS);
	//}

	range = sqrt((double) hp->centerx * hp->centerx +
	     (double) hp->centery * hp->centery) / (1.0 + LRAND() / MAXRAND);
	hp->i = hp->j = 0.0;
	hp->inc = (int) ((LRAND() / MAXRAND) * 200) - 100;

	switch (hp->op) {
		case MARTIN:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0;
			hp->b = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0;
			if (LRAND() & 1)
				hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0;
			else
				hp->c = 0.0;

			//if (MI_IS_VERBOSE(mi))
			//	(void) fprintf(stdout, "sqrt a=%g, b=%g, c=%g\n", hp->a, hp->b, hp->c);
			break;
		case EJK1:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0;
			hp->b = (LRAND() / MAXRAND) * 0.4;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 40.0;
			//if (MI_IS_VERBOSE(mi))
			//	(void) fprintf(stdout, "ejk1 a=%g, b=%g, c=%g\n", hp->a, hp->b, hp->c);
			break;
		case EJK2:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0;
			hp->b = pow(10.0, 6.0 + (LRAND() / MAXRAND) * 24.0);
			if (LRAND() & 1)
				hp->b = -hp->b;
			hp->c = pow(10.0, (LRAND() / MAXRAND) * 9.0);
			if (LRAND() & 1)
				hp->c = -hp->c;
			//if (MI_IS_VERBOSE(mi))
			//	(void) fprintf(stdout, "ejk2 a=%g, b=%g, c=%g\n", hp->a, hp->b, hp->c);
			break;
		case EJK3:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0;
			hp->b = (LRAND() / MAXRAND) * 0.35 + 0.5;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 70.0;
			//if (MI_IS_VERBOSE(mi))
			//	(void) fprintf(stdout, "ejk3 a=%g, b=%g, c=%g\n", hp->a, hp->b, hp->c);
			break;
		case EJK4:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 2.0;
			hp->b = (LRAND() / MAXRAND) * 9.0 + 1.0;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 200.0;
			//if (MI_IS_VERBOSE(mi))
			//	(void) fprintf(stdout, "ejk4 a=%g, b=%g, c=%g\n", hp->a, hp->b, hp->c);
			break;
		case EJK5:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 2.0;
			hp->b = (LRAND() / MAXRAND) * 0.3 + 0.1;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 200.0;
			//if (MI_IS_VERBOSE(mi))
			//	(void) fprintf(stdout, "ejk5 a=%g, b=%g, c=%g\n", hp->a, hp->b, hp->c);
			break;
		case EJK6:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 30.0;
			hp->b = (LRAND() / MAXRAND) + 0.5;
			//if (MI_IS_VERBOSE(mi))
			//	(void) fprintf(stdout, "ejk6 a=%g, b=%g\n", hp->a, hp->b);
			break;
		case RR:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 40.0;
			hp->b = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 200.0;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * range / 20.0;
			hp->d = (LRAND() / MAXRAND) * 0.9;
			//if (MI_IS_VERBOSE(mi))
			//	(void) fprintf(stdout, "rr a=%g, b=%g, c=%g, d=%g\n",
			//		       hp->a, hp->b, hp->c, hp->d);
			break;
		case POPCORN:
			hp->a = 0.0;
			hp->b = 0.0;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * 0.24 + 0.25;
			hp->inc = 100;
			//if (MI_IS_VERBOSE(mi))
			//	(void) fprintf(stdout, "popcorn a=%g, b=%g, c=%g, d=%g\n",
			//		       hp->a, hp->b, hp->c, hp->d);
			break;
		case JONG:
			hp->a = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			hp->b = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			hp->c = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			hp->d = ((LRAND() / MAXRAND) * 2.0 - 1.0) * M_PI;
			//if (MI_IS_VERBOSE(mi))
			//	(void) fprintf(stdout, "jong a=%g, b=%g, c=%g, d=%g\n",
			//		       hp->a, hp->b, hp->c, hp->d);
			break;
		case SINE:	/* MARTIN2 */
			hp->a = M_PI + ((LRAND() / MAXRAND) * 2.0 - 1.0) * 0.7;
			//if (MI_IS_VERBOSE(mi))
			//	(void) fprintf(stdout, "sine a=%g\n", hp->a);
			break;
	}
	//if (MI_NPIXELS(mi) > 2)
	//	hp->pix = NRAND(MI_NPIXELS(mi));
	//hp->bufsize = MI_COUNT(mi);
	hp->bufsize = 1;


	//if (hp->pointBuffer == NULL) {
	//	if ((hp->pointBuffer = (XPoint *) malloc(hp->bufsize *
	//			sizeof (XPoint))) == NULL)
	//		return;
	//}

	//MI_CLEARWINDOW(mi);

	//XSetForeground(display, gc, MI_WHITE_PIXEL(mi));
	hp->count = 0;
}


void draw_hop(ModeInfo * mi)
{
	double      oldj, oldi;
	XPoint     *xp;
	int         k;
	hopstruct  *hp;

	if (hops == NULL)
		return;
	//hp = &hops[MI_SCREEN(mi)];
	if (hp->pointBuffer == NULL)
		return;
	xp = hp->pointBuffer;
	k = hp->bufsize;

	//MI_IS_DRAWN(mi) = True;
	hp->inc++;
	//if (MI_NPIXELS(mi) > 2) {
	//	XSetForeground(MI_DISPLAY(mi), MI_GC(mi), MI_PIXEL(mi, hp->pix));
	//	if (++hp->pix >= MI_NPIXELS(mi))
	//		hp->pix = 0;
	//}
	while (k--) {
		oldj = hp->j;
		switch (hp->op) {
			case MARTIN:	/* SQRT, MARTIN1 */
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj + ((hp->i < 0)
					   ? sqrt(fabs(hp->b * oldi - hp->c))
					: -sqrt(fabs(hp->b * oldi - hp->c)));
				xp->x = hp->centerx + (int) (hp->i + hp->j);
				xp->y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK1:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? (hp->b * oldi - hp->c) :
						-(hp->b * oldi - hp->c));
				xp->x = hp->centerx + (int) (hp->i + hp->j);
				xp->y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK2:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i < 0) ? log(fabs(hp->b * oldi - hp->c)) :
					   -log(fabs(hp->b * oldi - hp->c)));
				xp->x = hp->centerx + (int) (hp->i + hp->j);
				xp->y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK3:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? sin(hp->b * oldi) - hp->c :
						-sin(hp->b * oldi) - hp->c);
				xp->x = hp->centerx + (int) (hp->i + hp->j);
				xp->y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK4:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? sin(hp->b * oldi) - hp->c :
					  -sqrt(fabs(hp->b * oldi - hp->c)));
				xp->x = hp->centerx + (int) (hp->i + hp->j);
				xp->y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK5:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i > 0) ? sin(hp->b * oldi) - hp->c :
						-(hp->b * oldi - hp->c));
				xp->x = hp->centerx + (int) (hp->i + hp->j);
				xp->y = hp->centery - (int) (hp->i - hp->j);
				break;
			case EJK6:
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - asin((hp->b * oldi) - (long) (hp->b * oldi));
				xp->x = hp->centerx + (int) (hp->i + hp->j);
				xp->y = hp->centery - (int) (hp->i - hp->j);
				break;
			case RR:	/* RR1 */
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - ((hp->i < 0) ? -pow(fabs(hp->b * oldi - hp->c), hp->d) :
				     pow(fabs(hp->b * oldi - hp->c), hp->d));
				xp->x = hp->centerx + (int) (hp->i + hp->j);
				xp->y = hp->centery - (int) (hp->i - hp->j);
				break;
			case POPCORN:
#define HVAL 0.05
#define INCVAL 50
				{
					double      tempi, tempj;

					if (hp->inc >= 100)
						hp->inc = 0;
					if (hp->inc == 0) {
						if (hp->a++ >= INCVAL) {
							hp->a = 0;
							if (hp->b++ >= INCVAL)
								hp->b = 0;
						}
						hp->i = (-hp->c * INCVAL / 2 + hp->c * hp->a) * M_PI / 180.0;
						hp->j = (-hp->c * INCVAL / 2 + hp->c * hp->b) * M_PI / 180.0;
					}
					tempi = hp->i - HVAL * sin(hp->j + tan(3.0 * hp->j));
					tempj = hp->j - HVAL * sin(hp->i + tan(3.0 * hp->i));
					xp->x = hp->centerx + (int) (SW / 40 * tempi);
					xp->y = hp->centery + (int) (SH / 40 * tempj);
					hp->i = tempi;
					hp->j = tempj;
				}
				break;
			case JONG:
				if (hp->centerx > 0)
					oldi = hp->i + 4 * hp->inc / hp->centerx;
				else
					oldi = hp->i;
				hp->j = sin(hp->c * hp->i) - cos(hp->d * hp->j);
				hp->i = sin(hp->a * oldj) - cos(hp->b * oldi);
				xp->x = hp->centerx + (int) (hp->centerx * (hp->i + hp->j) / 4.0);
				xp->y = hp->centery - (int) (hp->centery * (hp->i - hp->j) / 4.0);
				break;
			case SINE:	/* MARTIN2 */
				oldi = hp->i + hp->inc;
				hp->j = hp->a - hp->i;
				hp->i = oldj - sin(oldi);
				xp->x = hp->centerx + (int) (hp->i + hp->j);
				xp->y = hp->centery - (int) (hp->i - hp->j);
				break;
		}
		xp++;
	}
	//forse come colore il hp->count?
	SetPixel(hdc, xp->x, xp->y, RGB(255,255,255);
	//XDrawPoints(MI_DISPLAY(mi), MI_WINDOW(mi), MI_GC(mi),hp->pointBuffer, hp->bufsize, CoordModeOrigin);
	if(++hp->count > count)
		init_hop(hdc,hwnd);
	//if (++hp->count > MI_(mi)) {
	//    erase_full_window(MI_DISPLAY(mi), MI_WINDOW(mi));
	//	init_hop(mi);
	}
}

void release_hop()
{
	if (hops != NULL) {
		int         screen;

		//for (screen = 0; screen < MI_NUM_SCREENS(mi); screen++) {
		//	hopstruct  *hp = &hops[screen];

		if (hp->pointBuffer != NULL)
				(void) free((void *) hp->pointBuffer);
	
		(void) free((void *) hops);
		hops = (hopstruct *) NULL;
	}
}

