#pragma once
#include "..\Graphics\Mesh.h"
#include <vector>
#include <set>
#include <map>
using namespace std;
struct EdgeCutInfo
{
public: // Eigen support
	unsigned int edgeId;
	VECTOR4D cutPosition;
};

struct VolumeCutInfo
{
	std::vector<EdgeCutInfo>	cutEdges;
	std::vector<unsigned int>	cutNodes;
	unsigned int				type_cut;
};

struct MassSpring
{
	VECTOR4D Velocity;
	VECTOR4D Fuerza;
	float Masa;
	set<long> vecinos;
	map<long long, float> distancia;
};

/*
struct MassSpring
{
	float4 velocity;
	float4 fuerza;
	Vecino vecinos[NUM_VECINOS];
	uint numVecinos;
	float masa;
};
*/

struct MassSpringGPU
{
	VECTOR4D velocity;
	VECTOR4D fuerza;
	struct {
		int idVecino;
		float distancia;
	}vecinos[24];
	int numVecinos;
	float masa;
};


struct Tetrahedron
{
	int indexes[4];
	VECTOR4D C;
};


class CVMesh :
	public CMesh
{
public:
	/* Compute shaders MassSpring */
	static ID3D11ComputeShader*     s_pCSInitForce;
	static ID3D11ComputeShader*     s_pCSVolumePreservation;
	static ID3D11ComputeShader*     s_pCSComputeForces;
	static ID3D11ComputeShader*     s_pCSApplyForces;
	static ID3D11ComputeShader*     s_pCSComputeNormals;

	static ID3D11Buffer*			s_pCBMassSpring; // Constan buffer MassSpring.hlsl

	struct PARAMS_MASS_SPRING_CB
	{
		VECTOR4D Gravity;
		float Ki;
		float Delta_t;
	}m_CB_MASS_SPRING;

	/* Compute shaders buffer */
	ID3D11Buffer* m_pMassSpringBuffer;
	ID3D11Buffer* m_pTetraIndices;

	// UAV For vertex buffer  and primbuffer
	ID3D11UnorderedAccessView* m_pUAVMassSpringBuffer;
	ID3D11UnorderedAccessView* m_pUAVTetraIndices;

	// SRV For vertex buffer and index buffer
	ID3D11ShaderResourceView* m_pSRVMassSpringBuffer;
	ID3D11ShaderResourceView* m_pSRVTetraIndices;

	static void CompileCSShaders(CDXManager* pManager);
	void CreateTetraindexAndMassSpringBuffers(CDXManager* m_pManager);

	vector<EdgeCutInfo> m_EdgeCutInfo;
	vector<Tetrahedron> m_IndicesTetrahedros;
	vector<unsigned long> m_IndicesDibujarTetrahedros;
	vector<MassSpring> m_MassSpring;
	vector<MassSpringGPU> m_MassSpringGPU;
	vector<VECTOR4D> m_CollisionForces;
public:
	CVMesh();
	~CVMesh();
	void LoadMSHFile(char * filename);
	vector<EdgeCutInfo>& GetEdges() { return m_EdgeCutInfo; }
	vector<CDXPainter::VERTEX>& GetVertices() { return m_Vertices; }
	void ResetBufferCollisionForces() { memset(&m_CollisionForces[0], 0, m_CollisionForces.size() * sizeof(VECTOR4D)); }

	// Mass Spring Systems
	void CreateNeighbors();
	void InitializaMassSpring();
	void ApplyForces(VECTOR4D Gravity, VECTOR4D ExternalForce);
	void CSApplyForces(CDXManager * pManager, VECTOR4D Gravity, VECTOR4D ExternalForce);

	void CreateSurfaceMesh();
};

