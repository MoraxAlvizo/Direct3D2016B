#include "stdafx.h"
#include "DXTextRender.h"
#include "Graphics\ImageBMP.h"

CDXTextRender::CDXTextRender(CDXManager * pManager, CDXBasicPainter * pPainter, CFX* mFX)
{
	m_pOwner = pManager;
	m_pPainter = pPainter;
	m_pFX = mFX;
	m_pSRVFont = nullptr;
}

CImageBMP::PIXEL AlphaGen(CImageBMP::PIXEL& p)
{
	CImageBMP::PIXEL r = p;
	r.a = p.r;
	return r;
}

bool CDXTextRender::Initialize()
{
	CImageBMP* img = CImageBMP::CreateBitmapFromFile("..\\Assets\\font.bmp", AlphaGen);

	auto tex = img->CreateTexture(m_pOwner);
	CImageBMP::DestroyBitmap(img);

	m_pOwner->GetDevice()->CreateShaderResourceView(tex, NULL, &m_pSRVFont);

	D3D11_BLEND_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.AlphaToCoverageEnable = false;
	dbd.IndependentBlendEnable = false;
	dbd.RenderTarget[0].BlendEnable = true;
	dbd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	// Color = ColorSrc*(as) + ColorDest*(1-as)
	dbd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	dbd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	dbd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	dbd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	dbd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

	dbd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
 	m_pOwner->GetDevice()->CreateBlendState(&dbd, &m_pBS);

	return true;
}

void CDXTextRender::RenderText(MATRIX4D M, char * szText)
{
	CFX::VERTEX Frame[4];

	Frame[0].Position = { -1, 1,0,1 };
	Frame[1].Position = { 1, 1,0,1 };
	Frame[2].Position = { -1, -1,0,1 };
	Frame[3].Position = {  1, -1,0,1 };

	Frame[0].TexCoord = { 0,0,0,0 };
	Frame[1].TexCoord = { 1,0,0,0 };
	Frame[2].TexCoord = { 0,1,0,0 };
	Frame[3].TexCoord = { 1,1,0,0 };

	Frame[0].Color = { 0,0,1,0 };
	Frame[1].Color = { 1,0,0,0 };
	Frame[2].Color = { 0,1,0,0 };
	Frame[3].Color = { 1,1,0,0 };

	unsigned long FrameIndex[6];
	FrameIndex[0] = 0;
	FrameIndex[1] = 1;
	FrameIndex[2] = 2;
	FrameIndex[3] = 2;
	FrameIndex[4] = 1;
	FrameIndex[5] = 3;

	m_pFX->m_Params.WVP = M;
	//m_pPainter->m_Params.Flags1 = MAPPING_EMISSIVE;
	m_pOwner->GetContext()->PSSetShaderResources(4,1,&m_pSRVFont);

	ID3D11Texture2D* pBackBuffer = 0;
	D3D11_TEXTURE2D_DESC dtd;


	m_pOwner->GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
	pBackBuffer->GetDesc(&dtd);
	m_pOwner->GetContext()->OMSetBlendState(m_pBS, NULL, -1);

	while (*szText)
	{
		// 1. Calcular las coordenas de texturas correspondientes al codigo ascii.
		int i, j;
		i = (unsigned char)*szText & 0x0f;
		j = ((unsigned char)*szText & 0xf0) >> 4;
		Frame[0].TexCoord.x = i / 16.f;
		Frame[0].TexCoord.y = j / 16.f;
		Frame[1].TexCoord.x = (i + 1) / 16.f;
		Frame[1].TexCoord.y = j / 16.f;
		Frame[2].TexCoord.x = i / 16.f;
		Frame[2].TexCoord.y = (j + 1) / 16.f;
		Frame[3].TexCoord.x = (i + 1) / 16.f;
		Frame[3].TexCoord.y = (j + 1) / 16.f;

		m_pFX->SetImgVertex(Frame, FrameIndex);
		m_pFX->SetRenderTarget(m_pOwner->GetMainRTV());
		m_pFX->SetInput(m_pSRVFont);

		m_pFX->Process(0, FX_NONE, dtd.Width, dtd.Height, FX_FLAGS_USE_IMG_BUFFR);
		
		for (int i = 0; i < 4; i++)
		{
			VECTOR4D Derecha = { 2,0,0,0 };
			Frame[i].Position = Frame[i].Position + Derecha;
		}

		szText++;
	}

	SAFE_RELEASE(pBackBuffer);
}

void CDXTextRender::Uninitialize()
{
	SAFE_RELEASE(m_pSRVFont);
}

CDXTextRender::~CDXTextRender()
{
	Uninitialize();
}
