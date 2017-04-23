#pragma once
#include "Matrix4D.h"
#include <vector>
#include <map>
#include "DXPainter.h"
#include <assimp/scene.h>
#include <set>
#include "../Cut/VolumeMeshGenericID.h"

using namespace std;

struct BoundingBox
{
	VECTOR4D min;
	VECTOR4D max;
};

struct centroid
{
	int id;
	VECTOR4D position;
	VECTOR4D max;
	VECTOR4D min;
};

struct auxVecinos
{
	set<long> vecinos;
};

#define MESH_MAX_VECINOS 24
struct vecinosVisuales
{
	unsigned long size;
	unsigned long idVecinos[MESH_MAX_VECINOS];
};

struct CVMesh;
struct BVH;

class CMesh
{
public:
	/* Compute shaders */
	static ID3D11ComputeShader*     s_pCSApplyTransform;
	static ID3D11Buffer*			s_pCBMesh; // Constan buffer Mesh.hlsl

	static void CompileCSShaders(CDXManager* pManager);

	struct PARAMS_MESH_CB
	{
		MATRIX4D Transformation;
	}m_Params_Mesh_CB;
	
public:

	struct INTERSECTIONINFO
	{
		int Face;
		VECTOR4D LocalPosition;
	};
	vector<CDXPainter::VERTEX> m_Vertices;
	vector<unsigned long> m_Indices;
	//MATRIX4D m_World; //
	vector<centroid> m_Centroides;

	char m_cName[128];
	// DirectX buffers
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	ID3D11Buffer* m_pPrimitivesBuffer;
	ID3D11Buffer* m_pVisualNeighbors;
	// UAV For vertex buffer  and primbuffer
	ID3D11UnorderedAccessView* m_pUAVVertexBuffer;
	ID3D11UnorderedAccessView* m_pUAVPrimitiveBuffer;

	// SRV For vertex buffer and index buffer
	ID3D11ShaderResourceView* m_pSRVVertexBuffer;
	ID3D11ShaderResourceView* m_pSRVIndexBuffer;
	ID3D11ShaderResourceView* m_pSRVVisualNeighbors;

	void CreateVertexAndIndexBuffer(CDXManager* m_pManager);
	void Draw(CDXPainter* m_pPainter);
	void CSApplyTranformation(MATRIX4D & t, CDXManager* pManager);

	CMesh();
	bool RayCast(VECTOR4D& RayOrigin,
		VECTOR4D & RayDir,
		multimap<float, INTERSECTIONINFO>& Faces //Lista de caras que se intersectan
		);
	bool RayCast(VECTOR4D& RayOrigin,
		VECTOR4D& RayDir,
		multimap<float, unsigned long>& Vertices,float radius);
	void VertexShade(CDXPainter::VERTEX(*pVS)(CDXPainter::VERTEX V));
	void LoadSuzanne();
	void BuildTangentSpaceFromTexCoordsIndexed(void);
	void BuildTangentSpaceFromTexCoordsIndexed(bool bGenerateNormal);
	void CreateVisualNeighbors(CDXManager * m_pManager);
	void Optimize();
	void GenerarCentroides();
	
	vector<centroid>& getCentroides() { return m_Centroides; }

	/* Colission detection */ 
	unsigned long m_lID;
	BoundingBox m_Box;
	BVH* m_BVH;

	void ResetColors();
	void MoveVertex(MATRIX4D Translation);
	void CreateMeshCollisionFromVMesh(CVMesh& vMesh);
	void ApplyTransformation(MATRIX4D & m);

	~CMesh();
};

