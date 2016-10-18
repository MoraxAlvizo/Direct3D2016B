#pragma once
#include "HSM\StateBase.h"
#include "Graphics\FX.h"
#include <d3d11.h>
#include <SpriteFont.h>

using namespace DirectX;

#define CLSID_CSMainMenu 0xa2c4a2c4

enum
{
	MAIN_MENU_START = 0,
	MAIN_MENU_EXIT ,
	MAIN_MENU_SIZE
};

class CSMainMenu :
	public CStateBase
{
public:
	unsigned long GetClassID() { return CLSID_CSMainMenu; }
	const char* GetClassString() { return "CSMainMenu"; }

	CSMainMenu();
	virtual ~CSMainMenu();
private:
	/* Members */
	CDXManager* m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	CFX* m_FX;
	long m_nOption;
	float m_fOffsetX;
	float m_fOffsetY;
	ID3D11ShaderResourceView* m_pSRVBackGround;
	ID3D11ShaderResourceView* m_pSRVStartGame;
	ID3D11ShaderResourceView* m_pSRVExitGame;

	struct MenuOption
	{
		ID3D11ShaderResourceView* pSRV;
		CFX::VERTEX frame[4];
		unsigned long indices[6];
	};

	/* Vector menu */
	vector<MenuOption> m_vMenu;
	long m_lOptionSelected;
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
};

