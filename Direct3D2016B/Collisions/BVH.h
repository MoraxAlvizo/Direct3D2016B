#pragma once

#include "../Graphics/Mesh.h"
class BVH
{
	struct Box
	{
		VECTOR4D min;
		VECTOR4D max;
	}m_Box;
public:

	/* Members */
	vector<unsigned long> m_Primitives;
	BVH* m_pLeft;
	BVH* m_pRight;

	/* Methods*/
	BVH();
	~BVH();
	void Build(CMesh& object, vector<unsigned long> Primitives);
	void Draw(CDXBasicPainter* painter, int depth);
	
};

