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
#include "SIntro.h"
#include "Graphics\ImageBMP.h"
#include "SMain.h"

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

void CSOnGame::OnEntry(void)
{
	CSMain* main = (CSMain*)GetSuperState();
	VECTOR4D White = { 1, 1, 1, 1 };
	VECTOR4D EyePos = { 6, 10, 6, 1 };
	VECTOR4D Target = { 0, 0, 0, 1 };
	VECTOR4D Up = { 0, 0, 1, 0 };

	/* Create View, Projection and World Matrix */
	m_View = View(EyePos, Target, Up);
	m_Projection = PerspectiveWidthHeightLH(0.05, 0.05, 0.1, 100);
	m_World = Identity();

	/* Load surface */
	m_Surface.LoadSuzanne();
	//m_Surface.Optimize();
	m_Surface.BuildTangentSpaceFromTexCoordsIndexed(true);
	m_Surface.SetColor(White, White, White, White);

	LoadScene("..\\Assets\\spheres.blend");

	/* Load pointers */
	m_pDXManager = main->m_pDXManager;
	m_pDXPainter = main->m_pDXPainter;
	m_hWnd = main->m_hWnd;

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



}

unsigned long CSOnGame::OnEvent(CEventBase * pEvent)
{
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			if (pWin32->m_wParam == 'a')
			{
				m_pSMOwner->Transition(CLSID_CSIntro);
				CSMain* main = (CSMain*)GetSuperState();
				InvalidateRect(main->m_hWnd, NULL, false);
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
			ManageKeyboardEvents(pWin32->m_msg, pWin32->m_wParam);
			return 0;
		case WM_PAINT:

			if (m_pDXManager->GetSwapChain())
			{

				ID3D11Texture2D* pBackBuffer = 0;
				MATRIX4D AC; /* Matriz de correction de aspecto */
				// Colors
				VECTOR4D DarkGray = { 0.25,0.25,0.25,1 };
				VECTOR4D White = { 1,1,1,1 };
				VECTOR4D Gray = { .5,.5,.5,0 };
				VECTOR4D NightBlue = { 0,0,.1, 0 };
				VECTOR4D Black = { 0, 0, 0, 0 };
				D3D11_TEXTURE2D_DESC dtd;

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

				/* Clear main render target */
				m_pDXManager->GetContext()->ClearRenderTargetView(m_pDXManager->GetMainRTV(), (float*)&NightBlue);
				m_pDXManager->GetContext()->ClearDepthStencilView(
					m_pDXManager->GetMainDSV(),
					D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
					1.0f,
					0);

				/* Set SRVs */
				m_pDXManager->GetContext()->PSSetShaderResources(0, 1, &m_pSRVTexture);
				m_pDXManager->GetContext()->PSSetShaderResources(1, 1, &m_pSRVNormalMap);
				m_pDXManager->GetContext()->PSSetShaderResources(2, 1, &m_pSRVEnvMap);
				m_pDXManager->GetContext()->PSSetShaderResources(3, 1, &m_pSRVNormalMapTrue);
				m_pDXManager->GetContext()->PSSetShaderResources(4, 1, &m_pSRVEmissiveMap);

				// Actualizar camara si fue movida
				UpdateCamera();

				/* Set params */
				m_pDXPainter->m_Params.Brightness = Black;
				m_pDXPainter->m_Params.Flags1 = MAPPING_DIFFUSE ;
				
				m_pDXPainter->m_Params.World = m_World;
				m_pDXPainter->m_Params.View = m_View;
				m_pDXPainter->m_Params.Projection = m_Projection*AC;

				/* Render with Left Hand*/
				m_pDXManager->GetContext()->RSSetState(m_pDXPainter->GetDrawLHRState());

				/* Draw scene */
				for (unsigned long i = 0; i < m_Scene.size(); i++)
				{
					m_pDXPainter->m_Params.World = m_Scene[i].m_World;
					m_pDXPainter->DrawIndexed(&m_Scene[i].m_Vertices[0], m_Scene[i].m_Vertices.size(), &m_Scene[i].m_Indices[0], m_Scene[i].m_Indices.size(), PAINTER_DRAW);
				}
				
				/* Draw surface */
				/*m_pDXPainter->DrawIndexed(&m_Surface.m_Vertices[0], 
					m_Surface.m_Vertices.size(), 
					&m_Surface.m_Indices[0], 
					m_Surface.m_Indices.size(), 
					PAINTER_DRAW);*/

				m_pDXManager->GetSwapChain()->Present(1, 0);

			}
			
			break;
		
		default:
			break;
		}
	}
	return __super::OnEvent(pEvent);
}

void CSOnGame::OnExit(void)
{
	CSMain* main = (CSMain*)GetSuperState();

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

void CSOnGame::LoadScene(char * filename)
{
	/* the global Assimp scene object */
	const struct aiScene* scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);

	m_Scene.resize(scene->mNumMeshes);
	for (unsigned long i = 0; i < scene->mNumMeshes; i++)
	{
		float maxX, maxY, maxZ;
		float minX, minY, minZ;

		maxX = maxY = maxZ = FLT_MIN;
		minX = minY = minZ = FLT_MAX;

		m_Scene[i].m_Vertices.resize(scene->mMeshes[i]->mNumVertices);
		for (unsigned long j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
		{
			m_Scene[i].m_Vertices[j].Position = {
				scene->mMeshes[i]->mVertices[j].x,
				scene->mMeshes[i]->mVertices[j].y,
				scene->mMeshes[i]->mVertices[j].z,
				1 };
			if (scene->mMeshes[i]->mVertices[j].x > maxX)
				maxX = scene->mMeshes[i]->mVertices[j].x;
			if (scene->mMeshes[i]->mVertices[j].y > maxY)
				maxY = scene->mMeshes[i]->mVertices[j].y;
			if (scene->mMeshes[i]->mVertices[j].z > maxZ)
				maxZ = scene->mMeshes[i]->mVertices[j].z;

			if (scene->mMeshes[i]->mVertices[j].x < minX)
				minX = scene->mMeshes[i]->mVertices[j].x;
			if (scene->mMeshes[i]->mVertices[j].y < minY)
				minY = scene->mMeshes[i]->mVertices[j].y;
			if (scene->mMeshes[i]->mVertices[j].z < minZ)
				minZ = scene->mMeshes[i]->mVertices[j].z;
		}

		m_Scene[i].m_Box.min = { minX, minY, minZ, 1 };
		m_Scene[i].m_Box.max = { maxX, maxY, maxZ, 1 };

		m_Scene[i].m_octree = new COctree(m_Scene[i].m_Box.min, m_Scene[i].m_Box.max, 0, NULL);
		m_Scene[i].m_octree->m_Color = { i % 2 ? 1.f : 0.f , 1,i % 3 ? 1.f : 0.f,0 };

		MATRIX4D t;
		t.m00 = scene->mRootNode->mChildren[i]->mTransformation.a1;
		t.m01 = scene->mRootNode->mChildren[i]->mTransformation.a2;
		t.m02 = scene->mRootNode->mChildren[i]->mTransformation.a3;
		t.m03 = scene->mRootNode->mChildren[i]->mTransformation.a4;
		t.m10 = scene->mRootNode->mChildren[i]->mTransformation.b1;
		t.m11 = scene->mRootNode->mChildren[i]->mTransformation.b2;
		t.m12 = scene->mRootNode->mChildren[i]->mTransformation.b3;
		t.m13 = scene->mRootNode->mChildren[i]->mTransformation.b4;
		t.m20 = scene->mRootNode->mChildren[i]->mTransformation.c1;
		t.m21 = scene->mRootNode->mChildren[i]->mTransformation.c2;
		t.m22 = scene->mRootNode->mChildren[i]->mTransformation.c3;
		t.m23 = scene->mRootNode->mChildren[i]->mTransformation.c4;
		t.m30 = scene->mRootNode->mChildren[i]->mTransformation.d1;
		t.m31 = scene->mRootNode->mChildren[i]->mTransformation.d2;
		t.m32 = scene->mRootNode->mChildren[i]->mTransformation.d3;
		t.m33 = scene->mRootNode->mChildren[i]->mTransformation.d4;

		m_Scene[i].m_World = Transpose(t);

		m_Scene[i].m_Indices.resize(scene->mMeshes[i]->mNumFaces * scene->mMeshes[i]->mFaces[0].mNumIndices);
		for (unsigned long j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
		{
			for (unsigned long k = 0; k < scene->mMeshes[i]->mFaces[j].mNumIndices; k++)
			{
				m_Scene[i].m_Indices[j*scene->mMeshes[i]->mFaces[j].mNumIndices + k] = scene->mMeshes[i]->mFaces[j].mIndices[k];
			}

		}
		for (unsigned long j = 0; j < m_Scene[i].m_Vertices.size(); j++)
		{
			VECTOR4D TexCoord = { 0,0,0,0 };
			TexCoord.x = m_Scene[i].m_Vertices[j].Position.x;
			TexCoord.y = m_Scene[i].m_Vertices[j].Position.z;
			TexCoord.z = m_Scene[i].m_Vertices[j].Position.y;
			TexCoord = Normalize(TexCoord);
			TexCoord.x = TexCoord.x * 0.5 + 0.5;
			TexCoord.y = TexCoord.y * 0.5 + 0.5;

			m_Scene[i].m_Vertices[j].TexCoord = TexCoord;
		}
		//g_Scene[i].Optimize();
		m_Scene[i].BuildTangentSpaceFromTexCoordsIndexed(true);
		m_Scene[i].GenerarCentroides();
	}
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



	//if (g_onFirstMouseMove)
	//{
	//	lastX = g_iWidth / 2;
	//	lastY = g_iHeight / 2;

	//	g_onFirstMouseMove = false;
	//}
	//else
	//{

	//	if (mouseX - lastX != 0)
	//	{
	//		float diffX = (float)(mouseX)-(lastX);
	//		diffX /= g_iWidth / 2;

	//		MATRIX4D R = RotationAxis(-speed*(diffX > 0 ? 1 : -1), YDir);
	//		//O = O*R;

	//	}

	//	if (mouseY - lastY != 0)
	//	{
	//		float diffY = (float)mouseY - lastY;
	//		diffY /= g_iHeight / 2;

	//		MATRIX4D R = RotationAxis(-speed*(diffY > 0 ? 1 : 0), XDir);
	//		//O = O*R;
	//	}

	//	lastX = mouseX;
	//	lastY = mouseY;

	//}


	InvV = O;

	InvV.m30 = EyePos.x;
	InvV.m31 = EyePos.y;
	InvV.m32 = EyePos.z;

	m_View = Orthogonalize(FastInverse(InvV));
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
