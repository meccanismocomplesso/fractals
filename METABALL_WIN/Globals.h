/*
	GLOBALS.h
*/

#ifndef _GLOBAL_
#define _GLOBAL_

namespace prefs
{
	// Display prefs
	int screenx = 800, screeny = 600;
	int bitdepth = 32;

	// Fullscreen is not practical when debugging
	bool fullscreen = true;

	// Allow time for other tasks?
	bool taskfriendly = false;

	// Performance prefs
	int texquality = 1; // 0 = nearest, 1 = linear, 2 = mipmap
	int polylevel = 2;	// 0 = low, 1 = medium, 2 = high, 3 = LOTS
};


#endif // _GLOBAL 
