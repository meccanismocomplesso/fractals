#ifndef _METABALLS_
#define _METABALLS_

#include "CubeTables.h"

#define sqr(x) ((x)*(x))

class Metaballs
{
	struct Metaball
	{
		vec pos;
		float energy;
	};
	
	struct GridPoint
	{
		float energy;
		vec pos;
	};
	
	struct GridCube
	{
		GridPoint *points[8];
	};

	GridPoint *grid;
	GridCube *cubes;
	
	
	Metaball *balls;

	int gridSize;
	int numBalls;

	// To simplify code a bit
	int makevxyz(int x, int y, int z)
	{
		return x*sqr(gridSize+1)+y*(gridSize+1)+z;
	}

public:
	// Constructor
	Metaballs(int newGridSize, float realSize, int newNumBalls)
	{
		gridSize = newGridSize;
		numBalls = newNumBalls;

		grid = new GridPoint[(gridSize+1)*(gridSize+1)*(gridSize+1)];
		cubes = new GridCube[gridSize*gridSize*gridSize];

		float dx = (realSize*2)/gridSize;

		vec tmp = vec(-realSize, -realSize, -realSize);

		int a, b, c;

		// Make the vertices
		int i = 0;
		for(a = 0; a < gridSize+1; a++)
		{
			tmp.x += dx;
			tmp.y = -realSize;
			for(b = 0; b < gridSize+1; b++)
			{
				tmp.y += dx;
				tmp.z = -realSize;
				for(c = 0; c < gridSize+1; c++)
				{
					tmp.z += dx;
					grid[i++].pos = tmp;
				}
			}
		}

		// Set up the grid points
		i = 0;
		for(a = 0; a < gridSize; a++)
			for(b = 0; b < gridSize; b++)
				for(c = 0; c < gridSize; c++)	
				{
					cubes[i].points[0] = &grid[makevxyz(a  , b  , c  )];
					cubes[i].points[1] = &grid[makevxyz(a+1, b  , c  )];
					cubes[i].points[2] = &grid[makevxyz(a+1, b  , c+1)];
					cubes[i].points[3] = &grid[makevxyz(a  , b  , c+1)];
					cubes[i].points[4] = &grid[makevxyz(a  , b+1, c  )];
					cubes[i].points[5] = &grid[makevxyz(a+1, b+1, c  )];
					cubes[i].points[6] = &grid[makevxyz(a+1, b+1, c+1)];
					cubes[i].points[7] = &grid[makevxyz(a  , b+1, c+1)];
					i++; 
				}

		balls = new Metaball [numBalls];
	}

	// Destructor
	~Metaballs()
	{
		delete [] grid;
		delete [] cubes;
		delete [] balls;
	}

	// Recalculates all grid points based on current metaball data
	void RecalcGrid()
	{
		float sqrEnergy[20];

		for(int j = 0; j < numBalls; j++)
			sqrEnergy[j] = sqr(balls[j].energy);

		float x, y, z;

		// Recalculate the grid
		int i = 0;
		for(int a = 0; a < gridSize+1; a++)
			for(int b = 0; b < gridSize+1; b++)
				for(int c = 0; c < gridSize+1; c++)
				{
					grid[i].energy = 0;

					for(int d = 0; d < numBalls; d++)
					{
						x = grid[i].pos.x - balls[d].pos.x;
						y = grid[i].pos.y - balls[d].pos.y;
						z = grid[i].pos.z - balls[d].pos.z;

						// If the ball is negative, subtract the force rather than adding it
						if(balls[d].energy > 0)
							grid[i].energy += sqrEnergy[d] / (x*x+y*y+z*z);
						else
							grid[i].energy -= sqrEnergy[d] / (x*x+y*y+z*z);
					}

					i++;
				}
	}

	// Draws the mesh. Separated from the recalc to allow multilayered balls,
	// wireframe + fill, multipass, and other special fx
	void Draw(float treshold = 1)
	{
		// Draw all the cubes
		glBegin(GL_TRIANGLES);
			for(int i = 0; i < gridSize*gridSize*gridSize; i++)
				DrawCube(&cubes[i], treshold);
		glEnd();
	}

	// Draw a single cube
	void DrawCube(GridCube *cube, float treshold)
	{
		// Calculate the marching cubes case
		int cubeIndex = 0;
		if(cube->points[0]->energy < treshold) cubeIndex |= 0x01;
		if(cube->points[1]->energy < treshold) cubeIndex |= 0x02;
		if(cube->points[2]->energy < treshold) cubeIndex |= 0x04;
		if(cube->points[3]->energy < treshold) cubeIndex |= 0x08;
		if(cube->points[4]->energy < treshold) cubeIndex |= 0x10;
		if(cube->points[5]->energy < treshold) cubeIndex |= 0x20;
		if(cube->points[6]->energy < treshold) cubeIndex |= 0x40;
		if(cube->points[7]->energy < treshold) cubeIndex |= 0x80;

		// If it's completely inside or outside, it doesn't need to
		// be drawn
		if(!EdgeTable[cubeIndex])
			return;
		
		// Find the interpolated vertices of the cube
		vec verts[12], normals[12];
		if(EdgeTable[cubeIndex] & 0x001)
			Interpolate(cube->points[0], cube->points[1], &verts[0], treshold);
		if(EdgeTable[cubeIndex] & 0x002)
			Interpolate(cube->points[1], cube->points[2], &verts[1], treshold);
		if(EdgeTable[cubeIndex] & 0x004)
			Interpolate(cube->points[2], cube->points[3], &verts[2], treshold);
		if(EdgeTable[cubeIndex] & 0x008)
			Interpolate(cube->points[3], cube->points[0], &verts[3], treshold);
		if(EdgeTable[cubeIndex] & 0x010)
			Interpolate(cube->points[4], cube->points[5], &verts[4], treshold);
		if(EdgeTable[cubeIndex] & 0x020)
			Interpolate(cube->points[5], cube->points[6], &verts[5], treshold);
		if(EdgeTable[cubeIndex] & 0x040)
			Interpolate(cube->points[6], cube->points[7], &verts[6], treshold);
		if(EdgeTable[cubeIndex] & 0x080)
			Interpolate(cube->points[7], cube->points[4], &verts[7], treshold);
		if(EdgeTable[cubeIndex] & 0x100)
			Interpolate(cube->points[0], cube->points[4], &verts[8], treshold);
		if(EdgeTable[cubeIndex] & 0x200)
			Interpolate(cube->points[1], cube->points[5], &verts[9], treshold);
		if(EdgeTable[cubeIndex] & 0x400)
			Interpolate(cube->points[2], cube->points[6], &verts[10], treshold);
		if(EdgeTable[cubeIndex] & 0x800)
			Interpolate(cube->points[3], cube->points[7], &verts[11], treshold);

		// Calculate the normals of the cube
		float squareDist;
		vec tmp;

		float s;
		for(int a = 0; a < 12; a++)
		{
			normals[a] = vec(0, 0, 0);
			for(int b = 0; b < numBalls; b++)
			{
				tmp.x = verts[a].x - balls[b].pos.x;
				tmp.y = verts[a].y - balls[b].pos.y;
				tmp.z = verts[a].z - balls[b].pos.z;
				squareDist = tmp.x*tmp.x+tmp.y*tmp.y+tmp.z*tmp.z;
				s = (balls[b].energy * 2.0f / sqr(squareDist));
				normals[a].x += tmp.x * s;
				normals[a].y += tmp.y * s;
				normals[a].z += tmp.z * s;
			}
			normals[a].Normalize();
		}

		// Finally render the cube
		int i = 0;
		while(TriangleTable[cubeIndex][i] != -1)
		{
			glNormal3fv(normals[TriangleTable[cubeIndex][i  ]].v);
			glVertex3fv(verts[TriangleTable[cubeIndex][i  ]].v);
			
			glNormal3fv(normals[TriangleTable[cubeIndex][i+1]].v);
			glVertex3fv(verts[TriangleTable[cubeIndex][i+1]].v);
			
			glNormal3fv(normals[TriangleTable[cubeIndex][i+2]].v);
			glVertex3fv(verts[TriangleTable[cubeIndex][i+2]].v);
			i += 3;
		}
	}

	// Interpolates between two gridpoints to find the approximate
	// location of the surface
	void Interpolate(GridPoint *p1, GridPoint *p2, vec *vert, float treshold)
	{
		if(p1->energy == treshold)
		{
			*vert = p1->pos;
			return;
		}

		if(p2->energy == treshold)
		{
			*vert = p2->pos;
			return;
		}

		if(p1->energy == p2->energy)
		{
			*vert = p1->pos;
			return;
		}

		float bal = (treshold - p1->energy) / (p2->energy - p1->energy);
		vert->Lerp(p1->pos, p2->pos, bal);
	}

	// Sets the position and energy of a single ball
	void SetBall(int index, vec newPos, float newEnergy)
	{
		// Invialid index -> do nothing
		if(index < 0 || index >= numBalls)
			return;

		balls[index].pos = newPos;
		balls[index].energy = newEnergy;
	}

	// Sets the energy level of all balls
	void SetEnergy(float newEnergy)
	{
		for(int i = 0; i < numBalls; i++)
			balls[i].energy = newEnergy;
	}

	// Multiplies the energy levels of all balls with the given constant
	void ScaleEnergy(float energyFactor)
	{
		for(int i = 0; i < numBalls; i++)
			balls[i].energy *= energyFactor;
	}
};

#endif // _METABALLS_

