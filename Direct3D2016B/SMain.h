#pragma once

#include <ole2.h>
#include <xmllite.h>
#include <stdio.h>
#include <shlwapi.h>

#include "HSM/StateBase.h"
#include "Graphics\DXManager.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\FX.h"
#include "Sound\SndManager.h"
#include "Sound\SndControl.h"
#include "Sound\SndFx.h"
#include "Sound\SndFactory.h"
#include "Input\InputManager.h"

#define CLSID_CSMain 0xd5d6a900
#define MAIN ((CSMain*)m_pSMOwner->GetObjectByID(CLSID_CSMain))
#define BUF_SIZE 128

#define INPUT_EVENT 0x11221122
class CInputEvent :
	public CEventBase
{
public:
	long			m_nSource;
	DIJOYSTATE2		m_js2;
	unsigned long	m_ulTime;

	CInputEvent(long nSource, unsigned long ulTime, DIJOYSTATE2& js2)
	{
		m_ulEventType = INPUT_EVENT;
		m_nSource = nSource;
		m_ulTime = ulTime;
		m_js2 = js2;
	}
};

class CSMain :
	public CStateBase
{
public:
	/* Public memmber */
	struct InitParameters
	{
		wchar_t user[BUF_SIZE];
		wchar_t scene[BUF_SIZE];
		wchar_t gpu[BUF_SIZE];
		unsigned long FXEffect;
		unsigned long PainterFlags;

#define CUTTING_FEM		0x01
#define CUTTING_XFEM	0x01

		unsigned long MethodCutting;

#define COLLISION_OCTREE	0x01
#define COLLISION_BVH		0x02
		unsigned long MethodCollisionScene;
		unsigned long MethodCollisionPerObject;
		VECTOR4D PlaneCut[4];
	}m_Params;

	HINSTANCE m_hInstance;
	HWND m_hWnd;
	CDXManager* m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	CSndManager* m_pSndManager;
	CInputManager* m_pInputManager;
	CFX* m_FX;
	bool m_bInitCorrect;
	unsigned long GetClassID() { return CLSID_CSMain; }
	const char* GetClassString() { return "CSMain"; }
public:
	CSMain();
	virtual ~CSMain();
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
	
private:

	void ReadInitFile();
	HRESULT WriteAttributes(IXmlReader* pReader, long tag, wchar_t* localTag);
	void printParams();
};

