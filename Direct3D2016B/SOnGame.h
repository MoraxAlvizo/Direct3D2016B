/*

File name:
	CSOnGame.h

Descrition:
	This file contains the definition of CSOnGame state.
	This state is responsible to draw all the scene and manage
	the camera.

	DD/MM/AA	Name	- Coment
	15/09/16	OMAR	- Creation

*/

#pragma once
#include "HSM\StateBase.h"
#include "Graphics\DXManager.h"
#include "Graphics\DXBasicPainter.h"
#include "Graphics\MeshMathSurface.h"
#include "Collisions\MeshCollision.h"
#include "Collisions\OctreeCube.h"
#include "Cut/VMesh.h"
#include "Collisions\BVH.h"
#include "GameMap.h"
#include "SMainMenu.h"
#include "Sound\SndFx.h"

#include <vector>

using namespace std;

#define CLSID_CSOnGame 0x14638246

/* Timers */
#define MAP_TIMER_PLAYER1		1
#define MAP_TIMER_PLAYER2		2
#define MAP_TIMER_CLOCK			3
#define MAP_TIMER_GAME_OVER		4


enum {
	ON_GAME_STATE_GAMING = 1,
	ON_GAME_STATE_PAUSE ,
	ON_GAME_STATE_WIN ,
	ON_GAME_STATE_LOSE,
	ON_GAME_STATE_INTRO
};

#define GAME_OVER_STEPS 40.f

enum GameOverAnimation
{
	ON_GAME_GAMEOVER_ANIMATION_INIT = 1,
	ON_GAME_GAMEOVER_ANIMATION_ON,
	ON_GAME_GAMEOVER_ANIMATION_DONE
};

enum GameSounds
{
	ON_GAME_SOUNDS_BACKGROUND,
	ON_GAME_SOUNDS_NOT_MOVE,
	ON_GAME_SOUNDS_YOU_LOSE
};

class CSOnGame :
	public CStateBase
{
public:

	/* Members */
	CDXManager* m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	HWND m_hWnd;
	unsigned long m_lPainterFlags;

	/* Collisions */
	COctreeCube* m_pOctree;
	unsigned long m_lFlags;

	unsigned long m_lMoveSphere1;
	unsigned long m_lMoveSphere2;
	/* Flags move object */
#define MOVE_OBJECT 0x01
#define MOVE_UP     0x02
#define MOVE_DOWN	0x04

	/* Flags physics state */
#define PHYSICS_DRAW_OCTREE		0x01
#define PHYSICS_PRINT_OCTREE	0x02

	ID3D11Texture2D* m_pTexture;   //GPU
	ID3D11Texture2D* m_pNormalMapTrue;
	ID3D11Texture2D* m_pNormalMap;
	ID3D11Texture2D* m_pEmissiveMap;
	ID3D11Texture2D* m_pCubeMap;
	ID3D11Texture2D* m_pEnvMap;
	/* SRVs */
	ID3D11ShaderResourceView* m_pSRVTexture ;
	ID3D11ShaderResourceView* m_pSRVNormalMap ;
	ID3D11ShaderResourceView* m_pSRVEnvMap ;
	ID3D11ShaderResourceView* m_pSRVEmissiveMap ;
	ID3D11ShaderResourceView* m_pSRVNormalMapTrue;
	/* Matrix */
	MATRIX4D m_World;
	MATRIX4D m_View;
	MATRIX4D m_Projection;

	/* Scene */
	CMeshMathSurface m_Surface;
	vector<CMeshCollision> m_Scene;

	/* Map */
	CGameMap m_Map;

	CVMesh MiVariable;
	//vector<BVH> m_BVHs;

	bool flag = true;
	long m_lClock;
	long m_lState;

	/* Vector menu */
	vector<MenuOption> m_vMenu;
	long m_lOptionSelected;

	/* Animation var game over */
	GameOverAnimation statusAnimation;
	long steps;
	VECTOR4D incrementosTarget;
	VECTOR4D incrementosEyepos;

	VECTOR4D m_cameraTarget;
	VECTOR4D m_cameraEyePos;
	VECTOR4D m_cameraUp;

	/* Sounds */
	CSndFx* m_pSndBackground;

	enum
	{
		ON_GAME_MENU_CONTINUE = 0,
		ON_GAME_MENU_SALIR,
		ON_GAME_MENU_SIZE
	};

	struct Edges
	{
		union
		{
			long long id;
			struct
			{
				long id1;
				long id2;
			};

		};
	};


	/* Constructor and destructor */
	CSOnGame();
	virtual ~CSOnGame();

	/* Public methods */
	unsigned long GetClassID() { return CLSID_CSOnGame; }
	const char* GetClassString() { return "CSOnGame"; }

private:
	/* Camera options */
	bool m_bLeft , m_bRight ;
	bool m_bUp , m_bDown ;
	bool m_bForward, m_bBackward ;
	bool m_bTurnLeft , m_bTurnRight ;
	bool m_bTurnUp , m_bTurnDown ;
	bool m_bTurnS , m_bTurnS1 ;

	/* Draw options */
	unsigned long m_nFlagsPainter;

	/* Timers */
	bool m_bTimerPlayer1;
	bool m_bTimerPlayer2;

	/* create RTV1 */
	ID3D11Texture2D* m_pRT1;			// Memoria
	ID3D11ShaderResourceView* m_pSRV1;	// Input
	ID3D11RenderTargetView* m_pRTV1;		// Output

protected:

	/* Protected methods */
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);
	void Cut();
	void LoadScene(char * filename);
	void UpdateCamera();
	void ManageKeyboardEvents(UINT event, WPARAM wParam);
	

};

