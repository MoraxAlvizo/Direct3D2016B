#pragma once
#include "Graphics\Mesh.h"
#include <vector>

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
class CVMesh :
	public CMesh
{
public:
	vector<EdgeCutInfo> m_EdgeCutInfo;
	vector<unsigned long> m_IndicesTetrahedros;
public:
	CVMesh();
	~CVMesh();
	void LoadMSHFile(char * filename);
	vector<EdgeCutInfo>& GetEdges() { return m_EdgeCutInfo; }
	vector<CDXBasicPainter::VERTEX>& GetVertices() { return m_Vertices; }
};

