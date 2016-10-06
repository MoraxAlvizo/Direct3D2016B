#pragma once
#include "HSM\StateBase.h"
#include "Graphics\FX.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\DXManager.h"
#include "Sound\SndControl.h"
#define CLSID_CSIntro 0x1a2b2001

enum
{
	INTRO_SND_EXPLOSION,
	INTRO_SND_BACKGROUND,
	INTRO_SND_SIZE
};
class CSIntro :
	public CStateBase
{
public:

	// Atributos
	CDXManager* m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	CFX* m_FX;
	CSndControl* m_pSndBackground;

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

