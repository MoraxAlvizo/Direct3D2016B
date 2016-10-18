#include "stdafx.h"
#include "ActionEvent.h"


CActionEvent::~CActionEvent()
{
}


CActionEvent::CActionEvent(int nSource, unsigned long ulTime, int iAction)
{
	m_ulEventType = ACTION_EVENT;
	m_nSource = nSource;
	m_ulTime = ulTime;
	m_iAction = iAction;
}

