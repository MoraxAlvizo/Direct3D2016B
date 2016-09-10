#pragma once
#include "Matrix4D.h"
#include "MeshCollision.h"
#include "DXBasicPainter.h"

#define OCTREECUBE_MAX_OBJECTS_PER_BOX 2

class COctreeCube
{
private:
	struct Box
	{
		VECTOR4D min;
		VECTOR4D max;
		VECTOR4D center;
	};

	// Atributos privados
	Box m_Box;
	long m_lDepth;
	COctreeCube *m_Children[2][2][2];
	bool hasChildren;
	long numObjects;
	set<CMeshCollision*> m_Objects;

	// Metodos privados
	void createChildren();

	
public:

	// Atributos publicos
	VECTOR4D m_Color;
	COctreeCube(VECTOR4D max, VECTOR4D min, long depth);
	~COctreeCube();

	void addObject(CMeshCollision * object, VECTOR4D bmin, VECTOR4D bmax);
	void fileObject(CMeshCollision * object, VECTOR4D bmin, VECTOR4D bmax, bool addBall);
	void DrawOctree(CDXBasicPainter* painter);

	void printCHildren(int tab);

};

