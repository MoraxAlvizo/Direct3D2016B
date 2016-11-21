#pragma once

#include "Graphics\DXBasicPainter.h"
#include "Graphics\DXManager.h"
#include "Graphics\FX.h"
class CDXTextRender
{
protected:
	CDXManager* m_pOwner;
	CDXBasicPainter* m_pPainter;
	CFX* m_pFX;
	ID3D11ShaderResourceView* m_pSRVFont;
	ID3D11BlendState* m_pBS;

public:
	CDXTextRender(CDXManager* pManager, CDXBasicPainter* pPainter, CFX* mFX);
	bool Initialize();
	void RenderText(MATRIX4D M, char* szText);
	void Uninitialize();
	~CDXTextRender();
};

