Readme file for Inter Screensaver version 1.0
------------------------------------------------------

THIS SOFTWARE IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND.
THE AUTHOR SHALL HAVE NO LIABILITY WITH RESPECT TO THE INFRINGEMENT OF COPYRIGHTS,
TRADE SECRETS OR ANY PATENTS BY THIS FILE OR ANY PART THEREOF.
IN NO EVENT WILL THE AUTHOR BE LIABLE FOR ANY LOST REVENUE OR PROFITS OR OTHER SPECIAL,
INDIRECT AND CONSEQUENTIAL DAMAGES.

The Blue Ant  www.the-blueant.com
1.0 - Initial Release

March 2006: Released on the Blue Ant site <www.the-blueant.com>
December 2006: Ported to Windows OS <fabio.nelli@virgilio.it>

Copyright (c) 2005-2006 Nelli Fabio  <fabio.nelli@virgilio.it>

Used and Modified some code of Craig Wilkie <craig.wilkie@bigfoot.com>
___________________________________________________________________________

- INERFERENCES -  
colored fields via decaying sinusoidal waves

____________________________________________________________________________


This was ported from the Unix xscreensaver package, which is copyright 
(c) 1992, 1995, 1996, 1997, 1998 Jamie Zawinski <jwz@jwz.org>

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  No representations are made about the suitability of this
software for any purpose.  It is provided "as is" without express or 
implied warranty.

/* interference.c --- colored fields via decaying sinusoidal waves.
 
* An entry for the RHAD Labs Screensaver Contest.
 *
 
* Author: Hannu Mallat <hmallat@cs.hut.fi>
 *
 
* Copyright (C) 1998 Hannu Mallat.
 *
 
* Permission to use, copy, modify, distribute, and sell this software and its
 
* documentation for any purpose is hereby granted without fee, provided that
 
* the above copyright notice appear in all copies and that both that
 
* copyright notice and this permission notice appear in supporting
 
* documentation.  No representations are made about the suitability of this
 
* software for any purpose.  It is provided "as is" without express or 
 
* implied warranty.
 *
 * decaying sinusoidal waves, which extend spherically from their
 
* respective origins, move around the plane. a sort of interference
 
* between them is calculated and the resulting twodimensional wave
 
* height map is plotted in a grid, using softly changing colours.
 *
 
* not physically (or in any sense) accurate, but fun to look at for 
 
* a while. you may tune the speed/resolution/interestingness tradeoff 
 
* with X resources, see below.
 *
 
* Created      : Wed Apr 22 09:30:30 1998, hmallat
 
* Last modified: Wed Apr 22 09:30:30 1998, hmallat
 
* Last modified: Sun Aug 31 23:40:14 2003, 
 
*              david slimp <rock808@DavidSlimp.com>
 
* 		added -hue option to specify base color hue
 *
 
* TODO:
 *
 *    This really needs to be sped up.
 *
 
*    I've tried making it use XPutPixel/XPutImage instead of XFillRectangle,
 
*    but that doesn't seem to help (it's the same speed at gridsize=1, and
 
*    it actually makes it slower at larger sizes.)
 *
 
*    I played around with shared memory, but clearly I still don't know how
 
*    to use the XSHM extension properly, because it doesn't work yet.
 *
 
*    Hannu had put in code to use the double-buffering extension, but that
 
*    code didn't work for me on Irix.  I don't think it would help anyway,
 
*    since it's not the swapping of frames that is the bottleneck (or a source
 
*    of visible flicker.)
 *
 
*     -- jwz, 4-Jun-98
 
*/

_____________________________________________________________________________

This distribution should contain the following files: -

    inter.scr           (The main screensaver)
    inter-Readme.txt    (This file)

To install the Screensaver, right click on it from Windows Explorer, and select "Install"
_______________________________________________________________________________


If anyone has any suggestions for improvements, or bug reports, please mail me at: -

    fabio.nelli@virgilio.it

Check my web site for further releases and upgrades: -

    http://www.the-blueant.com/screensaver.html

