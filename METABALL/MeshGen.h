/*
	MeshGen.h
*/

#ifndef _MESHGEN_
#define _MESHGEN_

#include "Mesh.h"

class MeshGen
{
public:
	// Create
	void MakeSphere(Mesh *mesh, float radius, int slices, int stacks);
	void MakeBox(Mesh *mesh, const vec &size, int xsegs, int ysegs, int zsegs);
	void MakeCylinder(Mesh *mesh, float baseRadius, float topRadius, float height, int slices, int stacks);
	void MakeTorus(Mesh *mesh, float radius, float segRadius, int segs, int slices);
	void MakeKnot(Mesh *mesh, float P, float Q, float radius, float segRadius, int segs, int slices);
	void MakeSpiral(Mesh *mesh, float baseRadius, float topRadius, float segRadius, int segs, int slices);
	
	// Distort
	void DistNoise(Mesh *mesh, float amount, int seed);
	void DistSine(Mesh *mesh, const vec &amplitude, const vec &shift, int xsines, int ysines, int zsines);

	// Transform
	void Translate(Mesh *mesh, const vec &offset);
	void Scale(Mesh *mesh, const vec &factor);
	void Grow(Mesh *mesh, float factor);
	void Normalize(Mesh *mesh);
	void Center(Mesh *mesh);

	// Normal ops
	void FlipNormals(Mesh *mesh);
};

// Create
void MeshGen::MakeSphere(Mesh *mesh, float radius, int slices, int stacks)
{
	// Allocate memory for the mesh
	mesh->SetSize(slices*(stacks+1), slices*stacks*6);

	float alpha, beta;

	int a, b, i = 1;

	// The top vertex
	mesh->pNormals[0] = vec(0, 0, 1);
	mesh->pVertices[0] = vec(0, 0, radius);

	// The middle vertices
	for(a = 1; a < stacks; a++)
	{
		alpha = a*(PI/(float)stacks);
		for(b = 0; b < slices; b++)
		{
			beta = b*(PI2/(float)(slices));

			mesh->pNormals[i] = vec(cosf(beta)*sinf(alpha), sinf(beta)*sinf(alpha), cosf(alpha));
			mesh->pVertices[i] = mesh->pNormals[i] * radius;
			i++;
		}
	}

	// The bottom vertex
	mesh->pNormals[i] = vec(0, 0, -1);
	mesh->pVertices[i] = vec(0, 0, -radius);
	
	// The first ring of triangles
	i = 0;
	for(b = 0; b < slices; b++)
	{
		// Triangle
		mesh->pIndices[i++] = 0;
		mesh->pIndices[i++] = 1 + b;
		mesh->pIndices[i++] = 1 + ((b+1)%slices);
	}
	
	// The middle rings
	for(a = 0; a < stacks-2; a++)
		for(b = 0; b < slices; b++)
		{
			// Triangle 1
			mesh->pIndices[i++] = 1 + a*slices + b;
			mesh->pIndices[i++] = 1 + (a+1)*slices + b;
			mesh->pIndices[i++] = 1 + a*slices + ((b+1)%slices);
			
			// Triangle 2
			mesh->pIndices[i++] = 1 + a*slices + ((b+1)%slices);
			mesh->pIndices[i++] = 1 + (a+1)*slices + b;
			mesh->pIndices[i++] = 1 + (a+1)*slices + ((b+1)%slices);
		}

	// And the last ring of triangles
	for(b = 0; b < slices; b++)
	{
		// Triangle
		mesh->pIndices[i++] = 1+slices*(stacks-1);
		mesh->pIndices[i++] = 1 + slices*(stacks-2) +((b+1)%slices);
		mesh->pIndices[i++] = 1 + slices*(stacks-2) + b;
	}
	mesh->numIndices = i;
}

// Distortion
void MeshGen::DistNoise(Mesh *mesh, float amount, int seed)
{
	// Distorts the mesh with some random noise
	for(int a = 0; a < mesh->numVertices; a++)
		mesh->pVertices[a] += mesh->pNormals[a] * semirand(a+seed) * amount;
}

void MeshGen::DistSine(Mesh *mesh, const vec &amplitude, const vec &shift, int xsines, int ysines, int zsines)
{
	// Distorts the mesh with a sine function
	vec tmp, v;

	for(int a = 0; a < mesh->numVertices; a++)
	{
		v = mesh->pVertices[a];

		tmp.x = sinf(v.y*v.z*PI*xsines + shift.x) * amplitude.x;
		tmp.y = sinf(v.x*v.z*PI*xsines + shift.y) * amplitude.y;
		tmp.z = sinf(v.x*v.y*PI*xsines + shift.z) * amplitude.z;

		mesh->pVertices[a] += tmp;
	}
}

// Transform
void MeshGen::Translate(Mesh *mesh, const vec &offset)
{
	// Translates the mesh
	for(int a = 0; a < mesh->numVertices; a++)
		mesh->pVertices[a] += offset;
}

void MeshGen::Scale(Mesh *mesh, const vec &factor)
{
	// Scales the mesh
	for(int a = 0; a < mesh->numVertices; a++)
		mesh->pVertices[a].ComponentMul(factor);
}

void MeshGen::Grow(Mesh *mesh, float factor)
{
	// Makes the mesh "grow" by adding the scaled normals
	// to the vertices
	for(int a = 0; a < mesh->numVertices; a++)
		mesh->pVertices[a] += mesh->pNormals[a] * factor;
} 

void MeshGen::Normalize(Mesh *mesh)
{
	// Makes the mesh have a width/height/depth of 1
	float x = 0, y = 0, z = 0;	

	for(int a = 0; a < mesh->numVertices; a++)
	{
		x = max<float>(x, absf(mesh->pVertices[a].x));
		y = max<float>(y, absf(mesh->pVertices[a].y));
		z = max<float>(z, absf(mesh->pVertices[a].z));
	}

	Scale(mesh, vec(1/x, 1/y, 1/z));
}

void MeshGen::Center(Mesh *mesh)
{
	// Centers the mesh
	vec center;	

	// Calculate center
	for(int a = 0; a < mesh->numVertices; a++)
		center += mesh->pVertices[a];

	center /= (float)mesh->numVertices;

	// Translate back to (0, 0, 0)
	Translate(mesh, -center);
}

#endif // _MESHGEN_
