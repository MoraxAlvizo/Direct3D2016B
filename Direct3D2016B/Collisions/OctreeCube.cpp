#include "stdafx.h"
#include "OctreeCube.h"


COctreeCube::COctreeCube(VECTOR4D min, VECTOR4D max, long depth)
{
	m_Box.max = max;
	m_Box.min = min;
	m_lDepth = depth;
	m_Box.center = (max + min) / 2.0f;
	numObjects = 0;
	m_Color = { 1,0,0,0 };
}


void COctreeCube::createChildren()
{
	for (int x = 0; x < 2; x++) {
		float minX;
		float maxX;
		if (x == 0) {
			minX = m_Box.min.v[0];
			maxX = m_Box.center.v[0];
		}
		else {
			minX = m_Box.center.v[0];
			maxX = m_Box.max.v[0];
		}

		for (int y = 0; y < 2; y++) {
			float minY;
			float maxY;
			if (y == 0) {
				minY = m_Box.min.v[1];
				maxY = m_Box.center.v[1];
			}
			else {
				minY = m_Box.center.v[1];
				maxY = m_Box.max.v[1];
			}

			for (int z = 0; z < 2; z++) {
				float minZ;
				float maxZ;
				if (z == 0) {
					minZ = m_Box.min.v[2];
					maxZ = m_Box.center.v[2];
				}
				else {
					minZ = m_Box.center.v[2];
					maxZ = m_Box.max.v[2];
				}
				//Crea un nuevo octree y la profundidad se incrementa en 1.
				m_Children[x][y][z] = new COctreeCube({ minX, minY, minZ ,1 }, { maxX, maxY, maxZ },
					m_lDepth + 1);
			}
		}
	}

	// Itera sobre todas las bolas del antiguo nodo y las va colocando en los nuevos hijos con fileBall(). 
	for (set<CMeshCollision*>::iterator it = m_Objects.begin(); it != m_Objects.end();
		it++) {
		CMeshCollision* object = *it;
		fileObject(object, object->m_Box.min * object->m_World, object->m_Box.max* object->m_World, true);
	}
	// Elimina todas las bolas de ese nodo en particula ya que fueron colocadas en sus hijos.
	m_Objects.clear();
	hasChildren = true;
}

COctreeCube::~COctreeCube()
{
}

inline void COctreeCube::addObject(CMeshCollision * object, VECTOR4D bmin, VECTOR4D bmax)
{
	numObjects++;

	if (!hasChildren &&
		numObjects > OCTREECUBE_MAX_OBJECTS_PER_BOX &&
		m_lDepth < 4) {
		createChildren();
	}

	if (hasChildren)
		fileObject(object, bmin,bmax,true);
	else
		m_Objects.insert(object);

}

void COctreeCube::fileObject(CMeshCollision* object, VECTOR4D bmin, VECTOR4D bmax, bool addBall)
{

	for (int x = 0; x < 2; x++) {
		if (x == 0) {
			if (bmin.v[0]  > m_Box.center.v[0] &&
				bmax.v[0]  > m_Box.center.v[0] ) {
				continue;
			}
		}
		else if (bmin.v[0]  < m_Box.center.v[0] &&
				 bmax.v[0]  < m_Box.center.v[0]) {
			continue;
		}

		for (int y = 0; y < 2; y++) {
			if (y == 0) {
				if (bmax.v[1] > m_Box.center.v[1] &&
					bmin.v[1] > m_Box.center.v[1]) {
					continue;
				}
			}
			else if (bmin.v[1] < m_Box.center.v[1] &&
					 bmax.v[1] < m_Box.center.v[1]) {
				continue;
			}

			for (int z = 0; z < 2; z++) {
				if (z == 0) {
					if (bmin.v[2]  > m_Box.center.v[2] &&
						bmax.v[2]  > m_Box.center.v[2]) {
						continue;
					}
				}
				else if (bmin.v[2] < m_Box.center.v[2] &&
						 bmax.v[2] < m_Box.center.v[2]){
					continue;
				}

				// Agrega o elimina la pelota.
				if (addBall) {
					m_Children[x][y][z]->addObject(object, bmin, bmax);
				}
				/*else {
					m_Children[x][y][z]->remove(point, pos);
				}*/
			}
		}
	}
}

void COctreeCube::DrawOctree(CDXBasicPainter * painter)
{
	VECTOR4D c1 = m_Box.min;
	VECTOR4D c2 = m_Box.max;

	CDXBasicPainter::VERTEX cube[8];
	unsigned long   m_lIndicesFrame[16];
	cube[0].Position = { c1.x,c1.y,c1.z,1 };
	cube[1].Position = { c1.x,c2.y,c1.z,1 };
	cube[2].Position = { c2.x,c1.y,c1.z,1 };
	cube[3].Position = { c2.x,c2.y,c1.z,1 };
	cube[4].Position = { c1.x,c1.y,c2.z,1 };
	cube[5].Position = { c1.x,c2.y,c2.z,1 };
	cube[6].Position = { c2.x,c1.y,c2.z,1 };
	cube[7].Position = { c2.x,c2.y,c2.z,1 };

	for (int i = 0; i < 8; i++)
		cube[i].Color = m_Color;

	m_lIndicesFrame[0] = 0;
	m_lIndicesFrame[1] = 1;
	m_lIndicesFrame[2] = 3;
	m_lIndicesFrame[3] = 2;
	m_lIndicesFrame[4] = 0;
	m_lIndicesFrame[5] = 4;
	m_lIndicesFrame[6] = 5;
	m_lIndicesFrame[7] = 1;
	m_lIndicesFrame[8] = 5;
	m_lIndicesFrame[9] = 7;
	m_lIndicesFrame[10] = 3;
	m_lIndicesFrame[11] = 2;
	m_lIndicesFrame[12] = 6;
	m_lIndicesFrame[13] = 7;
	m_lIndicesFrame[14] = 6;
	m_lIndicesFrame[15] = 4;

	painter->DrawIndexed(cube, 8, m_lIndicesFrame, 16, PAINTER_WITH_LINESTRIP);

	if (hasChildren)
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					m_Children[i][j][k]->DrawOctree(painter);
				}
			}
		}
}

// Nos ayuda a debuguear la creacion del arbol
void COctreeCube::printCHildren(int tab)
{
	for (int i = 0; i < tab; i++)printf(" ");

	
	printf( "%i, E1: [%.2f][%.2f][%.2f], E2: [%.2f][%.2f][%.2f]\n", numObjects, m_Box.min.v[0], m_Box.min.v[1], m_Box.min.v[2], m_Box.max.v[0], m_Box.max.v[1], m_Box.max.v[2]);
	int counter = 0;
	for (auto cube : this->m_Objects)
	{
		for (int i = 0; i < tab + 2; i++)printf(" ");
		printf("Object name %s \n", cube->m_cName);
		for (int i = 0; i < tab+4; i++)printf(" ");
		VECTOR4D max = cube->m_Box.max * cube->m_World;
		VECTOR4D min = cube->m_Box.min * cube->m_World;

		printf("Object[%i] min: [%.2f][%.2f][%.2f]\n", counter, min.x, min.y, min.z);
		for (int i = 0; i < tab+4; i++)printf(" ");
		printf("Object[%i] max: [%.2f][%.2f][%.2f]\n", counter, max.x, max.y, max.z);
		counter++;
	}
	if (hasChildren)
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					m_Children[i][j][k]->printCHildren(tab + 2);
				}
			}
		}

}