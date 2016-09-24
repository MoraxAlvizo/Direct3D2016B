/*

File name:
	CSPhysics.cpp

Descrition:
	This state is responsible to compute the physics that happend in the scene.

	DD/MM/AA	Name	- Coment
	16/09/16	OMAR	- Octree scene creation working.
	16/09/16	OMAR	- Creation

*/

#include "stdafx.h"
#include <cstdio>
#include "SPhysics.h"
#include "SOnGame.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"


CSPhysics::CSPhysics()
{
}


CSPhysics::~CSPhysics()
{
}

void CSPhysics::OnEntry(void)
{
	CSOnGame* OnGame = (CSOnGame*)GetSuperState();

	/* Load pointers */
	m_pDXManager = OnGame->m_pDXManager;
	m_pDXPainter = OnGame->m_pDXPainter;
	m_pScene = &OnGame->m_Scene;

	printf("[HCM] %s:OnEntry\n", GetClassString());

	for (unsigned long i = 0; i < m_pScene->size(); i++ )
	{
		(*m_pScene)[i].m_octree = new COctree((*m_pScene)[i].m_Box.min, (*m_pScene)[i].m_Box.max, 0, m_pDXPainter);
		(*m_pScene)[i].m_octree->m_Color = { i % 2 ? 1.f : 0.f , 1,i % 3 ? 1.f : 0.f,0 };
	}


	m_pOctree = new COctreeCube({ -BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2 , 0 },
	{ BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2 }, 0);

	//Create objects
	//int i = 1;
	for (unsigned long i = 0; i < m_pScene->size(); i++)
	{
		m_pOctree->addObject(&(*m_pScene)[i],
			(*m_pScene)[i].m_Box.min * (*m_pScene)[i].m_World,
			(*m_pScene)[i].m_Box.max * (*m_pScene)[i].m_World);

		//(*m_pScene)[i].createOctree();
	}
	
}

unsigned long CSPhysics::OnEvent(CEventBase * pEvent)
{
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			if (pWin32->m_wParam == 'p')
			{
				m_lFlags ^= PHYSICS_DRAW_OCTREE;
				return 0;
			}
			break;
		case WM_PAINT:

			if (m_pDXManager->GetSwapChain())
			{
				//if (m_lFlags & PHYSICS_DRAW_OCTREE)
				{
					m_pDXPainter->m_Params.World = Identity();
					m_pDXPainter->m_Params.Flags1 = DRAW_JUST_WITH_COLOR;
					VECTOR4D NightBlue = { 0,0,.1, 0 };
					m_pDXManager->GetContext()->ClearRenderTargetView(m_pDXManager->GetMainRTV(), (float*)&NightBlue);
					m_pDXManager->GetContext()->ClearDepthStencilView(
							m_pDXManager->GetMainDSV(),
							D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
							1.0f,
							0);
					m_pOctree->DrawOctree(m_pDXPainter);
					/*printf("\n------------------- Octree scene ----------------------\n\n");
					m_pOctree->printCHildren(0);
					printf("\n-------------------------------------------------------\n\n");
					m_lFlags ^= PHYSICS_DRAW_OCTREE;*/

					for (unsigned long i = 0; i < m_pScene->size(); i++)
					{
						m_pDXPainter->m_Params.World = (*m_pScene)[i].m_World;
						(*m_pScene)[i].m_octree->DrawOctree();

					}
				}

			}

			break;
		}
	}
	return __super::OnEvent(pEvent) ;
}

void CSPhysics::OnExit(void)
{
	printf("[HCM] %s:OnExit\n", GetClassString());
}
