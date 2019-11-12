#include <windows.h>
#include "resource.h"
#include "fractals.h"
#include "bitmap.h"

HINSTANCE hInst;
LPTSTR strAppName = "WndFrame";
LPTSTR WndName    = "Fractalizer 1.5";
LPTSTR strToolbox = "WndFloater";
LPTSTR AboutName = "About";
LPTSTR HelpName = "Instructions";

HWND   hWndMainFrame, hWndToolbox, hWndAbout, hWndHelp;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg,WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ToolboxProc(HWND hWnd, UINT Msg,WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AboutProc(HWND hWnd, UINT Msg,WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HelpProc(HWND hWnd,UINT Msg,WPARAM wParam, LPARAM lParam);

//---------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    MSG   msg;
    RECT rect;
	WNDCLASSEX  WndClsEx;

	WndClsEx.cbSize        = sizeof(WNDCLASSEX);
	WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
	WndClsEx.lpfnWndProc   = MainWndProc;
	WndClsEx.cbClsExtra    = 0;
	WndClsEx.cbWndExtra    = 0;
	WndClsEx.hIcon         = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_RESFUND));
	WndClsEx.hCursor       = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_TARGET));
	WndClsEx.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	WndClsEx.lpszMenuName  =  MAKEINTRESOURCE(IDR_MAIN_MENU);
	WndClsEx.lpszClassName = strAppName;
	WndClsEx.hInstance     = hInstance;
	WndClsEx.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RESFUND));

	if (!RegisterClassEx(&WndClsEx))
		return (FALSE);

    hInst = hInstance;

    hWndMainFrame = CreateWindow(strAppName,
			           WndName,
                             WS_OVERLAPPEDWINDOW,
                             10,
			           10,
			           sh,
			           sw,
			           NULL,
			           NULL,
			           hInstance,
			           NULL);

    if( !hWndMainFrame )
	return (FALSE);

    // Create a child window based on the available dialog box
    hWndToolbox = CreateDialog(hInst,
		         MAKEINTRESOURCE(IDD_TOOLBOX_DLG),
			   hWndMainFrame,
			   (DLGPROC)ToolboxProc);

    ShowWindow(hWndMainFrame, nCmdShow);
    ShowWindow(hWndToolbox, SW_SHOW);

    while (GetMessage(&msg,NULL, 0,0))
   {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK ToolboxProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_INITDIALOG:
			drawParameters(hWndDlg);
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDD_REDRAW:
					getParameters(hWndToolbox,colours);
					redraw();
					return 0;
					break;
			}
	}
	return FALSE;
}

//---------------------------------------------------------------------------
LRESULT CALLBACK AboutProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDD_ABOUT_OK:
					ShowWindow(hWndAbout, SW_HIDE);
					redraw();
					return 0;
					break;
			}
	}
	return FALSE;
}
//---------------------------------------------------------------------------
LRESULT CALLBACK HelpProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDD_ABOUT_OK:
					ShowWindow(hWndHelp, SW_HIDE);
					redraw();
					return 0;
					break;
			}
	}
	return FALSE;
}

//---------------------------------------------------------------------------
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT Msg,
			      WPARAM wParam, LPARAM lParam)
{
	HMENU hMenu;
	RECT rctMainWnd, rctToolbox;
	UINT ToolboxMenuState;

    switch(Msg)
    {

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case IDM_VIEW_TOOLBOX:
				hMenu = GetMenu(hWndMainFrame);
				ToolboxMenuState = GetMenuState(hMenu, IDM_VIEW_TOOLBOX, MF_BYCOMMAND);
				if( LOBYTE(ToolboxMenuState) & MF_CHECKED )
				{
					CheckMenuItem(hMenu, IDM_VIEW_TOOLBOX, MF_BYCOMMAND | MF_UNCHECKED);
					ShowWindow(hWndToolbox, SW_HIDE);
				}
				else
				{
					CheckMenuItem(hMenu, IDM_VIEW_TOOLBOX, MF_BYCOMMAND | MF_CHECKED);
					ShowWindow(hWndToolbox, SW_SHOW);
				}
				redraw();
				break;

			case IDM_COLOR_52:
				hMenu = GetMenu(hWndMainFrame);
				ToolboxMenuState = GetMenuState(hMenu, IDM_COLOR_52, MF_BYCOMMAND);
				if( LOBYTE(ToolboxMenuState) & MF_CHECKED )
				{
				}
				else
				{
					CheckMenuItem(hMenu, IDM_COLOR_256, MF_BYCOMMAND | MF_UNCHECKED);
					CheckMenuItem(hMenu, IDM_COLOR_52, MF_BYCOMMAND | MF_CHECKED);
					NC = 52;
					getSpectrum(colours);
				}
				redraw();
				break;

			case IDM_COLOR_256:
				hMenu = GetMenu(hWndMainFrame);
				ToolboxMenuState = GetMenuState(hMenu, IDM_COLOR_256, MF_BYCOMMAND);
				if( LOBYTE(ToolboxMenuState) & MF_CHECKED )
				{
				}
				else
				{
					CheckMenuItem(hMenu, IDM_COLOR_52, MF_BYCOMMAND | MF_UNCHECKED);
					CheckMenuItem(hMenu, IDM_COLOR_256, MF_BYCOMMAND | MF_CHECKED);
					NC = 256;
					getSpectrum(colours);
				}
				redraw();
				break;

			case IDM_FILE_EXIT:
				PostQuitMessage(WM_QUIT);
				return 0;
				break;

			case IDM_FILE_SAVE:
				saveAsBitmap(hWnd);
				redraw();
				return 0;
				break;

			case IDM_FRACTAL_MANDEL:
				alg = MANDEL;
				resetParameters();
				redraw();
				return 0;
				break;

			case IDM_FRACTAL_MANDELINV:
				alg = MANDELINV;
				resetParameters();
				zoom = 0.35;
				a1 = -0.7;
				redraw();
				return 0;
				break;

			case IDM_FRACTAL_JULIA:
				alg = JULIA;
				resetParameters();
				a1 = -1.1;
				redraw();
				return 0;
				break;

            case IDM_FRACTAL_JULIACUBIC:
				alg = JULIA_CUBIC;
				resetParameters();
				a1 = -1.1;
				w = 1.8; v = 1.8; zoom = 1.0;
				redraw();
				return 0;
				break;

            case IDM_FRACTAL_NEWTONRAPHSON:
				alg = NEWTON_RAPHSON;
				resetParameters();
				a1 = -1.1;
				redraw();
				return 0;
				break;

            case IDM_FRACTAL_NEWTONRAPHSON2:
				alg = NEWTON_RAPHSON2;
				resetParameters();
				a1 = -1.1;
				redraw();
				return 0;
				break;

			case IDM_FRACTAL_NEWTONRAPHSON3:
				alg = NEWTON_RAPHSON3;
				resetParameters();
				a1 = -1.1;
				redraw();
				return 0;
				break;

			case IDM_FRACTAL_JULIAN:
				alg = JULIAN;
				resetParameters();
				a1 = -1.1;
				w = -0.2; v = -0.6; k = 1.5; 
				redraw();
				return 0;
				break;

			case IDM_FRACTAL_HOPALONG:
				alg = HOPALONG;
				resetParameters();
				a1 = 0; b1 = 0;
				k = 2.5; w = 0.5; v =3.2;
				zoom = 5.2;
				redraw();
				return 0;
				break;

				
			case IDM_FRACTAL_QUADRUP2:
				alg = QUADRUP2;
				resetParameters();
				a1 = 3,4; b1 = 0;
				k = 34; w = 1; v =5;
				zoom = 4;
				redraw();
				return 0;
				break;

			case IDM_FRACTAL_THREEPLY:
				alg = THREEPLY;
				resetParameters();
				a1 = -1.1;
				k = -55; w = -1; v =-42;
				zoom = 0.2;
				redraw();
				return 0;
				break;

			case IDM_HELP_HELP:
				if(hWndHelp == NULL)
  				hWndHelp = CreateDialog(hInst,MAKEINTRESOURCE(IDD_HELP_DLG),hWndMainFrame,(DLGPROC)HelpProc);
				ShowWindow(hWndHelp, SW_SHOW);
				redraw();
				return 0;
				break;

			case IDM_HELP_ABOUT:
				if(hWndAbout == NULL)
  				hWndAbout = CreateDialog(hInst,MAKEINTRESOURCE(IDD_ABOUT_DLG),hWndMainFrame,(DLGPROC)AboutProc);
				ShowWindow(hWndAbout, SW_SHOW);
				redraw();
				return 0;
				break;
		};
		break;

	case WM_SIZE:
		GetClientRect(hWndMainFrame, &rctMainWnd);
		sw = rctMainWnd.right - rctMainWnd.left;
		sh = rctMainWnd.bottom - rctMainWnd.top; 
		redraw();
		break;

	case WM_MOVE:
		redraw();
		break;

    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;

	case WM_CREATE:
		srand( (unsigned)time( NULL ) );
		getSpectrum(colours);
		return 0;
        break;

	case WM_KEYDOWN:				
		switch(wParam)
		{
				case 0x31: //tasto 1
					increaseK();
					redraw();
				break;
				case 0x32: //tasto 2
					decreaseK();
					redraw();
				break;
				case 0x33: //tasto 3
					increaseW();
					redraw();
				break;
				case 0x34: //tasto 4
					decreaseW();
					redraw();
				break;
				case 0x35: //tasto 5
					increaseV();
					redraw();
				break;
				case 0x36: //tasto 6
					decreaseV();
					redraw();
				break;
				case VK_SPACE:
					changeSpectrum();
					redraw();
				break;
				case VK_LEFT: 
					moveToLeft();
					redraw();
				break;
				case VK_RIGHT: 
					moveToRight();
					redraw();
				break;
				case VK_UP: 
					moveToUp();
					redraw();
				break;
				case VK_DOWN: 
					moveToDown();
					redraw();
				break;
				case 0x5A: //tasto z
					zoomIn();
					redraw();
				break;
				case 0x58: //tasto x
					zoomOut();
					redraw();
				break;				
				default:
					keys[wParam] = TRUE;
				break;
			}		
			return 0;
			break;
	case WM_KEYUP:				
		keys[wParam] = FALSE;		
		return 0;				
		break;

    case WM_PAINT:
		if(!done){
			if(alg == HOPALONG || alg == QUADRUP2 || alg == THREEPLY)
			    iter_tipo_orbital(hWnd,hWndToolbox,i,j);
			else
				iter_tipo_mandel2(hWnd,hWndToolbox,i,j);
			done = true;
		}
		return 0;
        break;

    default:
        	return DefWindowProc(hWnd, Msg, wParam, lParam);
    	}

    	return 0;
}



