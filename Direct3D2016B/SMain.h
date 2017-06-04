#pragma once

#include <ole2.h>
#include <xmllite.h>
#include <stdio.h>
#include <shlwapi.h>

#include "HSM/StateBase.h"
#include "Graphics\DXManager.h"
#include "Graphics\DXPainter.h"
#include "Graphics\FX.h"


#define CLSID_CSMain 0xd5d6a900
#define MAIN ((CSMain*)m_pSMOwner->GetObjectByID(CLSID_CSMain))
#define BUF_SIZE 128
#define MAIN_ASSETS_DIR "..\\Assets\\"


class CSMain :
	public CStateBase
{
	struct objectData {
		char name[BUF_SIZE];
		VECTOR4D position;
	};
public:
	/* Public memmber */
	struct InitParameters
	{
		vector<objectData> scene;
		wchar_t gpu[BUF_SIZE];
		unsigned long FXEffect;
		unsigned long PainterFlags;

#define COLLISION_OCTREE	0x01
#define COLLISION_BVH		0x02
		unsigned long MethodCollisionScene;
		unsigned long MethodCollisionPerObject;
	}m_Params;

	HINSTANCE m_hInstance;
	HWND m_hWnd;

	/* Engines */
	CDXManager* m_pDXManager;
	CDXPainter* m_pDXPainter;


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

