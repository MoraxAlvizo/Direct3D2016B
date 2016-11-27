#pragma once
#include "HSM\StateBase.h"
#include "Graphics\FX.h"
#include <d3d11.h>
#include <SpriteFont.h>
#include "DXTextRender.h"

using namespace DirectX;

#define CLSID_CSMainMenu 0xa2c4a2c4

enum
{
	BUTTON_OVER,
	BUTTON_DOWN,
	BUTTON_UP,
	BUTTON_STATE_SIZE
};

enum MenuSounds
{
	ON_GAME_SOUNDS_MOVE_OPTION_MENU
};

struct MenuOption
{
	ID3D11ShaderResourceView* pSRV[BUTTON_STATE_SIZE];
	CFX::VERTEX frame[4];
	unsigned long indices[6];
	int stateButton;
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

	CDXTextRender* m_pTextRender;

	SpriteBatch *spriteBatch;
	SpriteFont  *spriteFont;

	/* Vector menu */
	vector<MenuOption> m_vMenu;
	long m_lOptionSelected;

	enum
	{
		MAIN_MENU_START = 0,
		MAIN_MENU_UNIRSE,
		MAIN_MENU_EXIT,
		MAIN_MENU_SIZE
	};
protected:
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
};

