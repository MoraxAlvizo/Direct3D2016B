#include "stdafx.h"
#include "SIntro.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
#include "SOnGame.h"
#include "SMain.h"
#include "Graphics\ImageBMP.h"
#include "SMainMenu.h"

CSIntro::CSIntro()
{
}


CSIntro::~CSIntro()
{
}

void CSIntro::OnEntry(void)
{
	CSMain* main = (CSMain*)GetSuperState();
	/* Cargar apuntadores */
	m_pDXManager = main->m_pDXManager;
	m_pDXPainter = main->m_pDXPainter;

	printf("[HCM] %s:OnEntry\n", GetClassString());
	m_FX = main->m_FX;
	/*m_FX = new CFX(m_pDXManager);
	if (!m_FX->Initialize())
	{
		MessageBox(NULL,
			L"No se ha podido inicializar FX",
			L"Error fatal", MB_ICONERROR);
		return;
	}*/

	CImageBMP* pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\intro.bmp", NULL);

	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	m_pImgIntro = pImage->CreateTexture(m_pDXManager);

	main->m_pSndManager->ClearEngine();
	auto fx = main->m_pSndManager->LoadSoundFx(L"..\\Assets\\Sonidos\\intro.wav", INTRO_SND_EXPLOSION);

	if (fx)
		printf("Explosion load success \n");
	else
		printf("Explosion load fail\n");

	m_pSndBackground = main->m_pSndManager->LoadSoundFx(L"..\\Assets\\Sonidos\\intro.wav", INTRO_SND_BACKGROUND);
	if (m_pSndBackground)
		m_pSndBackground->Play(false);
	SetTimer(main->m_hWnd, 1, 3000, NULL);
	/*SetTimer(main->m_hWnd, 2, 1000, NULL);*/
	return;
}

unsigned long CSIntro::OnEvent(CEventBase * pEvent)
{
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		if (m_pDXManager->GetSwapChain())
		{
			ID3D11Texture2D* pBackBuffer = 0;
			D3D11_TEXTURE2D_DESC dtd;
			ID3D11ShaderResourceView* pSRV = NULL;

			m_pDXManager->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
			pBackBuffer->GetDesc(&dtd);

			m_pDXManager->GetDevice()->CreateShaderResourceView(m_pImgIntro, NULL, &pSRV);
			m_pDXManager->GetContext()->PSSetShaderResources(0, 1, &pSRV);

			m_FX->m_Params.WVP = Identity();

			m_FX->SetRenderTarget(m_pDXManager->GetMainRTV());
			m_FX->SetInput(pSRV);
			m_FX->m_Params.Brightness = { 0,0,0,0 };
			m_FX->Process(0, FX_NONE, dtd.Width, dtd.Height);

			m_pDXManager->GetSwapChain()->Present(1, 0);

			SAFE_RELEASE(pSRV);
			SAFE_RELEASE(pBackBuffer);
		}
	}
	else if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			if (pWin32->m_wParam == 'a')
			{
				/*CSMain* main = (CSMain*)GetSuperState();
				m_pSMOwner->Transition(CLSID_CSMainMenu);
				InvalidateRect(main->m_hWnd, NULL, false);*/
				return 0;
			}
			/*{
				static float speed = 1.0f;
				if (pWin32->m_wParam == 'm')
				{
					speed = min(4.0, speed + 0.10);
					m_pSndBackground->SetSpeed(speed);
					return 0;
				}
				if (pWin32->m_wParam == 'n')
				{
					speed = max(0.1, speed - 0.10);
					m_pSndBackground->SetSpeed(speed);
					return 0;
				}
			}*/
			
		case WM_TIMER:
			if (1 == pWin32->m_wParam)
			{
				m_pSMOwner->Transition(CLSID_CSMainMenu);
				CSMain* main = (CSMain*)GetSuperState();
				//InvalidateRect(main->m_hWnd, NULL, false);
				return 0;
			}
			/*if (2 == pWin32->m_wParam)
			{
				CSMain* main = (CSMain*)GetSuperState();
				KillTimer(main->m_hWnd, 2);
				main->m_pSndManager->PlayFx(INTRO_SND_EXPLOSION);
			}*/
			break;
		default:
			break;
		}
	}
	return __super::OnEvent(pEvent);

}

void CSIntro::OnExit(void)
{
	printf("[HCM] %s:OnExit\n", GetClassString());
	CSMain* main = (CSMain*)GetSuperState();
	KillTimer(main->m_hWnd, 1);
	/*KillTimer(main->m_hWnd, 2);*/
	SAFE_RELEASE(m_pImgIntro);
}
