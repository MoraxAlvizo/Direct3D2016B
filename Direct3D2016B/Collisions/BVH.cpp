#include "stdafx.h"
#include "BVH.h"
#include "../Collisions/MeshCollision.h"

#define BVH_BOXES_COLLISION(box1, box2) \
	((box1.min.x < box2.max.x && \
	  box1.max.x > box2.min.x && \
	  box1.min.y < box2.max.y && \
	  box1.max.y > box2.min.y && \
	  box1.min.z < box2.max.z && \
	  box1.max.z > box2.min.z))

#define BVH_IS_LEFT(pBVH) (!(pBVH)->m_pLeft && !(pBVH)->m_pRight)

BVH::BVH()
{
	m_pLeft = m_pRight = NULL;
	m_Color = { 0, 1, 1, 0 };
	LBVH.resize(BVH_NUM_NODES);
}


BVH::~BVH()
{
}

void BVH::Build(CMesh & object, vector<unsigned long> Primitives)
{

	/* Case base */
	if (Primitives.size() == 1)
	{
		m_Box.max = object.m_Centroides[Primitives[0]].max ;
		m_Box.min = object.m_Centroides[Primitives[0]].min ;
		m_Box.idPrimitive = Primitives[0];
		return;
	}

	VECTOR4D max_box = SET_MIN_VEC4D, min_box = SET_MAX_VEC4D;
	/* 1. Check max and min in all primitives */
	for (unsigned long i = 0; i < Primitives.size(); i++)
	{
		max_box = MAX_VECTOR4D(max_box, object.m_Centroides[Primitives[i]].position);
		min_box = MIN_VECTOR4D(min_box, object.m_Centroides[Primitives[i]].position);
	}

	/* 2. Check what axis is longer */
	float size_x = max_box.x - min_box.x;
	float size_y = max_box.y - min_box.y;
	float size_z = max_box.z - min_box.z;

	int axis = -1;
	if (size_x > size_y)
	{
		if (size_x > size_z)
			axis = 0;
		else
			axis = 2;
	}
	else
	{
		if (size_y > size_z)
			axis = 1;
		else
			axis = 2;
	}


	vector<unsigned long> primitivesLeft, primitivesRight;
	/* Add primitives to corresponding box */
	float middle = (max_box.v[axis] + min_box.v[axis]) / 2;
	for (unsigned long i = 0; i < Primitives.size(); i++)
	{
		VECTOR4D prim = object.m_Centroides[Primitives[i]].position;
		if (prim.v[axis] > middle)
			primitivesLeft.push_back(Primitives[i]);
		else
			primitivesRight.push_back(Primitives[i]);
	}

	/* Build boxes indenpendly*/
	if (primitivesLeft.size() > 0)
	{
		m_pLeft = new BVH();
		m_pLeft->Build(object, primitivesLeft);
	}
	if (primitivesRight.size() > 0)
	{
		m_pRight = new BVH();
		m_pRight->Build(object, primitivesRight);
	}

	if (m_pLeft && m_pRight)
	{
		this->m_Box.max = MAX_VECTOR4D(m_pLeft->m_Box.max, m_pRight->m_Box.max);
		this->m_Box.min = MIN_VECTOR4D(m_pLeft->m_Box.min, m_pRight->m_Box.min);
	}
	else if (m_pLeft)
	{
		this->m_Box.max = m_pLeft->m_Box.max;
		this->m_Box.min = m_pLeft->m_Box.min;
	}
	else if(m_pRight)
	{
		this->m_Box.max = m_pRight->m_Box.max;
		this->m_Box.min = m_pRight->m_Box.min;
	}

}

void BVH::Draw(CDXBasicPainter * painter, int depth, MATRIX4D translation)
{
	VECTOR4D c1 = m_Box.min * translation ;
	VECTOR4D c2 = m_Box.max * translation ;

	/*c1.x += 0.01;
	c1.y += 0.01;
	c1.z += 0.01;

	c2.x -= 0.01;
	c2.y -= 0.01;
	c2.z -= 0.01;*/

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
	{
		if (!m_pLeft && !m_pRight)
			cube[i].Color = { 0, 0, 0, 0 };
		else
			cube[i].Color = { 0, 0, 0, 0 };
	}

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

	if(!m_pLeft && !m_pRight)
		painter->DrawIndexed(cube, 8, m_lIndicesFrame, 16, PAINTER_WITH_LINESTRIP);

	if (m_pLeft)
	{
		m_pLeft->Draw(painter, depth+1, translation);
	}
	if (m_pRight)
	{
		m_pRight->Draw(painter, depth + 1, translation);
	}
}


#define BVH_SET_COLOR(object, indice, color) \
{ \
	(object).m_Vertices[(object).m_Indices[indice]].Color = color; \
	(object).m_Vertices[(object).m_Indices[indice + 1]].Color = color; \
	(object).m_Vertices[(object).m_Indices[indice + 2]].Color = color; \
}

void BVH::Traversal(BVH * pTree, MATRIX4D& thisTranslation,MATRIX4D& translationTree, CMesh& object1, CMesh& object2)
{
	Box thisBox;
	Box treeBox;

	thisBox.max = this->m_Box.max * thisTranslation;
	thisBox.min = this->m_Box.min * thisTranslation;

	treeBox.max = pTree->m_Box.max * translationTree;
	treeBox.min = pTree->m_Box.min * translationTree;

	if (pTree == NULL)
		return;

	if (BVH_BOXES_COLLISION(thisBox, treeBox))
	{
		if (BVH_IS_LEFT(this))
		{
			/* Si ambos arboles son nodos hojas, entonces revisar los triangulos */
			if (BVH_IS_LEFT(pTree))
			{
				// Crear funcion de revisar triangulos

				unsigned long indicesThis = m_Box.idPrimitive * 3;

				VECTOR4D object1_V0 = object1.m_Vertices[object1.m_Indices[indicesThis]].Position * object1.m_World;
				VECTOR4D object1_V1 = object1.m_Vertices[object1.m_Indices[indicesThis +1]].Position* object1.m_World;
				VECTOR4D object1_V2 = object1.m_Vertices[object1.m_Indices[indicesThis +2]].Position* object1.m_World;

				unsigned long indicesPTree = pTree->m_Box.idPrimitive * 3;

				VECTOR4D object2_V0 = object2.m_Vertices[object2.m_Indices[indicesPTree]].Position* object2.m_World;
				VECTOR4D object2_V1 = object2.m_Vertices[object2.m_Indices[indicesPTree + 1]].Position* object2.m_World;
				VECTOR4D object2_V2 = object2.m_Vertices[object2.m_Indices[indicesPTree + 2]].Position* object2.m_World;

				VECTOR4D Intersection;
				VECTOR4D RayOrigin;
				VECTOR4D RayDir;

				/* Revisar triangulo objeto1 contra el objeto2 */

				RayOrigin = object2_V0;
				RayDir = Normalize( object2_V1 - RayOrigin);

				if (RayCastOnTriangle(object1_V0, object1_V1, object1_V2, RayOrigin, RayDir, Intersection))
				{
					this->m_Color = { 1, 1, 0, 0 };
					pTree->m_Color = { 1, 1, 0, 0 };
					BVH_SET_COLOR(object1, indicesThis, m_Color);
					BVH_SET_COLOR(object2, indicesPTree, m_Color);

				}
				RayOrigin = object2_V1;
				RayDir = Normalize(object2_V2 - RayOrigin);

				if (RayCastOnTriangle(object1_V0, object1_V1, object1_V2, RayOrigin, RayDir, Intersection))
				{
					this->m_Color = { 1, 1, 0, 0 };
					pTree->m_Color = { 1, 1, 0, 0 };
					BVH_SET_COLOR(object1, indicesThis, m_Color);
					BVH_SET_COLOR(object2, indicesPTree, m_Color);
				}

				RayOrigin = object2_V2;
				RayDir = Normalize(object2_V0 - RayOrigin);

				if (RayCastOnTriangle(object1_V0, object1_V1, object1_V2, RayOrigin, RayDir, Intersection))
				{
					this->m_Color = { 1, 1, 0, 0 };
					pTree->m_Color = { 1, 1, 0, 0 };
					BVH_SET_COLOR(object1, indicesThis, m_Color);
					BVH_SET_COLOR(object2, indicesPTree, m_Color);
				}

				/* Revisar triangulo objeto2 contra el objeto1 */
				RayOrigin = object1_V0;
				RayDir = Normalize(object1_V1 - RayOrigin);

				if (RayCastOnTriangle(object2_V0, object2_V1, object2_V2, RayOrigin, RayDir, Intersection))
				{
					this->m_Color = { 1, 1, 0, 0 };
					pTree->m_Color = { 1, 1, 0, 0 };
					BVH_SET_COLOR(object1, indicesThis, m_Color);
					BVH_SET_COLOR(object2, indicesPTree, m_Color);
				}
				RayOrigin = object1_V1;
				RayDir = Normalize(object1_V2 - RayOrigin);

				if (RayCastOnTriangle(object2_V0, object2_V1, object2_V2, RayOrigin, RayDir, Intersection))
				{
					this->m_Color = { 1, 1, 0, 0 };
					pTree->m_Color = { 1, 1, 0, 0 };
					BVH_SET_COLOR(object1, indicesThis, m_Color);
					BVH_SET_COLOR(object2, indicesPTree, m_Color);
				}

				RayOrigin = object1_V2;
				RayDir = Normalize(object1_V0 - RayOrigin);

				if (RayCastOnTriangle(object2_V0, object2_V1, object2_V2, RayOrigin, RayDir, Intersection))
				{
					this->m_Color = { 1, 1, 0, 0 };
					pTree->m_Color = { 1, 1, 0, 0 };
					BVH_SET_COLOR(object1, indicesThis, m_Color);
					BVH_SET_COLOR(object2, indicesPTree, m_Color);
				}
			}
			else
			{
				/* Sino es nodo hoja el pTree entonces revisar si sus hijos collision con este nodo hoja */
				if(pTree->m_pLeft)
					Traversal(pTree->m_pLeft, thisTranslation, translationTree, object1, object2);
				if(pTree->m_pRight)
					Traversal(pTree->m_pRight, thisTranslation, translationTree, object1, object2);
			}
		}
		/*

		Son 4 casos para saber por donde recorrer el arbol:
			1. this->left vs pTree->left.
			2. this->left vs pTree->right.
			3. this->right vs pTree->left.
			4. this->right vs pTree->right
		*/

		if (m_pLeft != NULL)
		{
			if(pTree->m_pLeft != NULL)
				this->m_pLeft->Traversal(pTree->m_pLeft, thisTranslation, translationTree, object1, object2);
			if(pTree->m_pRight != NULL)
				this->m_pLeft->Traversal(pTree->m_pRight, thisTranslation, translationTree, object1, object2);
		}
		if (m_pRight != NULL)
		{
			if (pTree->m_pLeft != NULL)
				this->m_pRight->Traversal(pTree->m_pLeft, thisTranslation, translationTree, object1, object2);
			if(pTree -> m_pRight != NULL)
				this->m_pRight->Traversal(pTree->m_pRight, thisTranslation, translationTree, object1, object2);
		}
	}
}

void BVH::Preconstruction(CMesh & object)
{
	object.GenerarCentroides();
}

void BVH::Construction(CMesh & object, unsigned long node, vector<unsigned long> Primitives)
{
	/* Case base */
	if (Primitives.size() == 1)
	{
		LBVH[node].numPrimitives = 1;
		LBVH[node].idPrimitive = Primitives[0];
		LBVH[node].isLeaf = true;
		return;
	}

	VECTOR4D max_box = SET_MIN_VEC4D, min_box = SET_MAX_VEC4D;

	/* 0. Assing num of primitives */
	LBVH[node].numPrimitives = Primitives.size();

	/* 1. Check max and min in all primitives */
	for (unsigned long i = 0; i < Primitives.size(); i++)
	{
		max_box = MAX_VECTOR4D(max_box, object.m_Centroides[Primitives[i]].position);
		min_box = MIN_VECTOR4D(min_box, object.m_Centroides[Primitives[i]].position);
	}

	if (node == 1)
	{
		CMeshCollision * col = (CMeshCollision*)&object;
		col->m_Box.max = max_box;
		col->m_Box.min = min_box;
	}

	/* 2. Check what axis is longer */
	float size_x = max_box.x - min_box.x;
	float size_y = max_box.y - min_box.y;
	float size_z = max_box.z - min_box.z;

	int axis = -1;
	if (size_x > size_y)
	{
		if (size_x > size_z)
			axis = 0;
		else
			axis = 2;
	}
	else
	{
		if (size_y > size_z)
			axis = 1;
		else
			axis = 2;
	}


	vector<unsigned long> primitivesLeft, primitivesRight;
	/* Add primitives to corresponding box */
	float middle = (max_box.v[axis] + min_box.v[axis]) / 2;
	for (unsigned long i = 0; i < Primitives.size(); i++)
	{
		VECTOR4D prim = object.m_Centroides[Primitives[i]].position;
		if (prim.v[axis] > middle)
			primitivesLeft.push_back(Primitives[i]);
		else
			primitivesRight.push_back(Primitives[i]);
	}

	/* Build boxes indenpendly*/
	if (primitivesLeft.size() > 0)
		Construction(object, node << 1, primitivesLeft);
	else
	{
		LBVH[node << 1].numPrimitives = -1;
		LBVH[node << 1].max = SET_MIN_VEC4D;
		LBVH[node << 1].min = SET_MAX_VEC4D;
	}


	if (primitivesRight.size() > 0)
		Construction(object, (node << 1) + 1, primitivesRight);
	else
	{
		LBVH[(node << 1) + 1].numPrimitives = -1;
		LBVH[(node << 1) + 1].max = SET_MIN_VEC4D;
		LBVH[(node << 1) + 1].min = SET_MAX_VEC4D;
	}

}

void BVH::Postconstruction(CMesh & object)
{
	for (long i = BVH_MAX_LEVEL - 1; i >= 0; i--)
	{
		unsigned long offset = 1 << i;
		unsigned long lastNode = 1 << i;
		for (unsigned long j = 0; j < lastNode; j++)
		{
			int index = offset + j;
			/* if it is invalid, skip */
			if (LBVH[index].numPrimitives <= 0)
				continue;

			if (LBVH[index].isLeaf)
			{
				LBVH[index].max = object.m_Centroides[LBVH[index].idPrimitive].max;
				LBVH[index].min = object.m_Centroides[LBVH[index].idPrimitive].min;
			}
			else
			{
				LBVH[index].max = MAX_VECTOR4D(LBVH[index << 1].max, LBVH[(index << 1) + 1].max);
				LBVH[index].min = MIN_VECTOR4D(LBVH[index << 1].min, LBVH[(index << 1) + 1].min);
			}
		}
	}
}

bool BVH::CheckIfPrimitivesCollision(BVH * pTree,
	unsigned long nodeThis,
	unsigned long nodeTree,
	CMesh& object1,
	CMesh& object2)
{
	// Crear funcion de revisar triangulos

	unsigned long indicesThis = this->LBVH[nodeThis].idPrimitive * 3;

	VECTOR4D object1_V0 = object1.m_Vertices[object1.m_Indices[indicesThis]].Position * object1.m_World;
	VECTOR4D object1_V1 = object1.m_Vertices[object1.m_Indices[indicesThis + 1]].Position* object1.m_World;
	VECTOR4D object1_V2 = object1.m_Vertices[object1.m_Indices[indicesThis + 2]].Position* object1.m_World;

	unsigned long indicesPTree = pTree->LBVH[nodeTree].idPrimitive * 3;

	VECTOR4D object2_V0 = object2.m_Vertices[object2.m_Indices[indicesPTree]].Position* object2.m_World;
	VECTOR4D object2_V1 = object2.m_Vertices[object2.m_Indices[indicesPTree + 1]].Position* object2.m_World;
	VECTOR4D object2_V2 = object2.m_Vertices[object2.m_Indices[indicesPTree + 2]].Position* object2.m_World;

	VECTOR4D Intersection;
	VECTOR4D RayOrigin;
	VECTOR4D RayDir;

	/* Revisar triangulo objeto1 contra el objeto2 */

	RayOrigin = object2_V0;
	RayDir = Normalize(object2_V1 - RayOrigin);

	if (RayCastOnTriangle(object1_V0, object1_V1, object1_V2, RayOrigin, RayDir, Intersection))
		return true;

	RayOrigin = object2_V1;
	RayDir = Normalize(object2_V2 - RayOrigin);

	if (RayCastOnTriangle(object1_V0, object1_V1, object1_V2, RayOrigin, RayDir, Intersection))
		return true;

	RayOrigin = object2_V2;
	RayDir = Normalize(object2_V0 - RayOrigin);

	if (RayCastOnTriangle(object1_V0, object1_V1, object1_V2, RayOrigin, RayDir, Intersection))
		return true;

	/* Revisar triangulo objeto2 contra el objeto1 */
	RayOrigin = object1_V0;
	RayDir = Normalize(object1_V1 - RayOrigin);

	if (RayCastOnTriangle(object2_V0, object2_V1, object2_V2, RayOrigin, RayDir, Intersection))
		return true;

	RayOrigin = object1_V1;
	RayDir = Normalize(object1_V2 - RayOrigin);

	if (RayCastOnTriangle(object2_V0, object2_V1, object2_V2, RayOrigin, RayDir, Intersection))
		return true;

	RayOrigin = object1_V2;
	RayDir = Normalize(object1_V0 - RayOrigin);

	if (RayCastOnTriangle(object2_V0, object2_V1, object2_V2, RayOrigin, RayDir, Intersection))
		return true;

	return false;
}

void BVH::DrawLBVH(CDXBasicPainter * painter, int node, MATRIX4D translation)
{
	/* Caso base */
	if (LBVH[node].numPrimitives <= 0)
		return;

	VECTOR4D c1 = LBVH[node].min * translation;
	VECTOR4D c2 = LBVH[node].max * translation;

	/*c1.x += 0.01;
	c1.y += 0.01;
	c1.z += 0.01;

	c2.x -= 0.01;
	c2.y -= 0.01;
	c2.z -= 0.01;*/

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
	{
		/*if (!m_pLeft && !m_pRight)
			cub
			e[i].Color = m_Color;
		else*/
		cube[i].Color = m_Color;// { 0, 0, 0, 0 };
	}

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

	//if (LBVH[node].isLeaf)
		painter->DrawIndexed(cube, 8, m_lIndicesFrame, 16, PAINTER_WITH_LINESTRIP);

	//DrawLBVH(painter, node << 1, translation);
	//DrawLBVH(painter, (node << 1)+1, translation);

}

void BVH::TraversalLBVH(
	BVH * pTree,
	unsigned long nodeThis,
	unsigned long nodeTree,
	MATRIX4D & thisTranslation,
	MATRIX4D & translationTree,
	CMesh & object1,
	CMesh & object2)
{
	Box thisBox;
	Box treeBox;

	thisBox.max = this->LBVH[nodeThis].max * thisTranslation;
	thisBox.min = this->LBVH[nodeThis].min * thisTranslation;

	treeBox.max = pTree->LBVH[nodeTree].max * translationTree;
	treeBox.min = pTree->LBVH[nodeTree].min * translationTree;

	if (pTree->LBVH[nodeTree].numPrimitives <= 0 ||
		this->LBVH[nodeThis].numPrimitives <= 0)
		return;

	if (BVH_BOXES_COLLISION(thisBox, treeBox))
	{
		if (this->LBVH[nodeThis].isLeaf)
		{
			/* Si ambos arboles son nodos hojas, entonces revisar los triangulos */
			if (pTree->LBVH[nodeTree].isLeaf)
			{
				// Crear funcion de revisar triangulos
				if (CheckIfPrimitivesCollision(pTree, nodeThis, nodeTree, object1, object2))
				{
					unsigned long indicesThis = this->LBVH[nodeThis].idPrimitive * 3;
					unsigned long indicesPTree = pTree->LBVH[nodeTree].idPrimitive * 3;

					this->m_Color = { 0, 0, 1, 0 };
					pTree->m_Color = { 0, 0, 1, 0 };
					BVH_SET_COLOR(object1, indicesThis, m_Color);
					BVH_SET_COLOR(object2, indicesPTree, m_Color);
				}
			}
			else
			{
				/* Sino es nodo hoja el pTree entonces revisar si sus hijos collision con este nodo hoja */
				TraversalLBVH(pTree, nodeThis, nodeTree << 1, thisTranslation, translationTree, object1, object2);
				TraversalLBVH(pTree, nodeThis, (nodeTree << 1) + 1, thisTranslation, translationTree, object1, object2);
			}
		}
		/*

		Son 4 casos para saber por donde recorrer el arbol:
		1. this->left vs pTree->left.
		2. this->left vs pTree->right.
		3. this->right vs pTree->left.
		4. this->right vs pTree->right
		*/

		TraversalLBVH(pTree, nodeThis << 1, nodeTree << 1, thisTranslation, translationTree, object1, object2);
		TraversalLBVH(pTree, nodeThis << 1, (nodeTree << 1) + 1, thisTranslation, translationTree, object1, object2);
		TraversalLBVH(pTree, (nodeThis << 1) + 1, nodeTree << 1, thisTranslation, translationTree, object1, object2);
		TraversalLBVH(pTree, (nodeThis << 1) + 1, (nodeTree << 1) + 1, thisTranslation, translationTree, object1, object2);
	}
}

int firstbitlow(int num)
{
	for (int i = 0; i < 32; i++)
	{
		if (num & (1 << i))
			return 1 << i;
	}
	return 0;

}

int firstbithigh(int num)
{
	for (int i = 31; i >= 0; i--)
	{
		if (num & (1 << i))
			return 1 << i;
	}
	return 0;

}

void BVH::BitTrailTraversal(BVH * pTree, MATRIX4D & thisTranslation, MATRIX4D & translationTree, CMesh & object1, CMesh & object2)
{

	int iTriangleId = -1;

	int nodeNum = 2;
	//int trail = (1 << (firstbithigh(nodeNum)));
	int nodeNum2 = 1;
	int trail2 = (1 << (firstbithigh(nodeNum2)));


	while (true)
	{
		int p2 = 0;

		while (!pTree->LBVH[nodeNum2].isLeaf)
		{
			Box thisBox;
			Box treeBox;
			p2 = firstbitlow(trail2 + 1);

			thisBox.max = this->LBVH[nodeNum].max * thisTranslation;
			thisBox.min = this->LBVH[nodeNum].min * thisTranslation;

			treeBox.max = pTree->LBVH[nodeNum2].max * translationTree;
			treeBox.min = pTree->LBVH[nodeNum2].min * translationTree;


			if (!BVH_BOXES_COLLISION(thisBox, treeBox))
			{
				//does not intersect, change to next node
				trail2 = (trail2 >> p2) + 1;
				nodeNum2 = (nodeNum2 >> p2) ^ 1;

			}
			else
			{
				trail2 = trail2 << 1;
				nodeNum2 = (nodeNum2 << 1);

			}
			if ( trail2 <= 1) break;
		}

		if (trail2 <= 1) break;

		p2 = firstbitlow(trail2 + 1);

		//it is a leaf, compare against all the primitives and asign next node
		if (CheckIfPrimitivesCollision(pTree, nodeNum, nodeNum2, object1, object2))
		{
			unsigned long indicesThis = this->LBVH[nodeNum].idPrimitive * 3;
			unsigned long indicesPTree = pTree->LBVH[nodeNum2].idPrimitive * 3;

			this->m_Color = { 1, 1, 0, 0 };
			pTree->m_Color = { 1, 1, 0, 0 };
			BVH_SET_COLOR(object1, indicesThis, m_Color);
			BVH_SET_COLOR(object2, indicesPTree, m_Color);
		}


		trail2 = (trail2 >> p2) + 1;
		nodeNum2 = (nodeNum2 >> p2) ^ 1;

		if (trail2 <=1 ) break;
	}
}
