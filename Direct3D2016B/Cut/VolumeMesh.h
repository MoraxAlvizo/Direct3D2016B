#pragma once
#include <vector>
#include"..\Graphics\Matrix4D.h"
#include "..\Graphics\Mesh.h"
#include "VolumeMeshGenericID.h"
#include "..\Graphics\DXBasicPainter.h"

typedef CDXBasicPainter::VERTEX Vertex;
using namespace std;
class CVolumeMesh : public CMesh
{
public:
	CVolumeMesh();
	~CVolumeMesh();
public:
	VCL_MESH_VOLUMEMESH_CREATEID(VertexId);		// Size: n0
	VCL_MESH_VOLUMEMESH_CREATEID(EdgeId);
	VCL_MESH_VOLUMEMESH_CREATEID(VolumeId);		// Size: n3

public: /* Temporary. To be removed */
	vector<CDXBasicPainter::VERTEX>& Vertices() { return mVertices; }
	vector<Edge>& Edges() { return mEdges; }
	vector<Volume>& Volumes() { return mVolumes; }
	VECTOR4D& getPoint(VertexId id);

protected:
	vector<Vertex> mVertices;
	vector<Edge> mEdges;
	vector<Volume> mVolumes;
};

