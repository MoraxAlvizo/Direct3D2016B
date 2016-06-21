#include "stdafx.h"
#include "Mesh.h"
CMesh::CMesh()
{
}
CMesh::~CMesh()
{
}
bool CMesh::RayCast(VECTOR4D& RayOrigin,
	VECTOR4D & RayDir,
	multimap<float,INTERSECTIONINFO>& Faces //Lista de caras que se intersectan
	)
{
	unsigned long nFaces = m_Indices.size() / 3;
	unsigned long nBaseIndex = 0;
	unsigned long nIntersectedFaces = 0;
	for (unsigned long iFace = 0; iFace < nFaces;iFace++)
	{
		VECTOR4D &V0 = m_Vertices[m_Indices[nBaseIndex+0]].Position;
		VECTOR4D &V1 = m_Vertices[m_Indices[nBaseIndex+1]].Position;
		VECTOR4D &V2 = m_Vertices[m_Indices[nBaseIndex+2]].Position;
		VECTOR4D Intersection;
		if (RayCastOnTriangle(V0, V1, V2, RayOrigin, RayDir, Intersection))
		{
			float dist = Magnity(Intersection - RayOrigin);
			INTERSECTIONINFO II;
			II.Face = iFace;
			II.LocalPosition = Intersection;
			Faces.insert(make_pair(dist, II));
			nIntersectedFaces++;
		}
		nBaseIndex += 3;
	}
	return nIntersectedFaces!=0;
}

void CMesh::VertexShade(CDXBasicPainter::VERTEX(*pVS)(CDXBasicPainter::VERTEX V))
{
	for (unsigned long int i = 0; i < m_Vertices.size(); i++)
		m_Vertices[i] = pVS(m_Vertices[i]);
}

bool RaySphereIntersect(
	VECTOR4D& RayOrigin,
	VECTOR4D& RayDirection, VECTOR4D& SphereCenter,
	float r)
{
	VECTOR4D RO = RayOrigin - SphereCenter;
	float a = Dot(RayDirection, RayDirection);
	float b = 2 * Dot(RayDirection, RO);
	float c = Dot(RO, RO) - r*r;
	float disc = b*b - 4*a*c;
	if (disc < 0)
		return false;
	return true;
}
bool CMesh::RayCast(
	VECTOR4D& RayOrigin,
	VECTOR4D& RayDir,
	multimap<float, unsigned long>& Vertices, float radius)
{
	for (unsigned long i = 0; i < m_Vertices.size(); i++)
	{
		if (RaySphereIntersect(RayOrigin, RayDir, m_Vertices[i].Position, radius))
		{
			float dist = Magnity(m_Vertices[i].Position - RayOrigin);
			Vertices.insert(make_pair(dist, i));
		}
	}
	return Vertices.size()!=0;
}
#include <fstream>
using namespace std;
void CMesh::LoadSuzanne()
{
	fstream in;
	m_Indices.clear();
	m_Vertices.clear();
	in.open("..\\Assets\\Monkey.vertex", ios::in|ios::binary);

	if (!in.is_open())
	{
		return;
	}

	unsigned long ulVetices;
	in.read((char*)&ulVetices, sizeof(unsigned long));
	m_Vertices.resize(ulVetices);

	for (unsigned long i = 0; i < ulVetices; i++)
	{
		VECTOR4D V;
		in.read((char*)&V, sizeof(VECTOR4D));
		m_Vertices[i].Position = V;
	}
	
	in.close();

	in.open("..\\Assets\\Monkey.normal", ios::in | ios::binary);
	
	if (!in.is_open())
	{
		return;
	}

	for (unsigned long i = 0; i < ulVetices; i++)
	{
		in.read((char*)&m_Vertices[i].Normal, sizeof(VECTOR4D));
	}

	in.close();
	in.open("..\\Assets\\Monkey.index", ios::in | ios::binary);

	if (!in.is_open())
		return;

	unsigned long ulIndices;
	in.read((char*)&ulIndices, sizeof(unsigned long));

	m_Indices.resize(ulIndices);

	in.read((char*)&m_Indices[0], sizeof(unsigned long) * ulIndices);
	in.close();

	// Generar espacio de coordenadas de textura
	// Estrategia: Vamos a generar el espacio de textura 
	// a partir de las posiciones de los vertices

	for (unsigned long i = 0; i < m_Vertices.size(); i++)
	{
		VECTOR4D TexCoord = { 0,0,0,0 };
		TexCoord.x = m_Vertices[i].Position.x;
		TexCoord.y = m_Vertices[i].Position.z;
		TexCoord.z = m_Vertices[i].Position.y;
		TexCoord = Normalize(TexCoord);
		TexCoord.x = TexCoord.x * 0.5 + 0.5;
		TexCoord.y = TexCoord.y * 0.5 + 0.5;

		m_Vertices[i].TexCoord = TexCoord;
	}
	return;
}