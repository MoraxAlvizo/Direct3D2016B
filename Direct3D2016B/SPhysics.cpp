/*

File name:
	CSPhysics.cpp

Descrition:
	This state is responsible to compute the physics that happend in the scene.

	DD/MM/AA	Name	- Coment
	24/09/16	OMAR	- Issue #2 if 2 objects are collided, stop them
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
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			ManageKeyboardEvents(pWin32->m_msg, pWin32->m_wParam);
			//return 0;
		}
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
				if (m_lFlags & PHYSICS_DRAW_OCTREE)
				{
					printf("\n------------------- Octree scene ----------------------\n\n");
					m_pOctree->printCHildren(0);
					printf("\n-------------------------------------------------------\n\n");
					m_lFlags ^= PHYSICS_DRAW_OCTREE;
				}

				if (m_bMoveSphere1 || m_bMoveSphere2)
				{
					set<unsigned long long> potencialCollisions;
					m_pOctree->potentialCollsions(potencialCollisions);

					for (set<unsigned long long>::iterator it2 = potencialCollisions.begin(); it2 != potencialCollisions.end(); it2++)
					{
						COctreeCube::MeshPair meshPair;
						CMeshCollision *object1;
						CMeshCollision *object2;
						VECTOR4D min1, max1;
						VECTOR4D min2, max2;

						meshPair.m_idColision = *it2;
						object1 = &(*m_pScene)[meshPair.m_object1ID];
						object2 = &(*m_pScene)[meshPair.m_object2ID];

						/* Max min object 1*/
						min1 = object1->m_Box.min * object1->m_World;
						max1 = object1->m_Box.max * object1->m_World;

						/* Max min object 2*/
						min2 = object2->m_Box.min * object2->m_World;
						max2 = object2->m_Box.max * object2->m_World;

						/* Check if boxes collision */
						if (min1.x < max2.x &&
							max1.x > min2.x &&
							min1.y < max2.y &&
							max1.y > min2.y &&
							min1.z < max2.z &&
							max1.z > min2.z)
						{
							if ((strcmp(object1->m_cName, "Sphere") == 0 || strcmp(object2->m_cName, "Sphere") == 0)&& m_bMoveSphere1)
								m_bMoveSphere1 = false;
							if ((strcmp(object1->m_cName, "Sphere.001") == 0 || strcmp(object2->m_cName, "Sphere.001") == 0) && m_bMoveSphere2)
								m_bMoveSphere2 = false;
							
						}
					}

					for (unsigned long i = 0; i < m_pScene->size(); i++)
					{
						if ((strcmp((*m_pScene)[i].m_cName, "Sphere") == 0 && m_bMoveSphere1) ||
							(strcmp((*m_pScene)[i].m_cName, "Sphere.001") == 0 && m_bMoveSphere2))
						{
							m_pOctree->removeObject(&(*m_pScene)[i],
								(*m_pScene)[i].m_Box.min * (*m_pScene)[i].m_World,
								(*m_pScene)[i].m_Box.max * (*m_pScene)[i].m_World);

							(*m_pScene)[i].m_World = (*m_pScene)[i].m_World * Translation(0, 0, -0.1);

							m_pOctree->addObject(&(*m_pScene)[i],
								(*m_pScene)[i].m_Box.min * (*m_pScene)[i].m_World,
								(*m_pScene)[i].m_Box.max * (*m_pScene)[i].m_World);
						}

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
	SAFE_DELETE(m_pOctree);
	printf("[HCM] %s:OnExit\n", GetClassString());
}

/* Keyboard keys */
#define VK_1 97
#define VK_2 98

void CSPhysics::ManageKeyboardEvents(UINT event, WPARAM wParam)
{
	switch (event)
	{
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case VK_1:
			m_bMoveSphere1 = false;
			break;
		case VK_2:
			m_bMoveSphere2 = false;
			break;
		default:
			break;
		}
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_1:
			m_bMoveSphere1 = true;
			break;
		case VK_2:
			m_bMoveSphere2 = true;
			break;
		default:
			break;
		}
		break;
	}
	default:
		break;

	}
}
