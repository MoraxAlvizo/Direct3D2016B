#pragma once
#include "HSM\StateBase.h"
#include "SMain.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\DXManager.h"
#include "Graphics\FX.h"
#include <vector>

using namespace std;

#define CLSID_CSCredits 0xa2c4a201

enum
{
	CREDITS_ON_ANIMATION = 1,
	CREDITS_ANIMATION_FINISH
};


class CSCredits :
	public CStateBase
{
public:
	CSCredits();
	virtual ~CSCredits();

	unsigned long GetClassID() { return CLSID_CSCredits; }
	const char* GetClassString() { return "CSCredits"; }

	CDXBasicPainter* m_pDXPainter;
	CDXManager* m_pDXManager;
	CFX *m_FX;
	
	vector<string> m_vNombres;
	unsigned long m_lState;
	float m_fInitPos;

protected:

	/* Protected methods */
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
};

