#pragma once
#include"..\Graphics\DXBasicPainter.h"
class CPlane
{

public:
	CPlane(VECTOR4D &p1v, VECTOR4D &p2v, VECTOR4D &p3v);
	~CPlane();
public:
	bool Intersection(VECTOR4D &s1, VECTOR4D &s2, VECTOR4D &point);
	float Distance(VECTOR4D &p);
	int OnAboveOrBelow(VECTOR4D & Pnodo);
public:
	VECTOR4D m_p1, m_p2, m_p3;
	VECTOR4D m_d1, m_d2, m_d3;

};

