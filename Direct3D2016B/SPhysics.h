/*

File name:
	CSPhysics.cpp

Descrition:
	This state is responsible to compute the physics that happend in the scene.

	DD/MM/AA	Name	- Coment
	16/09/16	OMAR	- Creation

*/
#pragma once
#include "HSM\StateBase.h"
#include "Collisions\MeshCollision.h"
#include "Collisions\OctreeCube.h"

#define CLSID_CSPhysics 0x1239acb

class CSPhysics :
	public CStateBase
{
public:
	/* Members */
	CDXManager* m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	COctreeCube* m_pOctree;
	unsigned long m_lFlags;

	bool m_bMoveSphere1;
	bool m_bMoveSphere2;
#define PHYSICS_DRAW_OCTREE 0x01
	/* Scene */
	vector<CMeshCollision> *m_pScene;

	CSPhysics();
	virtual ~CSPhysics();
	unsigned long GetClassID() { return CLSID_CSPhysics; }
	const char* GetClassString() { return "CSPhysics"; }

protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
	void ManageKeyboardEvents(UINT event, WPARAM wParam);
};

