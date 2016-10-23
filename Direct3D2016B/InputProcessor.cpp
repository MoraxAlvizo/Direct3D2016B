#include "stdafx.h"
#include "InputProcessor.h"
#include "ActionEvent.h"


CInputProcessor::CInputProcessor(CStateMachineManager *pHSMOwner)
{
	m_pHSMOwner = pHSMOwner;
}

unsigned long CInputProcessor::OnEvent(CEventBase * pEvent)
{
	if (INPUT_EVENT == pEvent->m_ulEventType)
	{
		// 1. Contruir copia del estado q interesa
		CInputEvent* pInput = (CInputEvent*)pEvent;
		CInputEvent* pCopy = new CInputEvent(
			pInput->m_nSource, 
			pInput->m_ulTime, 
			pInput->m_js2);
		auto GamePad = m_mapLastEvents.find(pInput->m_nSource);

		// 2. Procesar diferencias
		if (GamePad != m_mapLastEvents.end())
		{
			auto Last = GamePad->second->m_js2;
			auto Curr = pInput->m_js2;
			if (!Last.rgbButtons[0] && Curr.rgbButtons[0])
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_A_PRESSED);
				m_pHSMOwner->PostEvent(Action);
			}
			if (Last.rgbButtons[0] && !Curr.rgbButtons[0])
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_A_RELEASED);
				m_pHSMOwner->PostEvent(Action);
			}
			if (!Last.rgbButtons[1] && Curr.rgbButtons[1])
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_B_PRESSED);
				m_pHSMOwner->PostEvent(Action);
			}
			if (Last.rgbButtons[1] && !Curr.rgbButtons[1])
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_B_RELEASED);
				m_pHSMOwner->PostEvent(Action);
			}
			if (!Last.rgbButtons[2] && Curr.rgbButtons[2])
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_X_PRESSED);
				m_pHSMOwner->PostEvent(Action);
			}
			if (Last.rgbButtons[2] && !Curr.rgbButtons[2])
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_BUTTON_X_RELEASED);
				m_pHSMOwner->PostEvent(Action);
			}
			/* Reportar el estado de los ejes */
			{
				auto Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_AXIS_LX); 
				Action->m_fAxis = (Curr.lX - 0x7FFF)/32768.0f;
				m_pHSMOwner->PostEvent(Action);

				Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_AXIS_LY);
				Action->m_fAxis = -(Curr.lY - 0x7FFF) / 32768.0f;
				m_pHSMOwner->PostEvent(Action);

				Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_AXIS_RX);
				Action->m_fAxis = (Curr.lRx - 0x7FFF) / 32768.0f;
				m_pHSMOwner->PostEvent(Action);

				Action = new CActionEvent(GamePad->first, pInput->m_ulTime, JOY_AXIS_RY);
				Action->m_fAxis = -(Curr.lRy - 0x7FFF) / 32768.0f;
				m_pHSMOwner->PostEvent(Action);
			}

		}


		// 3. Almacenar el estado anterior
		if (GamePad == m_mapLastEvents.end())
		{
			m_mapLastEvents.insert(make_pair(pInput->m_nSource, pCopy));
		}
		else
		{
			delete GamePad->second;
			GamePad->second = pCopy;
		}
	}
	return 0;
}


CInputProcessor::~CInputProcessor()
{
}
