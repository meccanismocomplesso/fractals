/* September 2002: Craig Wilkie <craig.wilkie@bigfoot.com>
               General Utilities for Windows Screensavers */

#include <windows.h>
#include <scrnsave.h>
#include <time.h>
#include "utils.h"
#include "options.h"

/* The options structure must be provided by the screensaver */
extern OPTIONS_STRUCT screenhack_options;

/* These functions must be provided by each screensaver */
extern void screenhack_init(HDC dc, HWND wnd);
extern void screenhack_loop(HDC dc, HWND wnd);

/* The window procedure for the configuration dialog. This will call: -
    read_registry_data
	set_dialog_options
	get_dialog_options
	write_registry_data
   which must be supplied by the screensaver itself */
BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_INITDIALOG:

            read_registry_data(&screenhack_options);

			set_dialog_options(hDlg, &screenhack_options);

            return TRUE;

        case WM_COMMAND:

            switch(LOWORD(wParam))
            {
                case IDOK:

					get_dialog_options(hDlg, &screenhack_options);

                    write_registry_data(&screenhack_options);
                    EndDialog(hDlg, TRUE);

                    return TRUE;
 
                case IDCANCEL:
                    EndDialog(hDlg, FALSE);

                    return TRUE;
            }
    }
    return FALSE;
}

/* The main window procedure for a screen saver. This will call: -
    read_registry_data 
    screenhack_init
    screenhack_loop
   which must be supplied by the screensaver itself */
LONG WINAPI ScreenSaverProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_CREATE:

			/* Seed the random number generator with a unique seed (almost) */
			srand(time(NULL));

            read_registry_data(&screenhack_options);

            /* Screenhack initialisation */
            screenhack_init(GetDC(hwnd), hwnd);

            break;

        case WM_TIMER:
            if (wParam == 10)
            {
                /* Call the main screenhack loop */
                screenhack_loop(GetDC(hwnd), hwnd);
            }
 
            return DefScreenSaverProc(hwnd, message, wParam, lParam);

        case WM_ERASEBKGND:
            return 1;

        case WM_DESTROY:
            break;
    }

    return DefScreenSaverProc(hwnd, message, wParam, lParam);
}


BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
	return TRUE;
}
