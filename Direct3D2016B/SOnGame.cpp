/*

File name:
	CSOnGame.cpp

Descrition:
	This state is responsible to draw all the scene and manage
	the camera.

	DD/MM/AA	Name	- Coment
	15/09/16	OMAR	- Creation

*/

#include "stdafx.h"
#include "SOnGame.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
#include "Graphics\ImageBMP.h"
#include "SMain.h"
#include "Cut/Plane.h"
#include "Collisions\BVH.h"
#include <time.h>

#include <iostream>
#include "Cut/VMesh.h"
/* assimp include files. */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CSOnGame::CSOnGame()
{
}


CSOnGame::~CSOnGame()
{
}

VECTOR4D Sphere1(float u, float v)
{
	float r = 1.f;
	VECTOR4D v1;
	v1.x = r*cos(2 * 3.14159 *u)*sin(3.14159 *v);
	v1.y = r*sin(2 * 3.14159*u)*sin(3.14159*v);
	v1.z = r*cos(3.14159*v);
	v1.w = 1;
	return v1;
}
void CSOnGame::OnEntry(void)
{
	CSMain* main = (CSMain*)GetSuperState();
	VECTOR4D White = { 1, 1, 1, 1 };
	VECTOR4D EyePos = { 6, 10, 6, 1 };
	VECTOR4D Target = { 0, 0, 0, 1 };
	VECTOR4D Up = { 0, 0, 1, 0 };

	printf("[HCM] %s:OnEntry\n", GetClassString());

	/* Create View, Projection and World Matrix */
	m_View = View(EyePos, Target, Up);
	m_Projection = PerspectiveWidthHeightLH(0.05, 0.05, 0.1, 100);
	m_World = Identity();

	/* Load Scene */
	char buffer[BUF_SIZE];
	int ret;
	ret = wcstombs(buffer, main->m_Params.scene, sizeof(buffer));


	LoadScene(buffer);

	/* Load pointers */
	m_pDXManager = main->m_pDXManager;
	m_pDXPainter = main->m_pDXPainter;
	m_hWnd = main->m_hWnd;
	m_lPainterFlags = main->m_Params.PainterFlags;

	/* Load textures */
	CImageBMP *texture = CImageBMP::CreateBitmapFromFile("..\\Assets\\tela.bmp", NULL);
	if (!texture)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	m_pTexture = texture->CreateTexture(m_pDXManager);
	if (!m_pTexture)
	{
		MessageBox(NULL, L"No se pudo cargar textura al GPU",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}
	CImageBMP* pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\ladrillo.bmp", NULL);

	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	m_pNormalMap = pImage->CreateTexture(m_pDXManager);
	if (!m_pNormalMap)
	{
		MessageBox(NULL, L"No se pudo cargar textura al GPU",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}
	CImageBMP::DestroyBitmap(pImage);

	pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\SanPedro.bmp", NULL);

	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	m_pEnvMap = pImage->CreateTexture(m_pDXManager);

	if (!m_pEnvMap)
	{
		MessageBox(NULL, L"No se pudo cargar textura al GPU",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	CImageBMP::DestroyBitmap(pImage);

	pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\Normal.bmp", NULL);

	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	m_pNormalMapTrue = pImage->CreateTexture(m_pDXManager);

	if (!m_pNormalMapTrue)
	{
		MessageBox(NULL, L"No se pudo cargar textura al GPU",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	CImageBMP::DestroyBitmap(pImage);

	pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\Emissive.bmp", NULL);

	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	m_pEmissiveMap = pImage->CreateTexture(m_pDXManager);

	if (!m_pEmissiveMap)
	{
		MessageBox(NULL, L"No se pudo cargar textura al GPU",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	CImageBMP::DestroyBitmap(pImage);

	pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\cubemap.bmp", NULL);

	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	m_pCubeMap = pImage->CreateTexture(m_pDXManager);

	if (!m_pCubeMap)
	{
		MessageBox(NULL, L"No se pudo cargar textura al GPU",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	CImageBMP::DestroyBitmap(pImage);

	/* Create a timer */
	SetTimer(main->m_hWnd, 1, 5000, NULL);

	/* Create SRVs */
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pTexture, NULL, &m_pSRVTexture);
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pNormalMap, NULL, &m_pSRVNormalMap);
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pEnvMap, NULL, &m_pSRVEnvMap);
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pNormalMapTrue, NULL, &m_pSRVNormalMapTrue);
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pEmissiveMap, NULL, &m_pSRVEmissiveMap);

	/* Initialize camera options */
	m_bLeft =  m_bRight =
	m_bUp = m_bDown =
	m_bForward =  m_bBackward = m_bTurnLeft = m_bTurnRight =
	m_bTurnUp =  m_bTurnDown = m_bTurnS =  m_bTurnS1 =false;

	/* Init collisions structures */

	m_pOctree = new COctreeCube({ -OCTREECUBE_BOX_SIZE / 2, -OCTREECUBE_BOX_SIZE / 2, -OCTREECUBE_BOX_SIZE / 2 , 0 },
	{ OCTREECUBE_BOX_SIZE / 2, OCTREECUBE_BOX_SIZE / 2, OCTREECUBE_BOX_SIZE / 2 }, 0);

	//Create objects
	//int i = 1;
	//for (unsigned long i = 0; i < m_Scene.size(); i++)
	//{
	//	/*m_pOctree->addObject(&m_Scene[i],
	//		m_Scene[i].m_Box.min * m_Scene[i].m_World,
	//		m_Scene[i].m_Box.max * m_Scene[i].m_World);*/

	//	vector<unsigned long> primitives;

	//	primitives.resize(m_Scene[i].m_Centroides.size());
	//	for (unsigned long j = 0; j < m_Scene[i].m_Centroides.size(); j++)
	//		primitives[j] = j;


	//	m_nFlagsPainter = 0;

	//	m_Scene[i].m_BVH = new BVH();

	//	double secs;
	//	LARGE_INTEGER t_ini, t_fin;

	//	QueryPerformanceCounter(&t_ini);   
	//	m_Scene[i].m_BVH->Preconstruction(m_Scene[i]);
	//	m_Scene[i].m_BVH->Construction(m_Scene[i], 1, primitives);
	//	m_Scene[i].m_BVH->Postconstruction(m_Scene[i]);
	//	QueryPerformanceCounter(&t_fin);

	//	LARGE_INTEGER freq;
	//	QueryPerformanceFrequency(&freq);
	//	secs =  (double)(t_fin.QuadPart - t_ini.QuadPart) / (double)freq.QuadPart;
	//	printf("mesh[%i] = Primitivas = %i , tiempo_construccion = %.16g ms\n",i, m_Scene[i].m_Centroides.size(), secs * 1000.0);

	//	//m_Scene[i].m_BVH->Build(m_Scene[i], primitives);
	//}
	/* Compile BVH CS shaders */
	BVH::CompileCSShaders(m_pDXManager);
	CMesh::CompileCSShaders(m_pDXManager);
	CVMesh::CompileCSShaders(m_pDXManager);

	m_ScenePhysics.resize(2);
	//m_SceneCollisions.resize(2);

	
	/***************** Cube 0 *******************************/
	m_ScenePhysics[0].LoadMSHFile("");
	m_ScenePhysics[0].InitializaMassSpring();
	//m_SceneCollisions[0].m_World = Identity();//Translation(-9, -9, -9);
	strcpy(m_ScenePhysics[0].m_cName, "Cube 0");
	m_ScenePhysics[0].m_lID = 0;
	m_ScenePhysics[0].CreateMeshCollisionFromVMesh(m_ScenePhysics[0]);
	m_ScenePhysics[0].CreateVertexAndIndexBuffer(m_pDXManager);
	m_ScenePhysics[0].m_BVH = new BVH();
	m_ScenePhysics[0].m_BVH->CreateGPUBuffer(m_pDXManager);
	m_ScenePhysics[0].m_BVH->BuildGPU(m_pDXManager, &m_ScenePhysics[0]);
	m_ScenePhysics[0].CreateTetraindexAndMassSpringBuffers(m_pDXManager);

	m_pOctree->addObject(&m_ScenePhysics[0],
		m_ScenePhysics[0].m_Box.min,
		m_ScenePhysics[0].m_Box.max);

	/***************** Cube 1 *******************************/
	/*m_SceneCollisions[1].m_World = Identity();*/
	m_ScenePhysics[1].LoadMSHFile("");
	m_ScenePhysics[1].InitializaMassSpring();
	m_ScenePhysics[1].m_lID = 1;
	strcpy(m_ScenePhysics[1].m_cName, "Cube 1");
	m_ScenePhysics[1].CreateMeshCollisionFromVMesh(m_ScenePhysics[1]);
	m_ScenePhysics[1].CreateVertexAndIndexBuffer(m_pDXManager);
	m_ScenePhysics[1].m_BVH = new BVH();

	m_ScenePhysics[1].CSApplyTranformation(Translation(.2, .2, .2), m_pDXManager);
	m_ScenePhysics[1].m_BVH->CreateGPUBuffer(m_pDXManager);
	m_ScenePhysics[1].m_BVH->BuildGPU(m_pDXManager, &m_ScenePhysics[1]);
	m_ScenePhysics[1].CreateTetraindexAndMassSpringBuffers(m_pDXManager);

	m_pOctree->addObject(&m_ScenePhysics[1],
		m_ScenePhysics[1].m_Box.min,
		m_ScenePhysics[1].m_Box.max);


	

	/*m_SceneCollisions[0].m_BVH->Preconstruction(m_SceneCollisions[0]);

	primitives.resize(m_SceneCollisions[0].m_Centroides.size());
	for (unsigned long j = 0; j < m_SceneCollisions[0].m_Centroides.size(); j++)
		primitives[j] = j;

	m_SceneCollisions[0].m_BVH->Construction(m_SceneCollisions[0], 1, primitives);
	m_SceneCollisions[0].m_BVH->Postconstruction(m_SceneCollisions[0]);*/
 
	//m_pOctree->addObject(&m_SceneCollisions[0],
	//	m_SceneCollisions[0].m_Box.min /* m_SceneCollisions[0].m_World*/,
	//	m_SceneCollisions[0].m_Box.max  /* m_SceneCollisions[0].m_World*/);

	
	/*m_SceneCollisions[0].m_World = 
	m_SceneCollisions[0].m_TranslationBVH = Translation(.5,.5,.5);*/
	

	/* Build with GPU */
	

	

	//m_SceneCollisions[1].ApplyTransformation(Translation(.2, .2, .2));

	//m_SceneCollisions[1].m_BVH->Preconstruction(m_SceneCollisions[1]);
	//primitives.resize(m_SceneCollisions[1].m_Centroides.size());
	//for (unsigned long j = 0; j < m_SceneCollisions[1].m_Centroides.size(); j++)
	//	primitives[j] = j;

	//m_SceneCollisions[1].m_BVH->Construction(m_SceneCollisions[1], 1, primitives);
	//m_SceneCollisions[1].m_BVH->Postconstruction(m_SceneCollisions[1]);

	//m_pOctree->addObject(&m_SceneCollisions[1],
	//	m_SceneCollisions[1].m_Box.min /* m_SceneCollisions[1].m_World*/,
	//	m_SceneCollisions[1].m_Box.max /* m_SceneCollisions[1].m_World*/);

	
	//m_SceneCollisions[1].m_World = Translation(1, 1, 1);
	//m_SceneCollisions[1].m_TranslationBVH = Identity();
	

	/***************** Cube 2 *******************************/

	//m_SceneCollisions[2].m_World = Translation(.5, .5, .5); 
	//m_SceneCollisions[2].CreateMeshCollisionFromVMesh(m_ScenePhysics[2]);
	//m_SceneCollisions[2].m_BVH = new BVH();

	//m_SceneCollisions[2].m_BVH->Preconstruction(m_SceneCollisions[2]);

	//primitives.resize(m_SceneCollisions[2].m_Centroides.size());
	//for (unsigned long j = 0; j < m_SceneCollisions[2].m_Centroides.size(); j++)
	//	primitives[j] = j;

	//m_SceneCollisions[2].m_BVH->Construction(m_SceneCollisions[2], 1, primitives);
	//m_SceneCollisions[2].m_BVH->Postconstruction(m_SceneCollisions[2]);

	//m_pOctree->addObject(&m_SceneCollisions[2],
	//	m_SceneCollisions[2].m_Box.min * m_SceneCollisions[2].m_World,
	//	m_SceneCollisions[2].m_Box.max * m_SceneCollisions[2].m_World);

	//m_SceneCollisions[2].m_lID = 2;

}

unsigned long CSOnGame::OnEvent(CEventBase * pEvent)
{
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		if (m_pDXManager->GetSwapChain())
		{
			static int currentFrame = 0;
			currentFrame++;
			// Clear render targer and deph stencil
			ID3D11Texture2D* pBackBuffer = 0;
			MATRIX4D AC; /* Matriz de correction de aspecto */
						 // Colors
			VECTOR4D DarkGray = { 0.25,0.25,0.25,1 };
			VECTOR4D White = { 1,1,1,1 };
			VECTOR4D Gray = { .5,.5,.5,0 };
			VECTOR4D NightBlue = { 0,0,.1, 0 };
			VECTOR4D Black = { 0, 0, 0, 0 };
			D3D11_TEXTURE2D_DESC dtd;
			m_pDXManager->GetContext()->ClearRenderTargetView(m_pDXManager->GetMainRTV(), (float*)&NightBlue);
			m_pDXManager->GetContext()->ClearDepthStencilView(
				m_pDXManager->GetMainDSV(),
				D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
				1.0f,
				0);

			if (m_lMoveSphere1 || m_lMoveSphere2)
			{

				for (unsigned long i = 0; i < m_ScenePhysics.size(); i++)
				{
					m_ScenePhysics[i].ResetColors();
					unsigned long flags;
					if ((m_ScenePhysics[i].m_lID == 0 && (flags = m_lMoveSphere1)) ||
						(m_ScenePhysics[i].m_lID == 1 && (flags = m_lMoveSphere2))
						)
					{
						float direction = -1;

						if (flags)
						{
							if (flags & MOVE_DOWN)
								direction = -1;
							else if (flags & MOVE_UP)
								direction = 1;
							else
								direction = 0;
						}

						/* Remove object from Octree*/
						m_pOctree->removeObject(&m_ScenePhysics[i],
							m_ScenePhysics[i].m_Box.min ,
							m_ScenePhysics[i].m_Box.max );

						/* Move object with Compute Shader*/
						m_ScenePhysics[i].CSApplyTranformation(Translation(0, 0, direction*0.1), m_pDXManager);
						/* Rebuild BVH in GPU*/
						m_ScenePhysics[i].m_BVH->BuildGPU(m_pDXManager, &m_ScenePhysics[i]);
						m_pDXManager->GetContext()->ClearState();

						/* Add again */
						m_pOctree->addObject(&m_ScenePhysics[i],
							m_ScenePhysics[i].m_Box.min,
							m_ScenePhysics[i].m_Box.max);
					}
				}
			}


			// Actualizar camara si fue movida
			UpdateCamera();

			/* Get Backbuffer to get height and width */
			m_pDXManager->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
			pBackBuffer->GetDesc(&dtd);
			dtd.BindFlags |= (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
			SAFE_RELEASE(pBackBuffer);

			/* Create AC Matrix */
			AC = Scaling((float)dtd.Height / dtd.Width, 1, 1);

			/* Set Material parameters */
			m_pDXPainter->SetRenderTarget(m_pDXManager->GetMainRTV());
			m_pDXPainter->m_Params.Material.Diffuse = Gray;
			m_pDXPainter->m_Params.Material.Ambient = Gray;

			/* Set SRVs */
			m_pDXManager->GetContext()->PSSetShaderResources(0, 1, &m_pSRVTexture);
			m_pDXManager->GetContext()->PSSetShaderResources(1, 1, &m_pSRVNormalMap);
			m_pDXManager->GetContext()->PSSetShaderResources(2, 1, &m_pSRVEnvMap);
			m_pDXManager->GetContext()->PSSetShaderResources(3, 1, &m_pSRVNormalMapTrue);
			m_pDXManager->GetContext()->PSSetShaderResources(4, 1, &m_pSRVEmissiveMap);

			/* Set params */
			m_pDXPainter->m_Params.Brightness = Black;
			m_pDXPainter->m_Params.Flags1 = m_lPainterFlags;

 			m_pDXPainter->m_Params.World = m_World;
			m_pDXPainter->m_Params.View = m_View;
			m_pDXPainter->m_Params.Projection = m_Projection*AC;

			/* Render with Left Hand*/
			m_pDXManager->GetContext()->RSSetState(m_pDXPainter->GetDrawLHRState());

			/* Print OCtree*/
			if (m_lFlags & PHYSICS_PRINT_OCTREE)
			{
				m_lFlags ^= PHYSICS_PRINT_OCTREE;
				m_pOctree->printCHildren(0);

			}

			/* Print BVH */
			if (m_lFlags & PHYSICS_PRINT_BVH)
			{
				m_lFlags ^= PHYSICS_PRINT_BVH;
				m_ScenePhysics[0].m_BVH->PrintLBVH(1, 0);

			}

			/* Check if the objects was moved */
			if (m_lFlags & PHYSICS_DRAW_OCTREE)
			{
				m_pDXPainter->m_Params.World = Identity();
				m_pDXPainter->m_Params.Flags1 = DRAW_JUST_WITH_COLOR;

				//m_pOctree->DrawOctree(m_pDXPainter);

				//unsigned long i = 0;
				for (unsigned long i = 0; i < m_ScenePhysics.size(); i++)
				{
					m_pDXPainter->m_Params.World = Identity();
					m_ScenePhysics[i].m_BVH->DrawLBVH(m_pDXPainter, 1);
				}
			}

			m_pDXPainter->m_Params.Flags1 = m_lPainterFlags;


			/************** Actualizar fuerza y BVH **********/
			VECTOR4D EF = { 0,0,0,0 };
			/* Aplicar fuerzas */
			if (m_nFlagsPainter & PAINTER_APPLY_FORCE)
			{
				m_nFlagsPainter ^= PAINTER_APPLY_FORCE;
				EF.y = 10;
			}

			for (unsigned long i = 0; i < m_ScenePhysics.size(); i++)
			{
				m_ScenePhysics[i].ResetColors();

				m_ScenePhysics[i].CSApplyForces(m_pDXManager, { 0,0,-0.98f,0 }, EF);
				m_ScenePhysics[i].CreateVertexAndIndexBuffer(m_pDXManager);

				
				/*m_ScenePhysics[i].ApplyForces({ 0,0,-0.98f,0 }, EF);
				m_ScenePhysics[i].CreateVertexAndIndexBuffer(m_pDXManager);*/

				/* Remove object from Octree*/
				m_pOctree->removeObject(&m_ScenePhysics[i],
					m_ScenePhysics[i].m_Box.min,
					m_ScenePhysics[i].m_Box.max);

				/* Rebuild BVH in GPU*/
				m_ScenePhysics[i].m_BVH->BuildGPU(m_pDXManager, &m_ScenePhysics[i]);

				/* Add again */
				m_pOctree->addObject(&m_ScenePhysics[i],
					m_ScenePhysics[i].m_Box.min,
					m_ScenePhysics[i].m_Box.max);
			}


			/************** Obtener colisiones *************/
			set<unsigned long long> potencialCollisions;
			m_pOctree->potentialCollsions(potencialCollisions);


			for (set<unsigned long long>::iterator it2 = potencialCollisions.begin(); it2 != potencialCollisions.end(); it2++)
			{
				COctreeCube::MeshPair meshPair;
				CMesh *object1;
				CMesh *object2;
				VECTOR4D min1, max1;
				VECTOR4D min2, max2;

				meshPair.m_idColision = *it2;
				object1 = &m_ScenePhysics[meshPair.m_object1ID];
				object2 = &m_ScenePhysics[meshPair.m_object2ID];

				/* Max min object 1*/
				min1 = object1->m_Box.min;
				max1 = object1->m_Box.max;

				/* Max min object 2*/
				min2 = object2->m_Box.min;
				max2 = object2->m_Box.max;

				/* Check if boxes collision */
				if (min1.x < max2.x &&
					max1.x > min2.x &&
					min1.y < max2.y &&
					max1.y > min2.y &&
					min1.z < max2.z &&
					max1.z > min2.z)
				{
					//object1->m_BVH->Traversal(object2->m_BVH, object1->m_TranslationBVH, object2->m_TranslationBVH, *object1, *object2);
					object1->m_BVH->TraversalLBVH(object2->m_BVH, 1, 1, *object1, *object2);
					//object1->m_BVH->BitTrailTraversal(object2->m_BVH, object1->m_TranslationBVH, object2->m_TranslationBVH, *object1, *object2);
				}
			}

			m_pDXPainter->m_Params.World = Identity();
			m_pDXPainter->m_Params.Flags1 = DRAW_JUST_WITH_COLOR;
			
			/* Draw scene */
			for (unsigned long i = 0; i < m_ScenePhysics.size(); i++)
			{

				m_pDXPainter->m_Params.Flags1 = m_lPainterFlags;
				m_pDXPainter->m_Params.World = Identity();

				m_pDXPainter->DrawIndexed(&m_ScenePhysics[i].m_Vertices[0],
					m_ScenePhysics[i].m_Vertices.size(),
					&m_ScenePhysics[i].m_Indices[0],
					m_ScenePhysics[i].m_Indices.size(),
					PAINTER_DRAW);
				
			}

				

			m_pDXManager->GetSwapChain()->Present(1, 0);

		}

	}
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			/*if (pWin32->m_wParam == '8')
			{
				m_pSMOwner->Transition(CLSID_CSIntro);
				CSMain* main = (CSMain*)GetSuperState();
				InvalidateRect(main->m_hWnd, NULL, false);
				return 0;
			}*/
			if (pWin32->m_wParam == 'b')
			{
				m_lFlags ^= PHYSICS_PRINT_BVH;
				return 0;
			}
			if (pWin32->m_wParam == 'p')
			{
				m_lFlags ^= PHYSICS_PRINT_OCTREE;
				return 0;
			}
			if (pWin32->m_wParam == '9')
			{
				m_nFlagsPainter ^= PAINTER_DRAW_WIREFRAME;
				return 0;
			}
			if (pWin32->m_wParam == '1')
			{
				m_nFlagsPainter ^= PAINTER_APPLY_FORCE;
				return 0;
			}
			if (pWin32->m_wParam == '0')
			{
				m_lFlags ^= PHYSICS_DRAW_OCTREE;
				return 0;
			}
			break;
		case WM_TIMER:
			switch (pWin32->m_wParam)
			{
			case 1:
			{
				/*m_pSMOwner->Transition(CLSID_CSIntro);
				CSMain* main = (CSMain*)GetSuperState();
				InvalidateRect(main->m_hWnd, NULL, false);*/
				return 0;
			}
			default:
				break;
			}
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			ManageKeyboardEvents(pWin32->m_msg, pWin32->m_wParam);
			return 0;
		}
		default:
			break;
		}
	}
	return __super::OnEvent(pEvent);
}

void CSOnGame::OnExit(void)
{
	CSMain* main = (CSMain*)GetSuperState();

	printf("[HCM] %s:OnExit\n", GetClassString());
	/* Kill timer */
	KillTimer(main->m_hWnd, 1);

	/* Release textures */
	SAFE_RELEASE(m_pTexture);   //GPU
	SAFE_RELEASE(m_pNormalMapTrue);
	SAFE_RELEASE(m_pNormalMap);
	SAFE_RELEASE(m_pEmissiveMap);
	SAFE_RELEASE(m_pCubeMap);
	SAFE_RELEASE(m_pEnvMap);

	/* Release SRVs */
	SAFE_RELEASE(m_pSRVTexture);
	SAFE_RELEASE(m_pSRVNormalMap);
	SAFE_RELEASE(m_pSRVEnvMap);
	SAFE_RELEASE(m_pSRVNormalMapTrue);
	SAFE_RELEASE(m_pSRVEmissiveMap);
}

void CSOnGame::Cut()
{
	std::cout << "Se realizo un corte" << std::endl;

	enum EdgeState { UNTESTED, CUT, UNCUT };

	int cutEdgesCount = 0;
	int intersectionTestCount = 0;

	// Test each node against the plane to detect if nodes are cut

}

void CSOnGame::LoadScene(char * filename)
{
	/* the global Assimp scene object */
	const struct aiScene* scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_Fast); //  aiProcessPreset_TargetRealtime_MaxQuality

	//m_Scene.resize(scene->mNumMeshes);

	//for (unsigned long i = 0; i < scene->mNumMeshes; i++)
	//{
	//	float maxX, maxY, maxZ;
	//	float minX, minY, minZ;

	//	maxX = maxY = maxZ = FLT_MIN;
	//	minX = minY = minZ = FLT_MAX;

	//	m_Scene[i].m_Vertices.resize(scene->mMeshes[i]->mNumVertices);
	//	for (unsigned long j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
	//	{
	//		m_Scene[i].m_Vertices[j].Position = {
	//			scene->mMeshes[i]->mVertices[j].x,
	//			scene->mMeshes[i]->mVertices[j].y,
	//			scene->mMeshes[i]->mVertices[j].z,
	//			1 };
	//		if (scene->mMeshes[i]->mVertices[j].x > maxX)
	//			maxX = scene->mMeshes[i]->mVertices[j].x;
	//		if (scene->mMeshes[i]->mVertices[j].y > maxY)
	//			maxY = scene->mMeshes[i]->mVertices[j].y;
	//		if (scene->mMeshes[i]->mVertices[j].z > maxZ)
	//			maxZ = scene->mMeshes[i]->mVertices[j].z;

	//		if (scene->mMeshes[i]->mVertices[j].x < minX)
	//			minX = scene->mMeshes[i]->mVertices[j].x;
	//		if (scene->mMeshes[i]->mVertices[j].y < minY)
	//			minY = scene->mMeshes[i]->mVertices[j].y;
	//		if (scene->mMeshes[i]->mVertices[j].z < minZ)
	//			minZ = scene->mMeshes[i]->mVertices[j].z;
	//	}

	//	m_Scene[i].m_Box.min = { minX, minY, minZ, 1 };
	//	m_Scene[i].m_Box.max = { maxX, maxY, maxZ, 1 };

	//	MATRIX4D t;
	//	t.m00 = scene->mRootNode->mChildren[i]->mTransformation.a1;
	//	t.m01 = scene->mRootNode->mChildren[i]->mTransformation.a2;
	//	t.m02 = scene->mRootNode->mChildren[i]->mTransformation.a3;
	//	t.m03 = scene->mRootNode->mChildren[i]->mTransformation.a4;
	//	t.m10 = scene->mRootNode->mChildren[i]->mTransformation.b1;
	//	t.m11 = scene->mRootNode->mChildren[i]->mTransformation.b2;
	//	t.m12 = scene->mRootNode->mChildren[i]->mTransformation.b3;
	//	t.m13 = scene->mRootNode->mChildren[i]->mTransformation.b4;
	//	t.m20 = scene->mRootNode->mChildren[i]->mTransformation.c1;
	//	t.m21 = scene->mRootNode->mChildren[i]->mTransformation.c2;
	//	t.m22 = scene->mRootNode->mChildren[i]->mTransformation.c3;
	//	t.m23 = scene->mRootNode->mChildren[i]->mTransformation.c4;
	//	t.m30 = scene->mRootNode->mChildren[i]->mTransformation.d1;
	//	t.m31 = scene->mRootNode->mChildren[i]->mTransformation.d2;
	//	t.m32 = scene->mRootNode->mChildren[i]->mTransformation.d3;
	//	t.m33 = scene->mRootNode->mChildren[i]->mTransformation.d4;

	//	m_Scene[i].m_World = Transpose(t);

	//	m_Scene[i].m_Indices.resize(scene->mMeshes[i]->mNumFaces * scene->mMeshes[i]->mFaces[0].mNumIndices);
	//	for (unsigned long j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
	//	{
	//		for (unsigned long k = 0; k < scene->mMeshes[i]->mFaces[j].mNumIndices; k++)
	//		{
	//			m_Scene[i].m_Indices[j*scene->mMeshes[i]->mFaces[j].mNumIndices + k] = scene->mMeshes[i]->mFaces[j].mIndices[k];
	//		}
	//	}

	//	for (unsigned long j = 0; j < m_Scene[i].m_Vertices.size(); j++)
	//	{
	//		VECTOR4D TexCoord = { 0,0,0,0 };
	//		TexCoord.x = m_Scene[i].m_Vertices[j].Position.x;
	//		TexCoord.y = m_Scene[i].m_Vertices[j].Position.z;
	//		TexCoord.z = m_Scene[i].m_Vertices[j].Position.y;
	//		TexCoord = Normalize(TexCoord);
	//		TexCoord.x = TexCoord.x * 0.5 + 0.5;
	//		TexCoord.y = TexCoord.y * 0.5 + 0.5;

	//		m_Scene[i].m_Vertices[j].TexCoord = TexCoord;
	//	}
	//	//g_Scene[i].Optimize();
	//	m_Scene[i].BuildTangentSpaceFromTexCoordsIndexed(true);
	//	m_Scene[i].GenerarCentroides();

	//	/* Set id */
	//	m_Scene[i].m_lID = i;
	//	strcpy(m_Scene[i].m_cName, scene->mMeshes[i]->mName.C_Str());
	//}
}

void CSOnGame::UpdateCamera()
{
	// Control de camara
	MATRIX4D InvV = FastInverse(m_View);
	VECTOR4D XDir = { InvV.m00, InvV.m01, InvV.m02, 0 };
	VECTOR4D YDir = { InvV.m10, InvV.m11, InvV.m12, 0 };
	VECTOR4D ZDir = { InvV.m20, InvV.m21, InvV.m22, 0 };
	VECTOR4D EyePos = { InvV.m30, InvV.m31, InvV.m32, 1 };


	MATRIX4D O = InvV;
	O.m03 = 0;
	O.m13 = 0;
	O.m23 = 0;

	VECTOR4D Speed = { 0.1, 0.1, 0.1, 0 };
	bool movePos = false;
	if (m_bBackward)
	{
		EyePos = EyePos - ZDir*Speed;
		movePos = true;
	}
	if (m_bForward)
	{
		EyePos = EyePos + ZDir*Speed;
		movePos = true;
	}
	if (m_bLeft)
	{
		EyePos = EyePos - XDir*Speed;
		movePos = true;
	}
	if (m_bRight)
	{
		EyePos = EyePos + XDir*Speed;
		movePos = true;
	}
	if (m_bUp)
	{
		EyePos = EyePos + YDir*Speed;
		movePos = true;
	}
	if (m_bDown)
	{
		EyePos = EyePos - YDir*Speed;
		movePos = true;
	}

	if (movePos)
	{
		//g_onFirstMouseMove = true;
	}

	m_pDXPainter->m_Params.lights[1].Position = EyePos;
	m_pDXPainter->m_Params.lights[1].Direction = ZDir;

	// Set camara pos in params
	m_pDXPainter->m_Params.CameraPosition = EyePos;
	float speed = .02;

	if (m_bTurnLeft)
	{
		MATRIX4D R = RotationAxis(-speed, YDir);
		O = O*R;
	}
	if (m_bTurnRight)
	{
		MATRIX4D R = RotationAxis(speed, YDir);
		O = O*R;
	}
	if (m_bTurnUp)
	{
		MATRIX4D R = RotationAxis(-speed, XDir);
		O = O*R;
	}
	if (m_bTurnDown)
	{
		MATRIX4D R = RotationAxis(speed, XDir);
		O = O*R;
	}
	if (m_bTurnS)
	{
		MATRIX4D R = RotationAxis(speed, ZDir);
		O = O*R;
	}
	if (m_bTurnS1)
	{
		MATRIX4D R = RotationAxis(-speed, ZDir);
		O = O*R;
	}

	InvV = O;

	InvV.m30 = EyePos.x;
	InvV.m31 = EyePos.y;
	InvV.m32 = EyePos.z;

	m_View = Orthogonalize(FastInverse(InvV));

	InvalidateRect(m_hWnd, NULL, false);
}

/* Keyboard keys */
#define VK_A 0x41
#define VK_Q 0x51
#define VK_J 0x4A
#define VK_L 0x4C
#define VK_K 0x4B
#define VK_I 0x49
#define VK_U 0x55
#define VK_O 0x4F
#define VK_1 97
#define VK_2 98
#define VK_3 99
#define VK_4 100
#define VK_5 101

void CSOnGame::ManageKeyboardEvents(UINT event, WPARAM wParam)
{
	switch (event)
	{
		case WM_KEYUP:
		{
			switch (wParam)
			{
			case VK_LEFT:
				m_bLeft = false;
				break;
			case VK_RIGHT:
				m_bRight = false;
				break;
			case VK_UP:
				m_bForward = false;
				break;
			case VK_DOWN:
				m_bBackward = false;
				break;
			case VK_A:
				m_bUp = false;
				break;
			case VK_Q:
				m_bDown = false;
				break;
			case VK_J:
				m_bTurnLeft = false;
				break;
			case VK_L:
				m_bTurnRight = false;
				break;
			case VK_K:
				m_bTurnDown = false;
				break;
			case VK_I:
				m_bTurnUp = false;
				break;
			case VK_U:
				m_bTurnS = false;
				break;
			case VK_O:
				m_bTurnS1 = false;
				break;
			case VK_1:
			case VK_4:
				m_lMoveSphere1 = 0;
				break;
			case VK_2:
			case VK_5:
				m_lMoveSphere2 = 0;
				break;
			/*case VK_1:
				m_bMoveSphere1 = false;
				break;
			case VK_2:
				m_bMoveSphere2 = false;
				break;*/
			default:
				break;
			}

		}
		break;
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
			case VK_LEFT:
				m_bLeft = true;
				break;
			case VK_RIGHT:
				m_bRight = true;
				break;
			case VK_UP:
				m_bForward = true;
				break;
			case VK_DOWN:
				m_bBackward = true;
				break;
			case VK_A:
				m_bUp = true;
				break;
			case VK_Q:
				m_bDown = true;
				break;
			case VK_J:
				m_bTurnLeft = true;
				break;
			case VK_L:
				m_bTurnRight = true;
				break;
			case VK_K:
				m_bTurnDown = true;
				break;
			case VK_I:
				m_bTurnUp = true;
				break;
			case VK_U:
				m_bTurnS = true;
				break;
			case VK_O:
				m_bTurnS1 = true;
				break;
			case VK_1:
				m_lMoveSphere1 = MOVE_OBJECT | MOVE_DOWN;
				break;
			case VK_2:
				m_lMoveSphere2 = MOVE_OBJECT | MOVE_DOWN;
				break;
			case VK_4:
				m_lMoveSphere1 = MOVE_OBJECT | MOVE_UP;
				break;
			case VK_5:
				m_lMoveSphere2 = MOVE_OBJECT | MOVE_UP;
				break;
			/*case VK_1:
				g_bMoveSphere1 = true;
				break;
			case VK_2:
				g_bMoveSphere2 = true;
				break;*/
			default:
				break;
			}

		}
		break;
	default:
		break;
	}

}
