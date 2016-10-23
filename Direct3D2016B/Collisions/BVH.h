#pragma once

#include "../Graphics/Mesh.h"
class BVH
{
	struct Box
	{
		VECTOR4D min;
		VECTOR4D max;
		unsigned long idPrimitive;

	}m_Box;
public:

	/* Members */
	
	BVH* m_pLeft;
	BVH* m_pRight;
	VECTOR4D m_Color;

	/* Methods*/
	BVH();
	~BVH();
	void Build(CMesh& object, vector<unsigned long> Primitives);
	void Draw(CDXBasicPainter* painter, int depth, MATRIX4D translation);
	void Traversal(BVH* pTree, 
		MATRIX4D& thisTranslation,
		MATRIX4D& translationTree, 
		CMesh& object1,
		CMesh& object2);
	
};

