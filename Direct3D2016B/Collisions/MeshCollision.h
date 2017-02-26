#pragma once
#include "../Graphics/Mesh.h"
#include "../Cut/VMesh.h"
#include "BVH.h"
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
	BVH* m_BVH;
	unsigned long m_lID;

	void ResetColors();
	void createOctree();
	void MoveVertex(MATRIX4D Translation);
	void CreateMeshCollisionFromVMesh(CVMesh vMesh);
	void ApplyTransformation(MATRIX4D & m);

};

