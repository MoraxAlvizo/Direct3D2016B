#pragma once
#include "HSM\EventListener.h"
#include "HSM\StateMachineManager.h"
#include "Input\InputManager.h"
#include "SMain.h"

#include <map>

using namespace std;

class CInputProcessor :
	public CEventListener
{
public:
	/* Members */
	map<int, CInputEvent*> m_mapLastEvents;
	CStateMachineManager* m_pHSMOwner;

	/* Methods*/
	CInputProcessor(CStateMachineManager*);
	virtual unsigned long OnEvent(CEventBase* pEvent);
	virtual ~CInputProcessor();
};

