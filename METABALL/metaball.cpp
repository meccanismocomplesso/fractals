#include <windows.h>		// Header File For Windows
#include <gl\gl.h>		// Header File For The OpenGL32 Library
#include <gl\glu.h>		// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include <scrnsave.h>
#include "BG.h"
#include "Misc.h"
#include "DemoTextures.h"
#include "Metaballs.h"
#include "CubeTables.h"
#include "DemoMeshes.h"

#define TIMER 1

void InitGL(HWND hWnd, HDC & hDC, HGLRC & hRC);
void CloseGL(HWND hWnd, HDC hDC, HGLRC hRC);
void GetConfig();		
void WriteConfig(HWND hDlg);
void SetupAnimation(int Width, int Height);
void CleanupAnimation();
void OnTimer(HDC hDC);

void SetupGLObjects(HWND hWnd, HDC & hDC);
void scene_metaballs(float sceneTime);

int Width, Height; //globals for size of screen


// Screen Saver Procedure
LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  static HDC hDC;
  static HGLRC hRC;
  static RECT rect;

  switch ( message ) {

  case WM_CREATE: 
    	// get window dimensions
    	GetClientRect( hWnd, &rect );
    	Width = rect.right;		
    	Height = rect.bottom;
    
	//get configuration from registry
		GetConfig();

	// setup OpenGL, then animation
    	InitGL( hWnd, hDC, hRC );
    	SetupAnimation(Width, Height);
		SetupGLObjects(hWnd, hDC);

	//set timer to tick every 10 ms
    	SetTimer( hWnd, TIMER, 10, NULL );
    	return 0;
 
  case WM_DESTROY:
    	KillTimer( hWnd, TIMER );
	CleanupAnimation();
  	CloseGL( hWnd, hDC, hRC );
    	return 0;

  case WM_TIMER:
    	OnTimer(hDC);	//animate!	
    	return 0;				
  }

  return DefScreenSaverProc( hWnd, message, wParam, lParam );
}

bool bTumble = true;


BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

// needed for SCRNSAVE.LIB
BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
  return TRUE;
}


/////////////////////////////////////////////////
////   INFRASTRUCTURE ENDS, SPECIFICS BEGIN   ///
/////////////////////////////////////////////////

static GLfloat sceneTime = 0;
GLUquadricObj *quadric;

// Initialize OpenGL
void InitGL(HWND hWnd, HDC & hDC, HGLRC & hRC)
{
  
  PIXELFORMATDESCRIPTOR pfd;
  ZeroMemory( &pfd, sizeof pfd );
  pfd.nSize = sizeof pfd;
  pfd.nVersion = 1;
  //pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL; //blaine's
  pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  
  hDC = GetDC( hWnd );
  
  int i = ChoosePixelFormat( hDC, &pfd );  
  SetPixelFormat( hDC, i, &pfd );

  hRC = wglCreateContext( hDC );
  wglMakeCurrent( hDC, hRC );

}

// Shut down OpenGL
void CloseGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
  wglMakeCurrent( NULL, NULL );
  wglDeleteContext( hRC );

  ReleaseDC( hWnd, hDC );
}


void SetupAnimation(int width, int height)
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

	glShadeModel(GL_SMOOTH);			
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				
	glClearDepth(1.0f);								
	glEnable(GL_DEPTH_TEST);							
	glDepthFunc(GL_LEQUAL);								
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
	//return TRUE;
}

void SetupGLObjects(HWND hWnd, HDC & hDC)
{
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
}

void OnTimer(HDC hDC)
{ 
	scene_metaballs(sceneTime);
	sceneTime += 0.02f;					
	SwapBuffers(hDC);	
}

void CleanupAnimation()
{
	//didn't create any objects, so no need to clean them up
}



/////////   REGISTRY ACCESS FUNCTIONS     ///////////

void GetConfig()
{

	HKEY key;
	//DWORD lpdw;

	if (RegOpenKeyEx( HKEY_CURRENT_USER,
		"Software\\GreenSquare", //lpctstr
		0,			//reserved
		KEY_QUERY_VALUE,
		&key) == ERROR_SUCCESS)	
	{
		DWORD dsize = sizeof(bTumble);
		DWORD dwtype =  0;

		RegQueryValueEx(key,"Tumble", NULL, &dwtype, 
			(BYTE*)&bTumble, &dsize);


		//Finished with key
		RegCloseKey(key);
	}
	else //key isn't there yet--set defaults
	{
		bTumble = true;
	}
	
}

void WriteConfig(HWND hDlg)
{

	HKEY key;
	DWORD lpdw;

	if (RegCreateKeyEx( HKEY_CURRENT_USER,
		"Software\\GreenSquare", //lpctstr
		0,			//reserved
		"",			//ptr to null-term string specifying the object type of this key
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		NULL,
		&key,
		&lpdw) == ERROR_SUCCESS)
		
	{
		RegSetValueEx(key,"Tumble", 0, REG_DWORD, 
			(BYTE*)&bTumble, sizeof(bTumble));

		//Finished with keys
		RegCloseKey(key);
	}

}


// ***********************************************

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
