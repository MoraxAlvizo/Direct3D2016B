#include "stdafx.h"
#include "SCredits.h"

#include "SIntro.h"

CSCredits::CSCredits()
{
}


CSCredits::~CSCredits()
{
}

void CSCredits::OnEntry(void)
{
	CSMain* main = (CSMain*)GetSuperState();
	/* Load pointers */
	m_pDXManager = main->m_pDXManager;
	m_pDXPainter = main->m_pDXPainter;
	m_FX = main->m_FX;
	printf("[HCM] %s:OnEntry\n", GetClassString());

	m_vNombres.push_back("CREDITOS");
	m_vNombres.push_back("PROGRAMARDOR: OMAR ALVIZO ");
	m_vNombres.push_back("DISENIADOR  : OMAR ALVIZO ");
	m_vNombres.push_back("CEO         : OMAR ALVIZO ");

	m_lState = CREDITS_ON_ANIMATION;
	m_fInitPos = -1;

}

unsigned long CSCredits::OnEvent(CEventBase * pEvent)
{
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		if (m_pDXManager->GetSwapChain())
		{
			
			/* clear render target */
			VECTOR4D Black = { 0, 0, 0, 0 };
			m_pDXManager->GetContext()->ClearRenderTargetView(m_pDXManager->GetMainRTV(), (float*)&Black);

			float translationY = m_fInitPos;

			for (unsigned int i = 0; i < m_vNombres.size(); i++)
			{
				MATRIX4D ST =
					Translation(0.5, -0.5, 0)*
					Scaling(0.03, 0.03, 1)*
					Translation(0, translationY, 0);

				translationY -= 0.2;
				MAIN->m_pTextRender->RenderText(ST,(char *) m_vNombres[i].c_str());
			}	

			if (translationY > 1.f)
			{
				m_pSMOwner->Transition(CLSID_CSIntro);
				return 0;
			}

			m_fInitPos += 0.005;

			m_pDXManager->GetSwapChain()->Present(1, 0);

		}
	}
	return 0;
}

void CSCredits::OnExit(void)
{
	m_vNombres.clear();
}
