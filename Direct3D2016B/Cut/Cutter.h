#pragma once
#include"..\Graphics\Matrix4D.h"
#include <vector>
#include"VolumeMeshGenericID.h"
class CCutter
{
public:
	VCL_MESH_VOLUMEMESH_CREATEID(VertexId);		// Size: n0
	VCL_MESH_VOLUMEMESH_CREATEID(EdgeId);
	VCL_MESH_VOLUMEMESH_CREATEID(VolumeId);		// Size: n3
	enum EdgeState { UNTESTED, CUT, UNCUT };
	CCutter();
	~CCutter();
public:
	void cut3(std::vector<VECTOR4D> &cuttingPoints, size_t &addedNodes);
private:
	int cutEdgesCount = 0;
	int cutNodesCount = 0;
	int intersectionTestCount = 0;

protected:
	//bool testATypeCut(VolumeId vol) const;
	void processCutInfo(size_t &addedNodes);
	void addCutEdge(VolumeId volId, EdgeId edgeId, VECTOR4D &p);
	void addCutNode(VolumeId volId, VertexId nodeId);
};

