#pragma once

#include "../Graphics/Mesh.h"
#define BVH_MAX_LEVEL 12
#define BVH_NUM_NODES (pow(2, BVH_MAX_LEVEL+1)-1)

class BVH
{
	struct Box
	{
		VECTOR4D min;
		VECTOR4D max;
		unsigned long idPrimitive;
		unsigned long numPrimitives; /* -1 means that it is a invalid node */
		int offset;
		int axis;
		bool isLeaf;
	}m_Box;

	struct CB_BVH 
	{
		unsigned int level;
		unsigned int numPrimitives;
		unsigned int suma;
	}m_CB_BVH;

#define MAX_INTERSECTIONS 3
	struct Intersections
	{
		unsigned long indexBelow[2][MAX_INTERSECTIONS];
		VECTOR4D listObj[2][MAX_INTERSECTIONS];
		VECTOR4D coordBari[2][MAX_INTERSECTIONS];
		unsigned long size[2];
	};

public:
	/* Compute shaders */
	static ID3D11ComputeShader*     s_pCSPrebuild ;
	static ID3D11ComputeShader*     s_pCSBuild;
	static ID3D11ComputeShader*     s_pCSPostbuild;

public:

	/* Members */
	
	BVH* m_pLeft;
	BVH* m_pRight;
	VECTOR4D m_Color;

	// Buffer in GPU 
	ID3D11Buffer* m_pCB_BVH;
	ID3D11Buffer* m_pGPU_BVH;
	// UAV for GPU buffer
	ID3D11UnorderedAccessView* m_pUAV_GPU_BVH;
	// vector to check if velocity was previous change 
	vector<bool> m_vVertexWasChanged;
	vector<Box> LBVH;

	/* Methods*/
	BVH();
	~BVH();
	void CreateGPUBuffer(CDXManager* pManager);
	void ResetVertexWasChanged(CMesh & object);
	void Build(CMesh& object, vector<unsigned long> Primitives);
	void Draw(CDXPainter* painter, int depth, MATRIX4D translation);
	void Traversal(BVH* pTree, 
		MATRIX4D& thisTranslation,
		MATRIX4D& translationTree, 
		CMesh& object1,
		CMesh& object2);

	void ApplyTransformation(MATRIX4D& m, unsigned long currentNode);
	static void CompileCSShaders(CDXManager* pManager);

	/* Parallel construction will be divided into 3 steps 
		
		1. Preconstruction  - Init centroids
		2. Construction	    - Set num primitives per level
		3. Postconstruction - build the tree in ascending order 

	*/

	void Preconstruction(CMesh& object);
	void Construction(CMesh& object, unsigned long node,vector<unsigned long> Primitives);
	void Postconstruction(CMesh & object);
	void BuildGPU(CDXManager* pManager, CMesh* mesh);

	Intersections CheckIfPrimitivesCollision(BVH* pTree,
		unsigned long nodeThis, 
		unsigned long nodeTree,
		CMesh& object1,
		CMesh& object2);
	
	void PrintLBVH(int node, int depth);

	void DrawLBVH(CDXPainter* painter, int node);
	void TraversalLBVH(BVH* pTree,
		unsigned long nodeThis,
		unsigned long nodeTree,
		CVMesh& object1,
		CVMesh& object2);

	void BitTrailTraversal(BVH* pTree, 
		MATRIX4D& thisTranslation, 
		MATRIX4D& translationTree, 
		CMesh& object1, 
		CMesh& object2);
	
};

