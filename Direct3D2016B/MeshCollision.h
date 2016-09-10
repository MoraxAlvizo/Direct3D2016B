#pragma once
#include "Mesh.h"
#include "Octree.h"

struct BoundingBox
{
	VECTOR4D min;
	VECTOR4D max;
};

class CMeshCollision :
	public CMesh
{
public:
	CMeshCollision();
	virtual ~CMeshCollision();

	BoundingBox m_Box;
	COctree* m_octree;
};

