#include <windows.h>		// Header File For Windows
#include <gl\gl.h>		// Header File For The OpenGL32 Library
#include <gl\glu.h>		// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include "BG.h"
#include "Misc.h"
#include "DemoTextures.h"
#include "Metaballs.h"
#include "CubeTables.h"
#include "DemoMeshes.h"
#include "ico_res.h"

HDC		hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	      // Fullscreen Flag Set To Fullscreen Mode By Default

// **************** ADDED *************************
static float sceneTime = 0;
//Metaballs *m_balls;
GLUquadricObj *quadric;
// *************************

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		
{
	if (height==0)										
		height=1;										

	glViewport(0,0,width,height);			// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);			// Select The Projection Matrix
	glLoadIdentity();					// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);			// Select The Modelview Matrix
	glLoadIdentity();					// Reset The Modelview Matrix
}

int InitGL(GLvoid)										
{
	glShadeModel(GL_SMOOTH);			
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				
	glClearDepth(1.0f);								
	glEnable(GL_DEPTH_TEST);							
	glDepthFunc(GL_LEQUAL);								
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
	//return TRUE;

	// Generate some textures and meshes
	loadTextures(&hWnd);
	loadMeshes(&hWnd);
	progress(&hWnd, 0.9f);
	// Enable texturing
	Texture::Enable(true);
	// Randomize
	srand(GetTickCount());
	// Get a quadric object
	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	progress(&hWnd, 1);						

	return TRUE;
}

void scene_metaballs(float sceneTime)
{
	// Clear the depth buffer only, as the background covers all of the screen anyways...
	glClear(GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Draw the background
	glColor3f(0.5f, 0.5f, 0.5f);
	t_refl->Bind();
	bg::DrawZooming(sceneTime*0.015f);

	// Enable lighting
	float lightPos[] = {0, 0, 0, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glTranslatef(0, 0, -4);

	glColor3f(1, 1, 1);

	// Enable texture coordinate generation
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	t_refl->Bind();

	// Update the positions of the metaballs
	m_balls->SetBall(0, vec(sinf(sceneTime*1.0f), cosf(sceneTime*1.3f), sinf(sceneTime*2.0f)), 0.15f);
	m_balls->SetBall(1, vec(sinf(sceneTime*0.2f), cosf(sceneTime*1.4f), sinf(sceneTime*1.1f)), 0.15f);
	m_balls->SetBall(2, vec(sinf(sceneTime*1.4f), cosf(sceneTime*0.7f), sinf(sceneTime*1.3f)), 0.20f);
	m_balls->SetBall(3, vec(sinf(sceneTime*1.1f), cosf(sceneTime*2.1f), sinf(sceneTime*1.2f)), 0.15f);
	m_balls->SetBall(4, vec(sinf(sceneTime*0.6f), cosf(sceneTime*1.4f), sinf(sceneTime*0.5f)), 0.20f);

	// Recalculate the metaballs
	m_balls->RecalcGrid();

	// Setup some blending
	glColor4f(1, 1, 1, 0.2f);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// Draw the metaballs with two different layers
	m_balls->Draw(0.1f);
	m_balls->Draw(0.15f);

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glDisable(GL_LIGHTING);

	// Draw the fade in
	if(sceneTime < 1)
	{
		glEnable(GL_BLEND);
		Texture::Enable(false);

		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(1, 1, 1, 1-sceneTime);
		
		bg::DrawStatic();

		glDisable(GL_BLEND);
		Texture::Enable(true);
	}

}


GLvoid KillGLWindow(GLvoid)								
{
	if (fullscreen)										
	{
		ChangeDisplaySettings(NULL,0);					
		ShowCursor(TRUE);								
	}

	if (hRC)											
	{
		if (!wglMakeCurrent(NULL,NULL))		// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(hRC))			// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										
	}

	if (hDC && !ReleaseDC(hWnd,hDC))			// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										
	}

	if (hWnd && !DestroyWindow(hWnd))			// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										
	}

	if (!UnregisterClass("OpenGL",hInstance))		// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;					
	}
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint	PixelFormat;	// Holds The Results After Searching For A Match
	WNDCLASS	wc;			// Windows Class Structure
	DWORD		dwExStyle;		// Window Extended Style
	DWORD		dwStyle;		// Window Style
	RECT		WindowRect;		// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;	// Set Left Value To 0
	WindowRect.right=(long)width;	// Set Right Value To Requested Width
	WindowRect.top=(long)0;		// Set Top Value To 0
	WindowRect.bottom=(long)height;// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;	// Set The Global Fullscreen Flag

	hInstance		= GetModuleHandle(NULL);			// Grab An Instance For Our Window
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc	= (WNDPROC) WndProc;				// WndProc Handles Messages
	wc.cbClsExtra	= 0;							// No Extra Window Data
	wc.cbWndExtra	= 0;							// No Extra Window Data
	wc.hInstance	= hInstance;					// Set The Instance
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);		// Load The Arrow Pointer
	wc.hbrBackground	= NULL;						// No Background Required For GL
	wc.lpszMenuName	= NULL;						// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";						// Set The Class Name
	wc.hIcon = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_RESFUND));

	if (!RegisterClass(&wc))							// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}
	
	if (fullscreen)									
	{
		DEVMODE dmScreenSettings;						// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	      // Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;				// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","Fabio GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		
			}else{
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;				
			}
		}
	}
	if (fullscreen)							
	{
		dwExStyle=WS_EX_APPWINDOW;				// Window Extended Style
		dwStyle=WS_POPUP;						// Windows Style
		ShowCursor(FALSE);					// Hide Mouse Pointer
	}else{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;	// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;				// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);	// Adjust Window To True Requested Size

	if (!(hWnd=CreateWindowEx(	dwExStyle,				// Extended Style For The Window
					"OpenGL",					// Class Name
					title,					// Window Title
					dwStyle |					// Defined Window Style
					WS_CLIPSIBLINGS |				// Required Window Style
					WS_CLIPCHILDREN,				// Required Window Style
					0, 0,						// Window Position
					WindowRect.right-WindowRect.left,	// Calculate Window Width
					WindowRect.bottom-WindowRect.top,	// Calculate Window Height
					NULL,						// No Parent Window
					NULL,						// No Menu
					hInstance,					// Instance
					NULL)))					// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	static PIXELFORMATDESCRIPTOR pfd=	// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,						// Version Number
		PFD_DRAW_TO_WINDOW |			// Format Must Support Window
		PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,				// Must Support Double Buffering
		PFD_TYPE_RGBA,				// Request An RGBA Format
		bits,						// Select Our Color Depth
		0, 0, 0, 0, 0, 0,				// Color Bits Ignored
		0,						// No Alpha Buffer
		0,						// Shift Bit Ignored
		0,						// No Accumulation Buffer
		0, 0, 0, 0,					// Accumulation Bits Ignored
		16,						// 16Bit Z-Buffer (Depth Buffer)  
		0,						// No Stencil Buffer
		0,						// No Auxiliary Buffer
		PFD_MAIN_PLANE,				// Main Drawing Layer
		0,						// Reserved
		0, 0, 0					// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))				
	{
		KillGLWindow();				
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;				
	}
	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	
	{
		KillGLWindow();				
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;				
	}
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		
	{
		KillGLWindow();				
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;				
	}
	if (!(hRC=wglCreateContext(hDC)))		
	{
		KillGLWindow();				
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;				
	}
	if(!wglMakeCurrent(hDC,hRC))			
	{
		KillGLWindow();				
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;				
	}
	ShowWindow(hWnd,SW_SHOW);			
	SetForegroundWindow(hWnd);			
	SetFocus(hWnd);					
	ReSizeGLScene(width, height);			
	if (!InitGL())					
	{
		KillGLWindow();				
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;				
	}
	return TRUE;					
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT	uMsg, WPARAM wParam, LPARAM lParam)	
{
	switch (uMsg)					
	{
		case WM_ACTIVATE:				
		{
			if (!HIWORD(wParam))		
			{
				active=TRUE;		
			}else{
				active=FALSE;		
			}
			return 0;				
		}

		case WM_SYSCOMMAND:		
		{
			switch (wParam)			
			{
				case SC_SCREENSAVE:	
				case SC_MONITORPOWER:	
				return 0;			
			}
			break;				
		}

		case WM_CLOSE:				
		{
			PostQuitMessage(0);		
			return 0;				
		}

		case WM_KEYDOWN:				
		{
			keys[wParam] = TRUE;		
			return 0;				
		}

		case WM_KEYUP:				
		{
			keys[wParam] = FALSE;		
			return 0;				
		}

		case WM_SIZE:				
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  
			return 0;				
		}
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,		// Instance
				HINSTANCE	hPrevInstance,	// Previous Instance
				LPSTR		lpCmdLine,		// Command Line Parameters
				int		nCmdShow)		// Window Show State
{
	MSG	msg;		
	BOOL	done=FALSE;	
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;	// Windowed Mode
	}
	if (!CreateGLWindow("Liquids - The Blue Ant",640,480,16,fullscreen))
	{
		return 0;		// Quit If Window Was Not Created
	}
	while(!done)	
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	
		{
			if (msg.message==WM_QUIT)				
			{
				done=TRUE;							
			}else	{
				TranslateMessage(&msg);				
				DispatchMessage(&msg);				
			}
		}else{
			if (active)
			{
				if (keys[VK_ESCAPE])				
				{
					done=TRUE;						
				}else{
					//DrawGLScene();
					// ************************ 
					scene_metaballs(sceneTime);
					sceneTime += 0.02f;					
					// ************************
					SwapBuffers(hDC);				
				}
			}
			if (keys[VK_F1])						
			{
				keys[VK_F1]=FALSE;					
				KillGLWindow();						
				fullscreen=!fullscreen;				
				if (!CreateGLWindow("Liquids - The Blue Ant",640,480,16,fullscreen))
				{
					return 0;						
				}
			}
		}
	}
	KillGLWindow();							
	return (msg.wParam);
}
