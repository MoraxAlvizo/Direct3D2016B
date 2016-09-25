#include "stdafx.h"
#include "SMainMenu.h"
#include "HSM\EventBase.h"
#include "HSM\StateMachineManager.h"
#include "HSM\EventWin32.h"
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
	printf("Cargando recursos de fondo ... \n");

	char* menuOption[MAIN_MENU_SIZE] = {
		"..\\Assets\\MMStartGameOption.bmp" ,			//0
		"..\\Assets\\MMExitGame.bmp",					//1
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
		CImageBMP* img = CImageBMP::CreateBitmapFromFile(menuOption[i], NULL);

		if (!img)
		{
			printf("Recurso %s no encontrado", menuOption[i]);
		}
		else
		{
			auto tex = img->CreateTexture(main->m_pDXManager);
			ID3D11ShaderResourceView* m_pSRV = NULL;
			main->m_pDXManager->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRV);
			m_vMenu[i].pSRV = m_pSRV;
		}
	}
	
	/* Set position and texcoord in start option */
	m_vMenu[MAIN_MENU_START].frame[0].Position = { -0.5f, 2.0f / 3.0f,0,1.0f };
	m_vMenu[MAIN_MENU_START].frame[1].Position = { 0.5f, 2.0f / 3.0f ,0,1.0f };
	m_vMenu[MAIN_MENU_START].frame[2].Position = { -0.5f,1.0f / 3.0f,0,1.0f };
	m_vMenu[MAIN_MENU_START].frame[3].Position = { 0.5f, 1.0f / 3.0f,0,1.0f };

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

	m_lOptionSelected = MAIN_MENU_START;

	/* Set position and texcoord in exit option */

	m_vMenu[MAIN_MENU_EXIT].frame[0].Position = { -0.5f, -1.0f / 3.0f,0,1.0f };
	m_vMenu[MAIN_MENU_EXIT].frame[1].Position = { 0.5f, -1.0f / 3.0f ,0,1.0f };
	m_vMenu[MAIN_MENU_EXIT].frame[2].Position = { -0.5f,-2.0f / 3.0f,0,1.0f };
	m_vMenu[MAIN_MENU_EXIT].frame[3].Position = { 0.5f, -2.0f / 3.0f,0,1.0f };

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
}

unsigned long CSMainMenu::OnEvent(CEventBase * pEvent)
{
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			break;
		case WM_KEYUP:
		{
			switch (pWin32->m_wParam)
			{
			case VK_UP:
				m_lOptionSelected++;
				if (m_lOptionSelected >= MAIN_MENU_SIZE)
					m_lOptionSelected = 0;
				break;
			case VK_DOWN:
				m_lOptionSelected--;
				if (m_lOptionSelected < 0)
					m_lOptionSelected = MAIN_MENU_SIZE -1;
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
		case WM_PAINT:
		{
			if (m_pDXManager->GetSwapChain())
			{
				ID3D11Texture2D* pBackBuffer = 0;
				D3D11_TEXTURE2D_DESC dtd;

				m_pDXManager->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
				pBackBuffer->GetDesc(&dtd);
				m_pDXManager->GetContext()->PSSetShaderResources(0, 1, &m_pSRVBackGround);

				m_FX->SetRenderTarget(m_pDXManager->GetMainRTV());
				m_FX->m_Params.Brightness = { 0,0,0,0 };
				m_FX->SetInput(m_pSRVBackGround);
				m_FX->Process(0, FX_NONE, dtd.Width, dtd.Height);

				for (unsigned long i = 0; i < MAIN_MENU_SIZE; i++)
				{
					m_FX->SetImgVertex(m_vMenu[i].frame, m_vMenu[i].indices);

					m_FX->SetRenderTarget(m_pDXManager->GetMainRTV());
					m_FX->SetInput(m_vMenu[i].pSRV);

					if (m_lOptionSelected != i)
						m_FX->m_Params.Brightness = { 0.5,0.5,0.5,0 };
					else
						m_FX->m_Params.Brightness = { 0,0,0,0 };
					m_FX->Process(0, FX_NONE, dtd.Width, dtd.Height, FX_FLAGS_USE_IMG_BUFFR);
				}
				
				m_pDXManager->GetSwapChain()->Present(1, 0);

				SAFE_RELEASE(pBackBuffer);
			}
			break;
		}
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
}
