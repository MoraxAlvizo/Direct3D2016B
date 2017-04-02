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


class CVMesh :
	public CMesh
{
public:
	/* Compute shaders MassSpring */
	static ID3D11ComputeShader*     s_pCSInitForce;
	static ID3D11ComputeShader*     s_pCSVolumePreservation;
	static ID3D11ComputeShader*     s_pCSComputeForces;
	static ID3D11ComputeShader*     s_pCSApplyForces;

	static ID3D11Buffer*			s_pCBMassSpring; // Constan buffer MassSpring.hlsl

	struct PARAMS_MASS_SPRING_CB
	{
		VECTOR4D Gravity;
		float K;
		float Delta_t;
	};

	static void CompileCSShaders(CDXManager* pManager);

	vector<EdgeCutInfo> m_EdgeCutInfo;
	vector<unsigned long> m_IndicesTetrahedros;
	vector<unsigned long> m_IndicesDibujarTetrahedros;
	vector<MassSpring> m_MassSpring;
public:
	CVMesh();
	~CVMesh();
	void LoadMSHFile(char * filename);
	vector<EdgeCutInfo>& GetEdges() { return m_EdgeCutInfo; }
	vector<CDXPainter::VERTEX>& GetVertices() { return m_Vertices; }

	// Mass Spring Systems
	void CreateNeighbors();
	void InitializaMassSpring();
	void ApplyForces(VECTOR4D Gravity, VECTOR4D ExternalForce);

	void CreateSurfaceMesh();
};

