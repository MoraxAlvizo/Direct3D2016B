#pragma once
#include "../Graphics/Matrix4D.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/DXPainter.h"

#include <set>

using namespace std;

#define OCTREECUBE_MIN_BALLS_PER_OCTREE  2
#define OCTREECUBE_MAX_OBJECTS_PER_BOX 1
#define OCTREECUBE_BOX_SIZE  20.0f

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
	set<CMesh*> m_Objects;

	// Metodos privados
	void createChildren();

	
public:

	// Atributos publicos
	VECTOR4D m_Color;
	COctreeCube(VECTOR4D max, VECTOR4D min, long depth);
	~COctreeCube();

	void addObject(CMesh * object, VECTOR4D bmin, VECTOR4D bmax);
	void removeObject(CMesh * object, VECTOR4D bmin, VECTOR4D bmax);
	void fileObject(CMesh * object, VECTOR4D bmin, VECTOR4D bmax, bool addBall);
	void DrawOctree(CDXPainter* painter);
	void destroyChildren();
	void collectObjects(set<CMesh*> &objects);
	void potentialCollsions(set<unsigned long long> &collisions);
	void printCHildren(int tab);

};

