#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "ifs.h"

HINSTANCE hInst;
LPTSTR strAppName = "WndFrame";
LPTSTR WndName    = "IFS Drawer 1.2";
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
			//setParameters_Sierpinsky();
			drawParameters(hWndDlg);
			return TRUE;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDD_REDRAW:
					getParameters(hWndDlg);
					UpdateWindow(hWndMainFrame);
					//redraw();
					return 0;
					break;
				case IDD_FX1:
					fx -= 0.1;
					drawParameters(hWndDlg);
					//redraw();
					return 0;
					break;
				case IDD_FX2:
					fx += 0.1;
					drawParameters(hWndDlg);
					//redraw();
					return 0;
					break;
					break;
				case IDD_FY1:
					fy -= 0.1;
					drawParameters(hWndDlg);
					//redraw();
					return 0;
					break;
				case IDD_FY2:
					fy += 0.1;
					drawParameters(hWndDlg);
					//redraw();
					return 0;
					break;
				case IDD_XY1:
					setIFSOrigine(0.0,0.0);
					return 0;
					break;
				case IDD_XY2:
					setIFSOrigine(0.0,0.5);
					return 0;
					break;
				case IDD_XY3:
					setIFSOrigine(0.0,1.0);
					return 0;
					break;
				case IDD_XY4:
					setIFSOrigine(0.5,0.0);
					return 0;
					break;
				case IDD_XY5:
					setIFSOrigine(0.5,0.5);
					return 0;
					break;
				case IDD_XY6:
					setIFSOrigine(0.5,1.0);
					return 0;
					break;
				case IDD_XY7:
					setIFSOrigine(1.0,0.0);
					return 0;
					break;
				case IDD_XY8:
					setIFSOrigine(1.0,0.5);
					return 0;
					break;
				case IDD_XY9:
					setIFSOrigine(1.0,1.0);
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
					//redraw();
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
					//redraw();
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
				break;

			case IDM_VIEW_COLOR:
				hMenu = GetMenu(hWndMainFrame);
				ToolboxMenuState = GetMenuState(hMenu, IDM_VIEW_COLOR, MF_BYCOMMAND);
				if( LOBYTE(ToolboxMenuState) & MF_CHECKED )
				{
					CheckMenuItem(hMenu, IDM_VIEW_COLOR, MF_BYCOMMAND | MF_UNCHECKED);
					colore_ON = false;
				}
				else
				{
					CheckMenuItem(hMenu, IDM_VIEW_COLOR, MF_BYCOMMAND | MF_CHECKED);
					colore_ON = true;
				}
				break;

			case IDM_FILE_EXIT:
				PostQuitMessage(WM_QUIT);
				return 0;
				break;

			case IDM_IFS_SIERPINSKY:
				setParameters_Sierpinsky();
				return 0;
				break;

			case IDM_IFS_FERN:
				setParameters_Fern();
				OnPaint(hWndMainFrame,hWndToolbox);
				return 0;
				break;

			case IDM_IFS_DRAGON:
				setParameters_Dragon();
				return 0;
				break;

			case IDM_IFS_SPIRAL:
				setParameters_Spiral();
				return 0;
				break;

			case IDM_IFS_LEAF:
				setParameters_Leaf();
				return 0;
				break;

			case IDM_HELP_HELP:
				if(hWndHelp == NULL)
  				hWndHelp = CreateDialog(hInst,MAKEINTRESOURCE(IDD_HELP_DLG),hWndMainFrame,(DLGPROC)HelpProc);
				ShowWindow(hWndHelp, SW_SHOW);
				return 0;
				break;

			case IDM_HELP_ABOUT:
				if(hWndAbout == NULL)
  				hWndAbout = CreateDialog(hInst,MAKEINTRESOURCE(IDD_ABOUT_DLG),hWndMainFrame,(DLGPROC)AboutProc);
				ShowWindow(hWndAbout, SW_SHOW);
				return 0;
				break;
		};
		break;

	case WM_SIZE:
		break;

	case WM_MOVE:
		break;

    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;

	case WM_CREATE:
		srand( (unsigned)time( NULL ) );
		setParameters_Sierpinsky();
		return 0;
        break;

	case WM_KEYDOWN:				
		switch(wParam)
		{
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
		OnPaint(hWnd,hWndToolbox);
		/*
		GetClientRect(hWndMainFrame, &rctMainWnd);
		sw = rctMainWnd.right - rctMainWnd.left;
		sh = rctMainWnd.bottom - rctMainWnd.top;
		setIFSOrigine(fcx,fcy);
		draw(hWnd,hWndToolbox);
		*/
		return 0;
        break;
		

    default:
        	return DefWindowProc(hWnd, Msg, wParam, lParam);
    	}

    	return 0;
}



