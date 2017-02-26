#pragma once

#include "../Graphics/Mesh.h"
#define BVH_MAX_LEVEL 21
#define BVH_NUM_NODES (pow(2, BVH_MAX_LEVEL+1)-1)

class BVH
{
	struct Box
	{
		VECTOR4D min;
		VECTOR4D max;
		unsigned long idPrimitive;
		long numPrimitives; /* -1 means that it is a invalid node */
		bool isLeaf;
	}m_Box;
public:

	/* Members */
	
	BVH* m_pLeft;
	BVH* m_pRight;
	VECTOR4D m_Color;

	vector<Box> LBVH;

	/* Compute shaders */
	ID3D11ComputeShader*      m_pCSPreconstruction;
	ID3D11ComputeShader*      m_pCSConstruction;
	ID3D11ComputeShader*      m_pCSPostconstruction;

	/* Methods*/
	BVH();
	~BVH();
	void Build(CMesh& object, vector<unsigned long> Primitives);
	void Draw(CDXPainter* painter, int depth, MATRIX4D translation);
	void Traversal(BVH* pTree, 
		MATRIX4D& thisTranslation,
		MATRIX4D& translationTree, 
		CMesh& object1,
		CMesh& object2);

	void ApplyTransformation(MATRIX4D& m, unsigned long currentNode);

	/* Parallel construction will be divided into 3 steps 
		
		1. Preconstruction  - Init centroids
		2. Construction	    - Set num primitives per level
		3. Postconstruction - build the tree in ascending order 

	*/

	void Preconstruction(CMesh& object);
	void Construction(CMesh& object, unsigned long node,vector<unsigned long> Primitives);
	void Postconstruction(CMesh & object);

	bool CheckIfPrimitivesCollision(BVH* pTree, 
		unsigned long nodeThis, 
		unsigned long nodeTree,
		CMesh& object1,
		CMesh& object2);

	void DrawLBVH(CDXPainter* painter, int depth);
	void TraversalLBVH(BVH* pTree,
		unsigned long nodeThis,
		unsigned long nodeTree,
		CMesh& object1,
		CMesh& object2);

	void BitTrailTraversal(BVH* pTree, 
		MATRIX4D& thisTranslation, 
		MATRIX4D& translationTree, 
		CMesh& object1, 
		CMesh& object2);
	
};

