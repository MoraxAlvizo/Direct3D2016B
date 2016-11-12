#include "stdafx.h"
#include "Plane.h"
#include "..\Graphics\Matrix4D.h"


CPlane::CPlane(VECTOR4D &p1v, VECTOR4D &p2v, VECTOR4D &p3v) : m_p1(p1v), m_p2(p2v), m_p3(p3v)
{
	m_d1 = m_p2 - m_p1; // Points 1 and 2 define the first base vector for the plane
	Normalize(m_d1);
	m_d2 = (m_p3 - m_p1) - Dot(m_p3 - m_p1, m_d1)*m_d1; // The second is computed projecting point 3
	Normalize(m_d2);
	m_d3 = Cross3(m_d1, m_d2); // We also need a vector normal to the plane
	Normalize(m_d3);
}




CPlane::~CPlane()
{
}

inline float linePointDistance(VECTOR4D &X0, VECTOR4D &X1, VECTOR4D &X2)
{
	return Magnity(Normalize(Cross3(X0 - X1, X0 - X2))) / Magnity(Normalize((X2 - X1)));
}

bool CPlane::Intersection(VECTOR4D & s1, VECTOR4D & s2, VECTOR4D & point)
{
	VECTOR4D u = s2 - s1;
	VECTOR4D w = s1 - m_p1;

	float d = Dot(m_d3, u);
	float n = -Dot(m_d3, w);

	if (fabs(d)<1e-4) // segment and plane are parallel
		return false;

	float t = n / d;

	if (t < 0.0f || t > 1.0f) // plane doesn't cut segment
		return false;

	point = s1 + t*u; // plane segment intersection point

	float z = Dot(point - m_p1, m_d2);
	if (z >= 0.0f)
		return true;  // the point is in the valid half-plane
	else
		return true; //WARNING: I have changed this to true

}

float CPlane::Distance(VECTOR4D & p)
{
	// Project point into plane
	VECTOR4D projection = Dot(p - m_p1, m_d3)*m_d3;
	VECTOR4D pointInPlane = p - projection;
	// Compute the half-plane that contains the projection
	float t2 = Dot(pointInPlane - m_p1, m_d2);
	if (t2 >= 0) // The point projection is in the valid halp-plane
		return fabs(Dot(p - m_p1, m_d3));
	else  // The point projection is in the invalid half-plane compute point-line distance
		return linePointDistance(p, m_p1, m_p2);
}

int CPlane::OnAboveOrBelow(VECTOR4D & Pnodo)
{
	double planeEq = m_d3.x * (Pnodo.x - m_p1.x) + m_d3.y * (Pnodo.y - m_p1.y) + m_d3.z * (Pnodo.z - m_p1.z);
	double TOLERANCE = 1.0e-9;
	if (fabs(planeEq) <= TOLERANCE) {
		return 1; //on
	}
	else if (planeEq > TOLERANCE) {
		return 1; //above
	}
	else {
		return 0; //below
	}
}
