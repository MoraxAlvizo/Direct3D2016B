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
	VECTOR4D x0;
	VECTOR4D Velocity;
	VECTOR4D Fuerza;
	float Masa;
	set<long> vecinos;
	map<long long, float> distancia;
};


class CVMesh :
	public CMesh
{
public:
	vector<EdgeCutInfo> m_EdgeCutInfo;
	vector<unsigned long> m_IndicesTetrahedros;
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
	void ApplyForces(VECTOR4D Gravity);
};

