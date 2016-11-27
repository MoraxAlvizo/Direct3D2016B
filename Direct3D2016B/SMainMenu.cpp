#include "stdafx.h"
#include "SMainMenu.h"
#include "HSM\EventBase.h"
#include "HSM\StateMachineManager.h"
#include "HSM\EventWin32.h"
#include "ActionEvent.h"
#include "SMain.h"
#include "Graphics\ImageBMP.h"
#include "SOnGame.h"

CSMainMenu::CSMainMenu()
{
	m_pSRVBackGround = NULL;
	m_pSRVExitGame = NULL;
	m_pSRVStartGame = NULL;
}


CSMainMenu::~CSMainMenu()
{
}

void CSMainMenu::OnEntry(void)
{
	CSMain* main = (CSMain*)GetSuperState();
	/* Load pointers */
	m_pDXManager = main->m_pDXManager;
	m_pDXPainter = main->m_pDXPainter;
	m_FX = main->m_FX;
	printf("[HCM] %s:OnEntry\n", GetClassString());
	m_nOption = 0;
	m_fOffsetX = m_fOffsetY = 0.0f;
	printf("Cargando recursos de fondo ... \n");

	char* menuOption[MAIN_MENU_SIZE*BUTTON_STATE_SIZE] = {
		"..\\Assets\\MMStartGameOptionUp.bmp" ,			//0
		"..\\Assets\\MMStartGameOptionDown.bmp" ,		//0
		"..\\Assets\\MMStartGameOptionOver.bmp" ,		//0
		"..\\Assets\\MMUnirseGameUp.bmp" ,				//1
		"..\\Assets\\MMUnirseGameDown.bmp" ,			//1
		"..\\Assets\\MMUnirseGameOver.bmp" ,			//1
		"..\\Assets\\MMExitGameUp.bmp",					//2
		"..\\Assets\\MMExitGameDown.bmp",				//2
		"..\\Assets\\MMExitGameOver.bmp",				//2
	};

	
	CImageBMP* img = CImageBMP::CreateBitmapFromFile("..\\Assets\\MainMenu.bmp", NULL);

	if (!img)
	{
		printf("Recurso ..\\Assets\\MainMenu.bmp no encontrado\n");
	}
	else
	{
		auto tex = img->CreateTexture(main->m_pDXManager);
		main->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVBackGround);
	}
	

	/* Load Menu option */
	m_vMenu.resize(MAIN_MENU_SIZE);
	for(unsigned long i = 0; i <MAIN_MENU_SIZE;i++)
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
	m_vMenu[MAIN_MENU_START].frame[0].Position = { 0.2f, .75f, 0,1.0f };
	m_vMenu[MAIN_MENU_START].frame[1].Position = { 0.8f, .75f ,0,1.0f };
	m_vMenu[MAIN_MENU_START].frame[2].Position = { 0.2f, .5f  ,0,1.0f };
	m_vMenu[MAIN_MENU_START].frame[3].Position = { 0.8f, .5f ,0,1.0f };

	m_vMenu[MAIN_MENU_START].frame[0].TexCoord = { 0,0,0,0 };
	m_vMenu[MAIN_MENU_START].frame[1].TexCoord = { 1,0,0,0 };
	m_vMenu[MAIN_MENU_START].frame[2].TexCoord = { 0,1,0,0 };
	m_vMenu[MAIN_MENU_START].frame[3].TexCoord = { 1,1,0,0 };

	m_vMenu[MAIN_MENU_START].indices[0] = 0;
	m_vMenu[MAIN_MENU_START].indices[1] = 1;
	m_vMenu[MAIN_MENU_START].indices[2] = 2;
	m_vMenu[MAIN_MENU_START].indices[3] = 2;
	m_vMenu[MAIN_MENU_START].indices[4] = 1;
	m_vMenu[MAIN_MENU_START].indices[5] = 3;

	m_vMenu[MAIN_MENU_START].stateButton = BUTTON_OVER;

	m_lOptionSelected = MAIN_MENU_START;

	/* Set position and texcoord in unirse option */
	m_vMenu[MAIN_MENU_UNIRSE].frame[0].Position = { 0.2f, .5f, 0,1.0f };
	m_vMenu[MAIN_MENU_UNIRSE].frame[1].Position = { 0.8f, .5f ,0,1.0f };
	m_vMenu[MAIN_MENU_UNIRSE].frame[2].Position = { 0.2f, .25f  ,0,1.0f };
	m_vMenu[MAIN_MENU_UNIRSE].frame[3].Position = { 0.8f, .25f ,0,1.0f };

	m_vMenu[MAIN_MENU_UNIRSE].frame[0].TexCoord = { 0,0,0,0 };
	m_vMenu[MAIN_MENU_UNIRSE].frame[1].TexCoord = { 1,0,0,0 };
	m_vMenu[MAIN_MENU_UNIRSE].frame[2].TexCoord = { 0,1,0,0 };
	m_vMenu[MAIN_MENU_UNIRSE].frame[3].TexCoord = { 1,1,0,0 };

	m_vMenu[MAIN_MENU_UNIRSE].indices[0] = 0;
	m_vMenu[MAIN_MENU_UNIRSE].indices[1] = 1;
	m_vMenu[MAIN_MENU_UNIRSE].indices[2] = 2;
	m_vMenu[MAIN_MENU_UNIRSE].indices[3] = 2;
	m_vMenu[MAIN_MENU_UNIRSE].indices[4] = 1;
	m_vMenu[MAIN_MENU_UNIRSE].indices[5] = 3;

	m_vMenu[MAIN_MENU_UNIRSE].stateButton = BUTTON_UP;

	/* Set position and texcoord in exit option */
	m_vMenu[MAIN_MENU_EXIT].frame[0].Position = { 0.2f, .25f, 0,1.0f };
	m_vMenu[MAIN_MENU_EXIT].frame[1].Position = { 0.8f, .25f ,0,1.0f };
	m_vMenu[MAIN_MENU_EXIT].frame[2].Position = { 0.2f, 0.f  ,0,1.0f };
	m_vMenu[MAIN_MENU_EXIT].frame[3].Position = { 0.8f, 0.f ,0,1.0f };

	m_vMenu[MAIN_MENU_EXIT].frame[0].TexCoord = { 0,0,0,0 };
	m_vMenu[MAIN_MENU_EXIT].frame[1].TexCoord = { 1,0,0,0 };
	m_vMenu[MAIN_MENU_EXIT].frame[2].TexCoord = { 0,1,0,0 };
	m_vMenu[MAIN_MENU_EXIT].frame[3].TexCoord = { 1,1,0,0 };

	m_vMenu[MAIN_MENU_EXIT].indices[0] = 0;
	m_vMenu[MAIN_MENU_EXIT].indices[1] = 1;
	m_vMenu[MAIN_MENU_EXIT].indices[2] = 2;
	m_vMenu[MAIN_MENU_EXIT].indices[3] = 2;
	m_vMenu[MAIN_MENU_EXIT].indices[4] = 1;
	m_vMenu[MAIN_MENU_EXIT].indices[5] = 3;

	m_vMenu[MAIN_MENU_EXIT].stateButton = BUTTON_UP;

	for (unsigned long i = 0; i < MAIN_MENU_SIZE; i++)
	{
		m_vMenu[i].frame[0].Position = m_vMenu[i].frame[0].Position * Translation(0, -0.5,0);
		m_vMenu[i].frame[1].Position = m_vMenu[i].frame[1].Position * Translation(0, -0.5, 0);
		m_vMenu[i].frame[2].Position = m_vMenu[i].frame[2].Position * Translation(0, -0.5, 0);
		m_vMenu[i].frame[3].Position = m_vMenu[i].frame[3].Position * Translation(0, -0.5, 0);
	}

	/* Load text */
	spriteBatch = new SpriteBatch(m_pDXManager->GetContext());
	spriteFont = new SpriteFont(m_pDXManager->GetDevice(), L"..\\Assets\\myfileb.spritefont");

	// Initializar el render text,
	m_pTextRender = new CDXTextRender(m_pDXManager, m_pDXPainter, m_FX);
	m_pTextRender->Initialize();

}

unsigned long CSMainMenu::OnEvent(CEventBase * pEvent)
{
	if (ACTION_EVENT == pEvent->m_ulEventType)
	{
		CActionEvent* pAction = (CActionEvent*)pEvent;
		if (pAction->m_iAction == JOY_BUTTON_A_PRESSED)
		{
			//m_pSMOwner->Transition(CLSID_CSOnGame);
			switch (m_lOptionSelected)
			{
			case MAIN_MENU_START:
			{
				m_pSMOwner->Transition(CLSID_CSOnGame);
				CSMain* main = (CSMain*)GetSuperState();
				InvalidateRect(main->m_hWnd, NULL, false);
				return 0;
			}
			case MAIN_MENU_EXIT:
			{
				m_pSMOwner->Transition(CLSID_CStateNull);
				PostQuitMessage(0);
				return 0;
			}
			default:
				break;
			}
			return 0;
			//MAIN->m_pSndManager->PlayFx(0);
		}
		/*if (JOY_AXIS_LX == pAction->m_iAction)
		{
			m_fOffsetX = m_fOffsetX + (pAction->m_fAxis - m_fOffsetX) * 0.1;
		}*/
		if (JOY_AXIS_LY == pAction->m_iAction)
		{
			static bool alreadyMove = false;

			if (fabs(pAction->m_fAxis) < 0.2)
			{
				if (alreadyMove)
					alreadyMove = false;
				return 0;
			}
				
			if (!alreadyMove)
			{
				if (pAction->m_fAxis > 0.0f)
				{
					m_vMenu[m_lOptionSelected].stateButton = BUTTON_UP;

					m_lOptionSelected--;
					if (m_lOptionSelected < 0)
						m_lOptionSelected = MAIN_MENU_SIZE - 1;

					m_vMenu[m_lOptionSelected].stateButton = BUTTON_OVER;
					alreadyMove = true;
				}
				else
				{
					m_vMenu[m_lOptionSelected].stateButton = BUTTON_UP;

					m_lOptionSelected++;
					if (m_lOptionSelected >= MAIN_MENU_SIZE)
						m_lOptionSelected = 0;

					m_vMenu[m_lOptionSelected].stateButton = BUTTON_OVER;

					alreadyMove = true;
				}
			}
			return 0;
		}
	}
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		if (m_pDXManager->GetSwapChain())
		{
			ID3D11Texture2D* pBackBuffer = 0;
			D3D11_TEXTURE2D_DESC dtd;
			

			m_pDXManager->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
			pBackBuffer->GetDesc(&dtd);
			m_pDXManager->GetContext()->PSSetShaderResources(0, 1, &m_pSRVBackGround);

			m_FX->m_Params.WVP = Translation(m_fOffsetX, m_fOffsetY, 0);
			m_FX->SetRenderTarget(m_pDXManager->GetMainRTV());
			m_FX->m_Params.Brightness = { 0,0,0,0 };
			m_FX->SetInput(m_pSRVBackGround);
			m_FX->Process(0, FX_NONE, dtd.Width, dtd.Height);

			m_FX->m_Params.WVP =Identity();
			
			for (unsigned long i = 0; i < MAIN_MENU_SIZE; i++)
			{
				m_FX->SetImgVertex(m_vMenu[i].frame, m_vMenu[i].indices);
				m_FX->m_Params.Brightness = { 0,0,0,0 };
				m_FX->SetRenderTarget(m_pDXManager->GetMainRTV());
				m_FX->SetInput(m_vMenu[i].pSRV[m_vMenu[i].stateButton]);
				m_FX->Process(0, FX_NONE, dtd.Width, dtd.Height, FX_FLAGS_USE_IMG_BUFFR);
			}
			
			m_pDXManager->GetSwapChain()->Present(1, 0);

			SAFE_RELEASE(pBackBuffer);

		}
	}
	else if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			break;
		case WM_KEYDOWN:
			switch (pWin32->m_wParam)
			{
			case VK_RETURN:
				m_vMenu[m_lOptionSelected].stateButton = BUTTON_DOWN;
				break;
			}
			break;
		case WM_KEYUP:
		{
			switch (pWin32->m_wParam)
			{
			case VK_UP:
				m_vMenu[m_lOptionSelected].stateButton = BUTTON_UP;

				m_lOptionSelected--;
				if (m_lOptionSelected < 0)
					m_lOptionSelected = MAIN_MENU_SIZE - 1;

				m_vMenu[m_lOptionSelected].stateButton = BUTTON_OVER;
				break;
			case VK_DOWN:

				m_vMenu[m_lOptionSelected].stateButton = BUTTON_UP;

				m_lOptionSelected++;
				if (m_lOptionSelected >= MAIN_MENU_SIZE)
					m_lOptionSelected = 0;

				m_vMenu[m_lOptionSelected].stateButton = BUTTON_OVER;
				
				break;
			case VK_RETURN:
				switch (m_lOptionSelected)
				{
				case MAIN_MENU_START:
				{
					m_pSMOwner->Transition(CLSID_CSOnGame);
					CSMain* main = (CSMain*)GetSuperState();
					InvalidateRect(main->m_hWnd, NULL, false);
					return 0;
				}
				case MAIN_MENU_EXIT:
				{
					m_pSMOwner->Transition(CLSID_CStateNull);
					PostQuitMessage(0);
					return 0;
				}
				default:
					break;
				}
				break;
			}
		}
		break;
		
		default:
			break;
		}
	}
	return __super::OnEvent(pEvent);
}

void CSMainMenu::OnExit(void)
{
	printf("[HCM] %s:OnExit\n", GetClassString());

	SAFE_RELEASE(m_pSRVBackGround);
	SAFE_RELEASE(m_pSRVStartGame);
	SAFE_RELEASE(m_pSRVExitGame);
	m_pTextRender->Uninitialize();
	SAFE_DELETE(m_pTextRender);
}
