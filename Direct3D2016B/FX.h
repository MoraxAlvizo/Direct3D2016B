#pragma once

#include <d3d11.h>
#include "Matrix4D.h"
#include "DXManager.h"
#include "MeshMathSurface.h"
#include "DXBasicPainter.h"
#include <vector>

using namespace std;

class CFX
{
	CDXManager* m_pOwner;
	ID3D11InputLayout* m_pIL;
	ID3D11Buffer* m_pCB;
	ID3D11VertexShader* m_pVS;
	vector<ID3D11PixelShader*> m_vecFX;
	ID3D11ShaderResourceView* m_pSRVInput0;
	ID3D11RenderTargetView * m_pRTVOutput;
	ID3D11RenderTargetView* m_pRTV;

	struct VERTEX
	{
		VECTOR4D Position;
		VECTOR4D TexCoord;
		VECTOR4D Color;

		static D3D11_INPUT_ELEMENT_DESC InputLayout[];
	};

	VERTEX vFrame[4];
	unsigned long   iFrame[6];
public:

	struct PARAMS
	{
		VECTOR4D Delta;
	}m_Params;
	CFX(CDXManager* pOwner);
	bool Initialize();
	void Process(unsigned long w, unsigned long h);
	void Uninitialize();
	void SetRenderTarget(ID3D11RenderTargetView* pRTV) { m_pRTV = pRTV; }
	~CFX();
};

