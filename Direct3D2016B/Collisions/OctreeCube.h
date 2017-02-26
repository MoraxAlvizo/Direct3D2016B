#pragma once
#include "../Graphics/Matrix4D.h"
#include "../Collisions/MeshCollision.h"
#include "../Graphics/DXPainter.h"

#define OCTREECUBE_MAX_OBJECTS_PER_BOX 1

class COctreeCube
{
public:
	struct Box
	{
		VECTOR4D min;
		VECTOR4D max;
		VECTOR4D center;
	};

	union MeshPair
	{
		unsigned long long m_idColision;
		struct
		{
			unsigned long m_object1ID;
			unsigned long m_object2ID;
		};
	};
private:
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
	void removeObject(CMeshCollision * object, VECTOR4D bmin, VECTOR4D bmax);
	void fileObject(CMeshCollision * object, VECTOR4D bmin, VECTOR4D bmax, bool addBall);
	void DrawOctree(CDXPainter* painter);
	void destroyChildren();
	void collectObjects(set<CMeshCollision*> &objects);
	void potentialCollsions(set<unsigned long long> &collisions);
	void printCHildren(int tab);

};

