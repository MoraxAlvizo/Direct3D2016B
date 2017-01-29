#include "stdafx.h"
#include "MeshCollision.h"


CMeshCollision::CMeshCollision()
{
	m_octree = NULL;
	m_BVH = NULL;
	m_TranslationBVH = Identity();
}


CMeshCollision::~CMeshCollision()
{

}

void CMeshCollision::ResetColors()
{
	for (unsigned long i = 0; i < m_Vertices.size(); i++)
		m_Vertices[i].Color = { 0,0,0,0 };
}

void CMeshCollision::createOctree()
{
	for (int z = 0; z < m_Centroides.size(); z++)
		m_octree->add((Point*)&(m_Centroides[z].position));
}

void CMeshCollision::MoveVertex(MATRIX4D Translation)
{
	for (unsigned long i = 0; i < m_Vertices.size(); i++)
		if(i != 0)
			m_Vertices[i].Position = m_Vertices[i].Position * Translation;
}


struct TriangleSurface
{
	int id[3];
	int realTriangle[3];
	int counter;

};

#define VMESH_IS_SAME_ID( id1, id2 ) \
	(id1[0] == id2[0] && id1[1] == id2[1] && id1[2] == id2[2])

void CMeshCollision::CreateMeshCollisionFromVMesh(CVMesh vMesh)
{
	vector<TriangleSurface> counterFaces;

	for (unsigned long i = 0; i < vMesh.m_Indices.size(); i += 3)
	{
		TriangleSurface newFace;
		newFace.realTriangle[0] = newFace.id[0] = vMesh.m_Indices[i];
		newFace.realTriangle[1] = newFace.id[1] = vMesh.m_Indices[i + 1];
		newFace.realTriangle[2] = newFace.id[2] = vMesh.m_Indices[i + 2];

		// ordenar hacer una llave 
		for (unsigned int j = 0; j < 3; j++)
		{
			for (unsigned int k = 0; k < 3; k++)
			{
				if (newFace.id[j] > newFace.id[k])
				{
					int aux = newFace.id[j];
					newFace.id[j] = newFace.id[k];
					newFace.id[k] = aux;
				}
			}
		}
		bool alreadyExist = false;
		for (unsigned int j = 0; j < counterFaces.size(); j++)
		{
			if (VMESH_IS_SAME_ID(newFace.id, counterFaces[j].id))
			{
				alreadyExist = true;
				counterFaces[j].counter++;
				break;
			}
		}

		if (!alreadyExist)
		{
			newFace.counter = 0;
			counterFaces.push_back(newFace);
		}
	}
	m_Indices.clear();

	for (unsigned long i = 0; i < counterFaces.size(); i++)
	{
		if (counterFaces[i].counter == 0)
		{
			m_Indices.push_back(counterFaces[i].realTriangle[0]);
			m_Indices.push_back(counterFaces[i].realTriangle[1]);
			m_Indices.push_back(counterFaces[i].realTriangle[2]);
		}
	}

	m_Vertices = vMesh.m_Vertices;
}
