#include "stdafx.h"
#include "BVH.h"


BVH::BVH()
{
	m_pLeft = m_pRight = NULL;
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

void BVH::Draw(CDXBasicPainter * painter, int depth)
{
	VECTOR4D c1 = m_Box.min;
	VECTOR4D c2 = m_Box.max;

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
			cube[i].Color = { 0, 1, 1, 0 };
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

	painter->DrawIndexed(cube, 8, m_lIndicesFrame, 16, PAINTER_WITH_LINESTRIP);
	
	if (m_pLeft /*&& depth != 1*/)
	{
		m_pLeft->Draw(painter, depth+1);
	}
	if (m_pRight)
	{
		m_pRight->Draw(painter, depth + 1);
	}

}
