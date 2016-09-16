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

#define CLSID_CSOnGame 0x14638246

class CSOnGame :
	public CStateBase
{
public:

	/* Members */
	CDXManager* m_pDXManager;
	CDXBasicPainter* m_pDXPainter;
	HWND m_hWnd;
	unsigned long m_lPainterFlags;

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

protected:

	/* Protected methods */
	void OnEntry(void);
	unsigned long OnEvent(CEventBase* pEvent);
	void OnExit(void);

	void LoadScene(char * filename);
	void UpdateCamera();
	void ManageKeyboardEvents(UINT event, WPARAM wParam);

};

