#include "stdafx.h"
#include "MeshCollision.h"


CMeshCollision::CMeshCollision()
{
	m_octree = NULL;
}


CMeshCollision::~CMeshCollision()
{
}

void CMeshCollision::createOctree()
{
	for (int z = 0; z < m_Centroides.size(); z++)
		m_octree->add((Point*)&(m_Centroides[z].position));
}
