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
#include "Cut/Plane.h"
#include "Collisions\BVH.h"
#include "ActionEvent.h"
#include "SCredits.h"


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

void CSOnGame::OnEntry(void)
{
	CSMain* main = (CSMain*)GetSuperState();
	VECTOR4D White = { 1, 1, 1, 1 };
	m_cameraEyePos = { 10, 22, 20, 1 };
	m_cameraTarget = { 10, 10, 0, 1 };
	m_cameraUp = { 0, 0, 1, 0 };

	printf("[HCM] %s:OnEntry\n", GetClassString());

	/* Create View, Projection and World Matrix */
	m_View = View(m_cameraEyePos, m_cameraTarget, m_cameraUp);
	m_Projection = PerspectiveWidthHeightLH(0.05, 0.05, 0.1, 100);
	m_World = Identity();

	/* Load surface */
	m_Surface.LoadSuzanne();
	//m_Surface.Optimize();
	m_Surface.BuildTangentSpaceFromTexCoordsIndexed(true);
	m_Surface.SetColor(White, White, White, White);

	/* Load Scene */
	char buffer[BUF_SIZE];
	int ret;
	ret = wcstombs(buffer, main->m_Params.scene, sizeof(buffer));
	LoadScene(buffer);
	m_Map.LoadMeshes();
	m_Map.LoadLevel(0, MAIN->m_numPlayers);

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

	pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\pasto.bmp", NULL);

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

	pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\ladrillonm.bmp", NULL);

	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	m_pNormalMapTrueLadrillo = pImage->CreateTexture(m_pDXManager);

	if (!m_pNormalMapTrueLadrillo)
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


	/* Create SRVs */
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pTexture, NULL, &m_pSRVTexture);
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pNormalMap, NULL, &m_pSRVNormalMap);
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pEnvMap, NULL, &m_pSRVEnvMap);
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pNormalMapTrue, NULL, &m_pSRVNormalMapTrue);
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pNormalMapTrueLadrillo, NULL, &m_pSRVNormalMapTrueLadrillo);
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pEmissiveMap, NULL, &m_pSRVEmissiveMap);

	/* Initialize camera options */
	m_bLeft = m_bRight =
		m_bUp = m_bDown =
		m_bForward = m_bBackward = m_bTurnLeft = m_bTurnRight =
		m_bTurnUp = m_bTurnDown = m_bTurnS = m_bTurnS1 = m_bTimerPlayer1 = m_bTimerPlayer2 = false;

	/* Init collisions structures */
	for (unsigned long i = 0; i < m_Scene.size(); i++)
	{
		m_Scene[i].m_octree = new COctree(m_Scene[i].m_Box.min, m_Scene[i].m_Box.max, 0, m_pDXPainter);
		m_Scene[i].m_octree->m_Color = { i % 2 ? 1.f : 0.f , 1,i % 3 ? 1.f : 0.f,0 };
	}

	/* Create timers */
	SetTimer(main->m_hWnd, MAP_TIMER_PLAYER1, 200, NULL);
	SetTimer(main->m_hWnd, MAP_TIMER_PLAYER2, 1000, NULL);
	SetTimer(main->m_hWnd, MAP_TIMER_CLOCK, 1000, NULL);

	/* Init clock */
	m_lClock = 20;

	/* Set state */
	m_lState = ON_GAME_STATE_GAMING;

	char* menuOption[ON_GAME_MENU_SIZE*BUTTON_STATE_SIZE] = {
		"..\\Assets\\OGContinueOptionUp.bmp" ,			//0
		"..\\Assets\\OGContinueOptionDown.bmp" ,		//0
		"..\\Assets\\OGContinueOptionOver.bmp" ,		//0
		"..\\Assets\\MMExitGameUp.bmp",					//1
		"..\\Assets\\MMExitGameDown.bmp",				//1
		"..\\Assets\\MMExitGameOver.bmp",				//1
	};

	/* Load Menu option */
	m_vMenu.resize(ON_GAME_MENU_SIZE);
	for (unsigned long i = 0; i < ON_GAME_MENU_SIZE; i++)
	{
		for (unsigned long j = 0; j < BUTTON_STATE_SIZE; j++)
		{
			CImageBMP* img = CImageBMP::CreateBitmapFromFile(menuOption[i * 3 + j], NULL);

			if (!img)
			{
				printf("Recurso %s no encontrado", menuOption[i * 3 + j]);
			}
			else
			{
				auto tex = img->CreateTexture(main->m_pDXManager);
				ID3D11ShaderResourceView* m_pSRV = NULL;
				main->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRV);
				m_vMenu[i].pSRV[j] = m_pSRV;
			}
		}
	}

	/* Set position and texcoord in start option */
	m_vMenu[ON_GAME_MENU_CONTINUE].frame[0].Position = { 0.2f, .75f, 0,1.0f };
	m_vMenu[ON_GAME_MENU_CONTINUE].frame[1].Position = { 0.8f, .75f ,0,1.0f };
	m_vMenu[ON_GAME_MENU_CONTINUE].frame[2].Position = { 0.2f, .5f  ,0,1.0f };
	m_vMenu[ON_GAME_MENU_CONTINUE].frame[3].Position = { 0.8f, .5f ,0,1.0f };

	m_vMenu[ON_GAME_MENU_CONTINUE].frame[0].TexCoord = { 0,0,0,0 };
	m_vMenu[ON_GAME_MENU_CONTINUE].frame[1].TexCoord = { 1,0,0,0 };
	m_vMenu[ON_GAME_MENU_CONTINUE].frame[2].TexCoord = { 0,1,0,0 };
	m_vMenu[ON_GAME_MENU_CONTINUE].frame[3].TexCoord = { 1,1,0,0 };

	m_vMenu[ON_GAME_MENU_CONTINUE].indices[0] = 0;
	m_vMenu[ON_GAME_MENU_CONTINUE].indices[1] = 1;
	m_vMenu[ON_GAME_MENU_CONTINUE].indices[2] = 2;
	m_vMenu[ON_GAME_MENU_CONTINUE].indices[3] = 2;
	m_vMenu[ON_GAME_MENU_CONTINUE].indices[4] = 1;
	m_vMenu[ON_GAME_MENU_CONTINUE].indices[5] = 3;

	m_vMenu[ON_GAME_MENU_CONTINUE].stateButton = BUTTON_OVER;

	m_lOptionSelected = ON_GAME_MENU_CONTINUE;

	/* Set position and texcoord in unirse option */
	m_vMenu[ON_GAME_MENU_SALIR].frame[0].Position = { 0.2f, .5f, 0,1.0f };
	m_vMenu[ON_GAME_MENU_SALIR].frame[1].Position = { 0.8f, .5f ,0,1.0f };
	m_vMenu[ON_GAME_MENU_SALIR].frame[2].Position = { 0.2f, .25f  ,0,1.0f };
	m_vMenu[ON_GAME_MENU_SALIR].frame[3].Position = { 0.8f, .25f ,0,1.0f };

	m_vMenu[ON_GAME_MENU_SALIR].frame[0].TexCoord = { 0,0,0,0 };
	m_vMenu[ON_GAME_MENU_SALIR].frame[1].TexCoord = { 1,0,0,0 };
	m_vMenu[ON_GAME_MENU_SALIR].frame[2].TexCoord = { 0,1,0,0 };
	m_vMenu[ON_GAME_MENU_SALIR].frame[3].TexCoord = { 1,1,0,0 };

	m_vMenu[ON_GAME_MENU_SALIR].indices[0] = 0;
	m_vMenu[ON_GAME_MENU_SALIR].indices[1] = 1;
	m_vMenu[ON_GAME_MENU_SALIR].indices[2] = 2;
	m_vMenu[ON_GAME_MENU_SALIR].indices[3] = 2;
	m_vMenu[ON_GAME_MENU_SALIR].indices[4] = 1;
	m_vMenu[ON_GAME_MENU_SALIR].indices[5] = 3;

	m_vMenu[ON_GAME_MENU_SALIR].stateButton = BUTTON_UP;

	/* Crear render target 1 */

	// Crear Render Target Auxiliar

	ID3D11Texture2D* pBackBuffer = 0;
	m_pDXManager->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);

	D3D11_TEXTURE2D_DESC dtd;
	pBackBuffer->GetDesc(&dtd);
	dtd.BindFlags |= (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

	m_pDXManager->GetDevice()->CreateTexture2D(&dtd, 0, &m_pRT1);
	m_pDXManager->GetDevice()->CreateShaderResourceView(m_pRT1, 0, &m_pSRV1);
	m_pDXManager->GetDevice()->CreateRenderTargetView(m_pRT1, 0, &m_pRTV1);

	SAFE_RELEASE(pBackBuffer);

	MAIN->m_pSndManager->ClearEngine();
	

	m_pSndBackground = main->m_pSndManager->LoadSoundFx(L"..\\Assets\\Sonidos\\wltdo.wav", ON_GAME_SOUNDS_BACKGROUND);
	m_pSndBackground->SetVolume(.8);
	if (m_pSndBackground)
		m_pSndBackground->Play(true);

	auto fx = main->m_pSndManager->LoadSoundFx(L"..\\Assets\\Sonidos\\notmove.wav", ON_GAME_SOUNDS_NOT_MOVE);

	if (fx)
		printf("Explosion load success \n");
	else
		printf("Explosion load fail\n");


	fx = main->m_pSndManager->LoadSoundFx(L"..\\Assets\\Sonidos\\youlose.wav", ON_GAME_SOUNDS_YOU_LOSE);

	if (fx)
		printf("Explosion load success \n");
	else
		printf("Explosion load fail\n");

	fx = main->m_pSndManager->LoadSoundFx(L"..\\Assets\\Sonidos\\perro.wav", ON_GAME_SOUNDS_DOG);

	if (fx)
		printf("Explosion load success \n");
	else
		printf("Explosion load fail\n");

	fx = main->m_pSndManager->LoadSoundFx(L"..\\Assets\\Sonidos\\perrollorando.wav", ON_GAME_SOUNDS_DOG_CRYING);

	if (fx)
		printf("Explosion load success \n");
	else
		printf("Explosion load fail\n");


}

unsigned long CSOnGame::OnEvent(CEventBase * pEvent)
{
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		CActionEvent *Action = (CActionEvent*)pEvent;
		MATRIX4D Camera = FastInverse(m_View);
		MATRIX4D Orientation = Camera;
		VECTOR4D Pos = Camera.vec[3];

		Orientation.vec[3] = { 0,0,0,1 };

		//if (JOY_AXIS_LY == Action->m_iAction)
		//{
		//	// Dead Zone
		//	VECTOR4D Dir = Camera.vec[2];
		//	float Stimulus = fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis;
		//	Pos = Pos + Dir*Stimulus*0.1;
		//}
		//if (JOY_AXIS_LX == Action->m_iAction)
		//{
		//	// Dead Zone
		//	VECTOR4D Dir = Camera.vec[0];
		//	float Stimulus = fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis;
		//	Pos = Pos + Dir*Stimulus*0.1;
		//}
		//if (JOY_AXIS_RX == Action->m_iAction)
		//{
		//	// Dead Zone
		//	float Stimulus = fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis;
		//	Orientation = Orientation * RotationAxis(Stimulus*0.01, Camera.vec[1]);
		//}
		//if (JOY_AXIS_RY == Action->m_iAction)
		//{
		//	// Dead Zone
		//	float Stimulus = fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis;
		//	Orientation = Orientation * RotationAxis(Stimulus*0.01, Camera.vec[0]);
		//}

		Camera.vec[0] = Orientation.vec[0];
		Camera.vec[1] = Orientation.vec[1];
		Camera.vec[2] = Orientation.vec[2];
		Camera.vec[3] = Pos;

		m_View = Orthogonalize(FastInverse(Camera));

		if (JOY_BUTTON_START_PRESSED == Action->m_iAction)
		{
			if (m_lState == ON_GAME_STATE_GAMING)
				m_lState = ON_GAME_STATE_PAUSE;
			else if (m_lState == ON_GAME_STATE_PAUSE)
				m_lState = ON_GAME_STATE_GAMING;
		}

	
		if (m_lState == ON_GAME_STATE_GAMING)
		{
			if (JOY_AXIS_LY == Action->m_iAction)
			{
				float Stimulus = fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis;
				bool move = false;
				static bool alreadyReproduce = false;

				if (fabs(Action->m_fAxis) < 0.2)
				{
					if (alreadyReproduce)
						alreadyReproduce = false;
					return 0;
				}
					

				if (Stimulus > 0.0f)
				{
					move = m_Map.MovePlayer(Action->m_nSource, PLAYER_MOVE_LESS_Y);
				}
				else
				{
					move = m_Map.MovePlayer(Action->m_nSource, PLAYER_MOVE_MORE_Y);
				}

				if (!move)
				{
					if (!alreadyReproduce)
					{
						MAIN->m_pSndManager->PlayFx(ON_GAME_SOUNDS_NOT_MOVE);
						alreadyReproduce = true;
					}
					
				}

			}
			if (JOY_AXIS_LX == Action->m_iAction)
			{
				float Stimulus = fabs(Action->m_fAxis) < 0.2 ? 0.0f : Action->m_fAxis;
				bool move = false;
				static bool alreadyReproduce = false;

				if (fabs(Action->m_fAxis) < 0.2)
				{
					if (alreadyReproduce)
						alreadyReproduce = false;
					return 0;
				}
					

				if (Stimulus > 0.0f)
				{
					move = m_Map.MovePlayer(Action->m_nSource, PLAYER_MOVE_MORE_X);
				}
					
				else
				{
					move = m_Map.MovePlayer(Action->m_nSource, PLAYER_MOVE_LESS_X);
				}
				if (!move)
				{
					if (!alreadyReproduce)
					{
						MAIN->m_pSndManager->PlayFx(ON_GAME_SOUNDS_NOT_MOVE);
						alreadyReproduce = true;
					}
					
				}
					
			}
			if (JOY_BUTTON_A_PRESSED == Action->m_iAction)
			{
				if (Action->m_nSource >= MAIN->m_numPlayers)
					return 0;
				if (!m_Map.PlayerHasTarget(Action->m_nSource))
				{
					if(m_Map.GetTarget(Action->m_nSource))
						MAIN->m_pSndManager->PlayFx(ON_GAME_SOUNDS_DOG);
				}
				else
				{
					if (m_Map.DropTarget(Action->m_nSource))
					{
						MAIN->m_pSndManager->PlayFx(ON_GAME_SOUNDS_DOG_CRYING);
					}
				}
					
			}
			
		}
		else if (m_lState == ON_GAME_STATE_PAUSE)
		{
			if (Action->m_iAction == JOY_BUTTON_A_PRESSED)
			{
				//m_pSMOwner->Transition(CLSID_CSOnGame);
				switch (m_lOptionSelected)
				{
				case ON_GAME_MENU_CONTINUE:
				{
					m_lState = ON_GAME_STATE_GAMING;
					return 0;
				}
				case ON_GAME_MENU_SALIR:
				{
					m_pSMOwner->Transition(CLSID_CSMainMenu);
					return 0;
				}
				default:
					break;
				}
				return 0;
				//MAIN->m_pSndManager->PlayFx(0);
			}
			if (JOY_AXIS_LY == Action->m_iAction && 0 == Action->m_nSource)
			{
				static bool alreadyMove = false;

				if (fabs(Action->m_fAxis) < 0.2)
				{
					if (alreadyMove)
						alreadyMove = false;
					return 0;
				}

				if (!alreadyMove)
				{
					if (Action->m_fAxis > 0.0f)
					{
						m_vMenu[m_lOptionSelected].stateButton = BUTTON_UP;

						m_lOptionSelected--;
						if (m_lOptionSelected < 0)
							m_lOptionSelected = ON_GAME_MENU_SIZE - 1;

						m_vMenu[m_lOptionSelected].stateButton = BUTTON_OVER;
						alreadyMove = true;
					}
					else
					{
						m_vMenu[m_lOptionSelected].stateButton = BUTTON_UP;

						m_lOptionSelected++;
						if (m_lOptionSelected >= ON_GAME_MENU_SIZE)
							m_lOptionSelected = 0;

						m_vMenu[m_lOptionSelected].stateButton = BUTTON_OVER;

						alreadyMove = true;
					}
				}
				return 0;
			}
			if (JOY_AXIS_LY == Action->m_iAction && 1 == Action->m_nSource)
			{
				static bool alreadyMove = false;

				if (fabs(Action->m_fAxis) < 0.2)
				{
					if (alreadyMove)
						alreadyMove = false;
					return 0;
				}

				if (!alreadyMove)
				{
					if (Action->m_fAxis > 0.0f)
					{
						m_vMenu[m_lOptionSelected].stateButton = BUTTON_UP;

						m_lOptionSelected--;
						if (m_lOptionSelected < 0)
							m_lOptionSelected = ON_GAME_MENU_SIZE - 1;

						m_vMenu[m_lOptionSelected].stateButton = BUTTON_OVER;
						alreadyMove = true;
					}
					else
					{
						m_vMenu[m_lOptionSelected].stateButton = BUTTON_UP;

						m_lOptionSelected++;
						if (m_lOptionSelected >= ON_GAME_MENU_SIZE)
							m_lOptionSelected = 0;

						m_vMenu[m_lOptionSelected].stateButton = BUTTON_OVER;

						alreadyMove = true;
					}
				}
				return 0;
			}
		}

		
	}
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		if (m_pDXManager->GetSwapChain())
		{
			m_pDXManager->GetContext()->ClearState();
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
			m_pDXManager->GetContext()->ClearRenderTargetView(m_pRTV1, (float*)&NightBlue);
			m_pDXManager->GetContext()->ClearDepthStencilView(
				m_pDXManager->GetMainDSV(),
				D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
				1.0f,
				0);

			// Draw
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

			/* Draw scene */
			m_pDXManager->GetContext()->OMSetBlendState(NULL, NULL, -1);
			m_Map.DrawMap(m_pDXPainter);

			m_pDXManager->GetContext()->ClearState();

			if (m_lState == ON_GAME_STATE_GAMING)
			{
				if (!m_Map.stillAreTargetsInMap())
				{
					m_lState = ON_GAME_STATE_WIN;
					statusAnimation = ON_GAME_GAMEOVER_ANIMATION_INIT;
					/* start timer */
					SetTimer(MAIN->m_hWnd, MAP_TIMER_GAME_OVER, 10000, NULL);
				}
				else if (m_lClock < 1 && m_Map.stillAreTargetsInMap())
				{
					m_lState = ON_GAME_STATE_LOSE;
					statusAnimation = ON_GAME_GAMEOVER_ANIMATION_INIT;
					/* start timer */
					SetTimer(MAIN->m_hWnd, MAP_TIMER_GAME_OVER, 10000, NULL);
				}
			}

			if (m_lState == ON_GAME_STATE_WIN  || m_lState == ON_GAME_STATE_LOSE)
			{
				switch (statusAnimation)
				{
				case ON_GAME_GAMEOVER_ANIMATION_INIT:
				{
					Position playerPos = m_Map.GetPlayerPos(0);
					VECTOR4D Target = { playerPos.x * 2.f, playerPos.y * 2.f, 0 ,1 };
					VECTOR4D EyePos = { (playerPos.x)*2.f, (playerPos.y + 1)*2.f, 0, 1 };

					incrementosEyepos.x = -(m_cameraEyePos.x - EyePos.x) / GAME_OVER_STEPS;
					incrementosEyepos.y = -(m_cameraEyePos.y - EyePos.y) / GAME_OVER_STEPS;
					incrementosEyepos.z = -(m_cameraEyePos.z - EyePos.z) / GAME_OVER_STEPS;
					incrementosEyepos.w = 0;

					incrementosTarget.x = -(m_cameraTarget.x - Target.x) / GAME_OVER_STEPS;
					incrementosTarget.y = -(m_cameraTarget.y - Target.y) / GAME_OVER_STEPS;
					incrementosTarget.z = -(m_cameraTarget.z - Target.z) / GAME_OVER_STEPS;
					incrementosTarget.w = 0;

					statusAnimation = ON_GAME_GAMEOVER_ANIMATION_ON;
					steps = 0;

					if (m_lState == ON_GAME_STATE_LOSE)
					{
						m_pSndBackground->Stop();
						MAIN->m_pSndManager->PlayFx(ON_GAME_SOUNDS_YOU_LOSE);
					}
				}
					break;
				case ON_GAME_GAMEOVER_ANIMATION_ON:
				{
					
					m_cameraEyePos = m_cameraEyePos + incrementosEyepos;
					m_cameraTarget = m_cameraTarget + incrementosTarget;


					/* Create View, Projection and World Matrix */
					m_View = View(m_cameraEyePos, m_cameraTarget, m_cameraUp);

					steps++;
					if (steps >= GAME_OVER_STEPS)
						statusAnimation = ON_GAME_GAMEOVER_ANIMATION_DONE;
				}
					break;
				case ON_GAME_GAMEOVER_ANIMATION_DONE:
				{

				}
					break;
				default:
					break;
				}

			}

			switch (m_lState)
			{
			case ON_GAME_STATE_GAMING:
			{
				MATRIX4D ST =
					Translation(0.5, -0.5, 0)*
					Scaling(0.05, 0.1, 1) *
					Translation(0, .9, 0);
				//RotationZ(3.141592 / 4)*
				//Translation(-1, 1, 0);

				char time[3];

				time[0] = ((m_lClock / 10) % 10) + 48;
				time[1] = (m_lClock % 10) + 48;
				time[2] = '\0';

				MAIN->m_pTextRender->RenderText(ST, time);
			}
			break;
			case ON_GAME_STATE_LOSE:
			{
				MATRIX4D ST =
					Translation(0.5, -0.5, 0)*
					Scaling(0.1, 0.1, 1) *
					Translation(0, .9, 0);

				/* You lose */
				MAIN->m_pTextRender->RenderText(ST, "You lose!!");
			}
			break;
			case ON_GAME_STATE_WIN:
			{
				MATRIX4D ST =
					Translation(0.5, -0.5, 0)*
					Scaling(0.1, 0.1, 1) *
					Translation(0, .9, 0);

				/* You win */
				MAIN->m_pTextRender->RenderText(ST, "You win!!");

			}
			break;
			case ON_GAME_STATE_PAUSE:
			{

				ID3D11Texture2D* pBackBuffer = 0;
				D3D11_TEXTURE2D_DESC dtd;


				m_pDXManager->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
				pBackBuffer->GetDesc(&dtd);

				//MAIN->m_FX->SetRenderTarget(m_pDXManager->GetMainRTV());
				//MAIN->m_FX->SetInput(m_pSRV1);
				//MAIN->m_FX->Process(0, FX_EDGE_DETECT, dtd.Width, dtd.Height);

				MAIN->m_FX->m_Params.WVP = Identity();

				for (unsigned long i = 0; i < ON_GAME_MENU_SIZE; i++)
				{
					MAIN->m_FX->SetImgVertex(m_vMenu[i].frame, m_vMenu[i].indices);
					MAIN->m_FX->m_Params.Brightness = { 0,0,0,0 };
					MAIN->m_FX->SetRenderTarget(m_pDXManager->GetMainRTV());
					MAIN->m_FX->SetInput(m_vMenu[i].pSRV[m_vMenu[i].stateButton]);
					MAIN->m_FX->Process(0, FX_NONE, dtd.Width, dtd.Height, FX_FLAGS_USE_IMG_BUFFR);
				}

				SAFE_RELEASE(pBackBuffer);
			}
			default:
				break;
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
			if (pWin32->m_wParam == '8')
			{
				m_pSMOwner->Transition(CLSID_CSIntro);
				CSMain* main = (CSMain*)GetSuperState();
				InvalidateRect(main->m_hWnd, NULL, false);
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
			if (pWin32->m_wParam == '0')
			{
				m_lFlags ^= PHYSICS_DRAW_OCTREE;
				return 0;
			}
			break;
		case WM_TIMER:
			switch (pWin32->m_wParam)
			{
			case MAP_TIMER_PLAYER1:
			{
				m_bTimerPlayer1 = true;
				return 0;
			}
			case MAP_TIMER_CLOCK:
			{
				if (m_lState == ON_GAME_STATE_GAMING)
					m_lClock--;
				return 0;
			}
			case MAP_TIMER_GAME_OVER:
			{
				KillTimer(MAIN->m_hWnd, MAP_TIMER_GAME_OVER);
				if (!m_Map.HasMoreLevels())
					m_pSMOwner->Transition(CLSID_CSCredits);
				else
				{
					if (m_lState == ON_GAME_STATE_WIN)
						m_Map.LoadNextLevel(MAIN->m_numPlayers);
					else
						m_Map.ResetLevel(MAIN->m_numPlayers);
					
					m_lState = ON_GAME_STATE_GAMING;
					m_lClock = 20;

					m_cameraEyePos = { 10, 22, 20, 1 };
					m_cameraTarget = { 10, 10, 0, 1 };
					m_cameraUp = { 0, 0, 1, 0 };

					m_View = View(m_cameraEyePos, m_cameraTarget, m_cameraUp);
					m_pSndBackground->Restart();
					m_pSndBackground->Play(true);
				}
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
	KillTimer(main->m_hWnd, MAP_TIMER_PLAYER1);
	KillTimer(main->m_hWnd, MAP_TIMER_PLAYER2);
	KillTimer(main->m_hWnd, MAP_TIMER_CLOCK);

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

	m_Scene.resize(scene->mNumMeshes);

	for (unsigned long i = 0; i < scene->mNumMeshes; i++)
	{
		float maxX, maxY, maxZ;
		float minX, minY, minZ;

		maxX = maxY = maxZ = FLT_MIN;
		minX = minY = minZ = FLT_MAX;

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

		m_Scene[i].m_Vertices.resize(scene->mMeshes[i]->mNumVertices);
		for (unsigned long j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
		{
			m_Scene[i].m_Vertices[j].Position = {
				scene->mMeshes[i]->mVertices[j].x,
				scene->mMeshes[i]->mVertices[j].y,
				scene->mMeshes[i]->mVertices[j].z,
				1 };

			m_Scene[i].m_Vertices[j].Position = m_Scene[i].m_Vertices[j].Position * m_Scene[i].m_World;
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

		/* Set id */
		m_Scene[i].m_lID = i;
		strcpy(m_Scene[i].m_cName, scene->mMeshes[i]->mName.C_Str());
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
