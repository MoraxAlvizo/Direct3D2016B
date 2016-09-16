#include "stdafx.h"
#include "SIntro.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"
#include "SOnGame.h"
#include "SMain.h"
#include "Graphics\ImageBMP.h"

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

	m_FX = new CFX(m_pDXManager);
	if (!m_FX->Initialize())
	{
		MessageBox(NULL,
			L"No se ha podido inicializar FX",
			L"Error fatal", MB_ICONERROR);
		return ;
	}

	CImageBMP* pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\intro.bmp", NULL);

	if (!pImage)
	{
		MessageBox(NULL, L"No se pudo cargar textura desde archivo",
			L"Verificar recursos sombreadores", MB_ICONERROR);
		return;
	}

	m_pImgIntro = pImage->CreateTexture(m_pDXManager);

	SetTimer(main->m_hWnd, 1, 5000, NULL);
	return;
}

unsigned long CSIntro::OnEvent(CEventBase * pEvent)
{
	if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_CHAR:
			if (pWin32->m_wParam == 'a')
			{
				CSMain* main = (CSMain*)GetSuperState();
				m_pSMOwner->Transition(CLSID_CSOnGame);
				InvalidateRect(main->m_hWnd, NULL, false);
				return 0;
			}
		case WM_TIMER:
			if (1 == pWin32->m_wParam)
			{
				//m_pSMOwner->Transition(CLSID_CSOnGame);
				//CSMain* main = (CSMain*)GetSuperState();
				//InvalidateRect(main->m_hWnd, NULL, false);
				return 0;
			}
			break;
		case WM_PAINT:

			if (m_pDXManager->GetSwapChain())
			{
				ID3D11Texture2D* pBackBuffer = 0;
				D3D11_TEXTURE2D_DESC dtd;
				ID3D11ShaderResourceView* pSRV = NULL;

				m_pDXManager->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
				pBackBuffer->GetDesc(&dtd);

				m_pDXManager->GetDevice()->CreateShaderResourceView(m_pImgIntro, NULL, &pSRV);
				m_pDXManager->GetContext()->PSSetShaderResources(0, 1, &pSRV);

				m_FX->SetRenderTarget(m_pDXManager->GetMainRTV());
				m_FX->SetInput(pSRV);
				m_FX->Process(0, 0, dtd.Width, dtd.Height);

				m_pDXManager->GetSwapChain()->Present(1, 0);

				SAFE_RELEASE(pSRV);
				SAFE_RELEASE(pBackBuffer);
			}

		default:
			break;
		}
	}
	return __super::OnEvent(pEvent);

}

void CSIntro::OnExit(void)
{
	CSMain* main = (CSMain*)GetSuperState();
	m_FX->Uninitialize();
	KillTimer(main->m_hWnd, 1);
	SAFE_RELEASE(m_pImgIntro);
	SAFE_DELETE(m_FX);
}
