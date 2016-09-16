#pragma once
#include "HSM\StateBase.h"
#include "Graphics\FX.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\DXManager.h"

#define CLSID_CSIntro 0x1a2b2001
class CSIntro :
	public CStateBase
{
public:

	// Atributos
	CDXManager* m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	CFX* m_FX;

	ID3D11Texture2D* m_pImgIntro;

	CSIntro();
	virtual ~CSIntro();
	unsigned long GetClassID() { return CLSID_CSIntro; }
	const char* GetClassString() { return "CSIntro"; }
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
};

