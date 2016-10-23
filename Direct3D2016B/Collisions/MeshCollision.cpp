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
