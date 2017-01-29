#include "stdafx.h"
#include "VMesh.h"
#include <fstream>
#include <string>
#include <iostream>

CVMesh::CVMesh()
{
}


CVMesh::~CVMesh()
{
}

void CVMesh::LoadMSHFile(char * filename)
{
	bool node = 0;
	bool element = 0;
	long long int numberNodes = 0;
	long long int numberIndex = 0;
	ifstream tourus("..\\Assets\\cube.msh");
	string line;
	if (tourus.is_open())
	{
		while (getline(tourus, line))
		{
			if (line == "$Nodes")
			{
				tourus >> numberNodes;
				m_Vertices.resize(numberNodes+1);
				for (int i = 1; i < numberNodes+1; i++)
				{
					int indice;
					tourus >> indice;
					tourus >> m_Vertices[i].Position.x;
					tourus >> m_Vertices[i].Position.y;
					tourus >> m_Vertices[i].Position.z;
					m_Vertices[i].Position.w = 1;
				}
			}

			else if (line == "$Elements")
			{
				tourus >> numberIndex;
				m_IndicesTetrahedros.resize(numberIndex*4);
				for (int i = 0; i < numberIndex; i++)
				{
					int v0;
					int v1;
					int v2;
					int v3;
					int v4;
					int offset = i * 4;
					tourus >> v0;
					tourus >> v1;
					tourus >> v2;
					tourus >> v3;
					tourus >> v4;
					tourus >> m_IndicesTetrahedros[offset];
					tourus >> m_IndicesTetrahedros[offset + 1];
					tourus >> m_IndicesTetrahedros[offset + 2];
					tourus >> m_IndicesTetrahedros[offset + 3];

				}

			}

		}
	}
	tourus.close();

	m_Indices.resize(m_IndicesTetrahedros.size()*3);

	for (unsigned long int i = 0, j = 0; i < m_IndicesTetrahedros.size(); i+= 4, j += 12)
	{
		int v0, v1, v2, v3;
		v0 = m_IndicesTetrahedros[i];
		v1 = m_IndicesTetrahedros[i+1];
		v2 = m_IndicesTetrahedros[i+2];
		v3 = m_IndicesTetrahedros[i+3];

		m_Indices[j] = v0;
		m_Indices[j+1] = v2;
		m_Indices[j + 2] = v1;

		m_Indices[j + 3] = v2;
		m_Indices[j + 4] = v3;
		m_Indices[j + 5] = v1;

		m_Indices[j + 6] = v3;
		m_Indices[j + 7] = v0;
		m_Indices[j + 8] = v1;

		m_Indices[j + 9] = v0;
		m_Indices[j + 10] = v3;
		m_Indices[j + 11] = v2;
	}


	for (unsigned long j = 0; j < m_Vertices.size(); j++)
	{
		VECTOR4D TexCoord = { 0,0,0,0 };
		TexCoord.x = m_Vertices[j].Position.x;
		TexCoord.y = m_Vertices[j].Position.z;
		TexCoord.z = m_Vertices[j].Position.y;
		TexCoord = Normalize(TexCoord);
		TexCoord.x = TexCoord.x * 0.5 + 0.5;
		TexCoord.y = TexCoord.y * 0.5 + 0.5;

		m_Vertices[j].TexCoord = TexCoord;
	}

	BuildTangentSpaceFromTexCoordsIndexed(true);

}
