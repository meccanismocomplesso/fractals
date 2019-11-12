#include "Mesh.h"
#include "MeshGen.h"
#include "Progress.h"

Mesh *m_distsphere;
Mesh *m_weirdsphere;
Mesh *m_insidesphere;

Metaballs *m_balls;

void loadMeshes(HWND *wnd)
{
	MeshGen *gen = new MeshGen;

	// Calc level-of-detail
	int m = prefs::polylevel * 8 + 8;

	// Create the wireframe sphere
	m_distsphere = new Mesh;
	gen->MakeSphere(m_distsphere, 1, m, m);
	gen->DistNoise(m_distsphere, 0.5f, 32);

	progress(wnd, 0.5f);

	// And the distorted one inside it
	m_weirdsphere = new Mesh;
	gen->MakeSphere(m_weirdsphere, 0.2f, m*2, m*2);
	gen->DistNoise(m_weirdsphere, 0.002f, 2131);
	gen->DistSine(m_weirdsphere, vec(0.2f, 0, 0), vec(0, 0, 0), 7, 2, 5);
	m_weirdsphere->RecalcNormals();

	progress(wnd, 0.6f);

	// Ant the very distored one inside that again
	m_insidesphere = new Mesh;
	gen->MakeSphere(m_insidesphere, 0.1f, m*2, m*2);
	gen->DistSine(m_insidesphere, vec(0.2f, 0.2f, 0.2f), vec(0, 0, 0), 17, 5, 6);
	m_insidesphere->RecalcNormals();

	progress(wnd, 0.7f);

	delete gen;

	// Animated meshes
	int q = prefs::polylevel*6 + 10;

	m_balls = new Metaballs(q, 2.0f, 5);
	m_balls->SetBall(0, vec(0, 0, 0), 0.2f);
	m_balls->SetBall(1, vec(-0.3f, -0.2f, 0.4f), 0.2f);

	progress(wnd, 0.8f);
}

void killMeshes()
{	
	// Static meshes
	delete m_distsphere;
	delete m_weirdsphere;
	delete m_insidesphere;

	// Animated meshes
	delete m_balls;
}

