#include "stdafx.h"
#include "BVH.h"

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
			cube[i].Color = m_Color;
		else
			cube[i].Color = { 1, 1, 0, 0 };
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
	
	if (m_pLeft /*&& depth != 1*/)
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
