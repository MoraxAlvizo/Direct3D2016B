#pragma once
#include "HSM\EventBase.h"

#define ACTION_EVENT	0xa5cc45a
enum JOY_EVENTS
{
 JOY_BUTTON_A_PRESSED= 1,
 JOY_BUTTON_A_RELEASED,

 JOY_BUTTON_B_PRESSED,
 JOY_BUTTON_B_RELEASED,	

 JOY_BUTTON_X_PRESSED,
 JOY_BUTTON_X_RELEASED,

 JOY_BUTTON_Y_PRESSED,
 JOY_BUTTON_Y_RELEASED,

 JOY_AXIS_LX,
 JOY_AXIS_LY,

 JOY_AXIS_RX,
 JOY_AXIS_RY,

 JOY_BUTTON_START_PRESSED,
 JOY_BUTTON_START_UNPRESSED

};
	



class CActionEvent :
	public CEventBase
{
public:
	/* Members */
	int m_nSource;
	unsigned long m_ulTime;
	int m_iAction;
	float m_fAxis; /* Valor normalizado de -1 a 1, punto central 0 */

	/* Methods */
	CActionEvent(int nSource, unsigned long ulTime, int iAction);
	virtual ~CActionEvent();
};

