/*
	Mesh.h
*/

#ifndef _MESH_
#define _MESH_

#include "Misc.h"

class Mesh
{
	// Vertices data
	vec *pVertices;
	vec *pNormals;
	int numVertices;

	// Indices data
	unsigned short *pIndices;
	int numIndices;

	// Primitive type, only GL_TRIANGLES is supported at the moment
	unsigned int primitiveType;

	// The Mesh Generator is a friend, so let it in
	friend class MeshGen;

public:
	// Default constructor
	Mesh(): pVertices(NULL), numVertices(0),
		    pIndices(NULL), numIndices(0),
		    primitiveType(GL_TRIANGLES) {}

	// More intuitive constructor
	Mesh(int newNumVertices, int newNumIndices): numVertices(newNumVertices), numIndices(newNumIndices),
												 pVertices(new vec [numVertices]), pNormals(new vec [numVertices]),
												 pIndices(new unsigned short [numIndices]),
												 primitiveType(GL_TRIANGLES) {}
	// Destructor
	~Mesh()
	{
		delete [] pVertices;
		delete [] pNormals;
		delete [] pIndices;
	}

	// Draws the mesh using vertex arrays
	void Draw() const
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		
		glVertexPointer(3, GL_FLOAT, 0, pVertices);
		glNormalPointer(GL_FLOAT, 0, pNormals);

		glDrawElements(primitiveType, numIndices, GL_UNSIGNED_SHORT, pIndices);

		// No reason to disable
		//glDisableClientState(GL_VERTEX_ARRAY);
		//glDisableClientState(GL_NORMAL_ARRAY);
	}
	
	// Draws the normals of the mesh
	void DrawNormals(float length) const
	{
		vec tmp;

		glBegin(GL_LINES);
			for(int a = 0; a < numVertices; a++)
			{
				tmp = pVertices[a];
				glVertex3fv(tmp.v);

				tmp += pNormals[a] * length;
				glVertex3fv(tmp.v);
			}
		glEnd();
	}

	// Recalculates some normals for the mesh
	void RecalcNormals()
	{
		// TODO: Make this code support non-triangular meshes
		ZeroMemory(pNormals, sizeof(vec)*numVertices);

		vec n;
		for(int a = 0; a < numIndices; a += 3)
		{
			n.NormalOf(pVertices[pIndices[ a ]],
					   pVertices[pIndices[a+1]],
					   pVertices[pIndices[a+2]]);

			n.Normalize();
			
			pNormals[pIndices[ a ]] += n;
			pNormals[pIndices[a+1]] += n;
			pNormals[pIndices[a+2]] += n;
		}

		for(int b = 0; b < numVertices; b++)
			pNormals[b].Normalize();
	}

	// Resizes the mesh (preserves data)
	void Resize(int newNumVertices, int newNumIndices)
	{
		// Calc the amount of data to preserve
		int tmpNumVertices = min<int>(numVertices, newNumVertices);
		int tmpNumIndices = min<int>(numIndices, newNumIndices);

		// Save the new sizes
		numVertices = newNumVertices;
		numIndices = newNumIndices;

		// Allocate some memory
		vec *tmpVertices = new vec [numVertices];
		vec *tmpNormals = new vec [numVertices];
		unsigned short *tmpIndices = new unsigned short [numIndices];

		// Copy the old memory to the new
		memcpy(tmpVertices, pVertices, tmpNumVertices*sizeof(vec));
		memcpy(tmpNormals, pNormals, tmpNumVertices*sizeof(vec));
		memcpy(tmpIndices, pIndices, tmpNumIndices*sizeof(unsigned short));

		// Free the old memory
		if(pVertices) delete [] pVertices;
		if(pVertices) delete [] pNormals;
		if(pVertices) delete [] pIndices;

		// .. And make the pointers point to the new memory
		pVertices = tmpVertices;
		pNormals = tmpNormals;
		pIndices = tmpIndices;
	}

	// Resizes the mesh (doesn't preserve data)
	void SetSize(int newNumVertices, int newNumIndices)
	{
		// Save the new sizes
		numVertices = newNumVertices;
		numIndices = newNumIndices;

		// Free the old memory
		if(pVertices) delete [] pVertices;
		if(pVertices) delete [] pNormals;
		if(pVertices) delete [] pIndices;

		// And allocate some new with the given dimensions
		pVertices = new vec [numVertices];
		pNormals = new vec [numVertices];
		pIndices = new unsigned short [numIndices];
	}
};

#endif // _MESH_
