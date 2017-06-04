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
#include "Graphics\DXPainter.h"
#include "Graphics\MeshMathSurface.h"
#include "Collisions\OctreeCube.h"
#include "Cut/VMesh.h"
#include "Collisions\BVH.h"
#include <time.h>

/* Get time API */
//#define TIMER

#define TIMER_NUM_ITERATION (100)
#define TIMER_PRINT_TIME(_function, _msg) \
{\
	double _secs = 0.0;\
	LARGE_INTEGER _t_ini, _t_fin, _freq;\
\
	for(unsigned int _i=0; _i < TIMER_NUM_ITERATION; _i++)\
	{\
		QueryPerformanceCounter(&_t_ini); \
		_function;\
		QueryPerformanceCounter(&_t_fin);\
		QueryPerformanceFrequency(&_freq);\
		_secs +=  (double)(_t_fin.QuadPart - _t_ini.QuadPart) / (double)_freq.QuadPart;\
	}\
 \
	printf("TIMER: %s: time[%.16g ms]\n", _msg, _secs * 1000 / TIMER_NUM_ITERATION);\
}

#define CLSID_CSOnGame 0x14638246

class CSOnGame :
	public CStateBase
{
public:

	/* Members */
	CDXManager* m_pDXManager;
	CDXPainter* m_pDXPainter;
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
#define PHYSICS_PRINT_BVH		0x04

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
	//vector<CMesh> m_SceneCollisions;
	vector<CVMesh> m_ScenePhysics;

	bool flag = true;

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

	/* FPS */
	double m_dStarttime;
	int m_iFrames ;
	float m_fFps;
	bool m_bChangeFPS;

	/* Simulation options */
	bool m_pause;

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

