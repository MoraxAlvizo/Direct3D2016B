#include "stdafx.h"
#include "FX.h"


float Plane(float x, float y)
{
	return 0.0f;
}

VECTOR4D PlaneNormalize(float x, float y, float z)
{
	VECTOR4D Normal = {
		0,
		0,
		1,
		0
	};
	return Normalize(Normal);
}

D3D11_INPUT_ELEMENT_DESC CFX::VERTEX::InputLayout[] =
{
	{ "POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "TEXCOORD",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,16,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,32,D3D11_INPUT_PER_VERTEX_DATA,0 }
};


CFX::CFX(CDXManager* pOwner)
{
	m_pOwner = pOwner;
	m_pCB = NULL;
	m_pRTVOutput = NULL;
	m_pVS = NULL;
	m_pIL = NULL;


	memset(&m_Params, 0, sizeof(PARAMS));

	m_vFrame[0].Position = { -1, 1,0,1 };
	m_vFrame[1].Position = {  1, 1,0,1 };
	m_vFrame[2].Position = {  1, -1,0,1 };
	m_vFrame[3].Position = { -1, -1,0,1 };

	m_vFrame[0].TexCoord = { 0,0,0,0 };
	m_vFrame[1].TexCoord = { 1,0,0,0 };
	m_vFrame[2].TexCoord = { 1,1,0,0 };
	m_vFrame[3].TexCoord = { 0,1,0,0 };

	m_vFrame[0].Color = { 0,0,1,0 };
	m_vFrame[1].Color = { 1,0,0,0 };
	m_vFrame[2].Color = { 0,1,0,0 };
	m_vFrame[3].Color = { 1,1,0,0 };

	m_lIndicesFrame[0] = 0;
	m_lIndicesFrame[1] = 1;
	m_lIndicesFrame[2] = 3;
	m_lIndicesFrame[3] = 3;
	m_lIndicesFrame[4] = 1;
	m_lIndicesFrame[5] = 2;
}


CFX::~CFX()
{
}

bool CFX::Initialize()
{
	Uninitialize();
	ID3D10Blob* pVSCode = NULL;
	m_pVS = m_pOwner->CompileVertexShader(
		L"..\\Shaders\\FX.hlsl", "VSMain", &pVSCode);
	if (!m_pVS)
	{
		SAFE_RELEASE(pVSCode);
		return false;
	}
	HRESULT hr =
		m_pOwner->GetDevice()->CreateInputLayout(
			VERTEX::InputLayout,
			sizeof(VERTEX::InputLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &m_pIL);
	SAFE_RELEASE(pVSCode);
	if (FAILED(hr))
	{
		SAFE_RELEASE(m_pVS);
		return false;
	}

	char* Effects[] = { "PSEdgeDetect" , "PSRadianBlur", "PSDirectionalBlur", "PSGaussHorizontalBlur", "PSGaussVerticalBlur" };
	for (auto FXName : Effects)
	{
		ID3D11PixelShader *pPS = m_pOwner->CompilePixelShader(
			L"..\\Shaders\\FX.hlsl", FXName);
		if (!pPS)
		{
			SAFE_RELEASE(m_pVS);
			SAFE_RELEASE(m_pIL);
			for (unsigned long i = 0; i < m_vecFX.size(); i++)
				SAFE_RELEASE(m_vecFX[i]);
			
			m_vecFX.clear();
			return false;
		}
		m_vecFX.push_back(pPS);
	}

	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.ByteWidth = 16 * ((sizeof(PARAMS) + 15) / 16);
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	m_pOwner->GetDevice()->CreateBuffer(&dbd, 0, &m_pCB);
	return true;
}
void CFX::Process(unsigned long idEffect, unsigned long w, unsigned long h)
{
	unsigned long nVertices = 4;
	unsigned long nIndices = 6;

	//1.- Crear los buffer de vértices e indices en el GPU.
	ID3D11Buffer  *pVB = NULL, *pIB = NULL;
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	dbd.ByteWidth = sizeof(VERTEX)*nVertices;
	dbd.CPUAccessFlags = 0;
	/*
	D3D11_USAGE_DEFAULT GPU:R/W CPU:None
	D3D11_USAGE_DYNAMIC GPU:R   CPU:W
	D3D11_USAGE_IMMUTABLE GPU:R CPU:W once
	D3D11_USAGE_STAGING   GPU:None CPU:W/R
	*/
	dbd.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA dsd;
	dsd.pSysMem = m_vFrame;
	dsd.SysMemPitch = 0;
	dsd.SysMemSlicePitch = 0;
	m_pOwner->GetDevice()->CreateBuffer(
		&dbd, &dsd, &pVB);
	dsd.pSysMem = m_lIndicesFrame;
	dbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dbd.ByteWidth = sizeof(unsigned long)*nIndices;
	m_pOwner->GetDevice()->CreateBuffer(
		&dbd, &dsd, &pIB);
	//2.- Instalar el VS , PS , IL
	m_pOwner->GetContext()->IASetInputLayout(m_pIL);
	m_pOwner->GetContext()->VSSetShader(m_pVS, 0, 0);
	m_pOwner->GetContext()->PSSetShader(m_vecFX[idEffect], 0, 0);
	//3.- Definir el puerto de visión y la topologia
	// a dibujar
	D3D11_VIEWPORT ViewPort;
	ID3D11Texture2D* pBackBuffer = NULL;
	D3D11_TEXTURE2D_DESC dtd;
	m_pOwner->GetSwapChain()->GetBuffer(0
		, IID_ID3D11Texture2D, (void**)&pBackBuffer);
	pBackBuffer->GetDesc(&dtd);
	ViewPort.Width = (float)dtd.Width;
	ViewPort.Height = (float)dtd.Height;
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;
	ViewPort.MaxDepth = 1.0f;
	ViewPort.MinDepth = 0.0f;
	m_pOwner->GetContext()->RSSetViewports(1, &ViewPort);
	m_pOwner->GetContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//4.- Configurar la salida. aqui lo omitimos por que ya fue seteado 
	m_pOwner->GetContext()->OMSetRenderTargets(1, &m_pRTVOutput, 0);

	//4.1 Setear el Render Target anterior ahora como textura
	m_pOwner->GetContext()->PSSetShaderResources(0, 1, &m_pSRVInput0);
	SAFE_RELEASE(pBackBuffer);

	//5.- Dibujar
	unsigned int Offset = 0;
	unsigned int Stride = sizeof(VERTEX);
	m_pOwner->GetContext()->IASetVertexBuffers(0, 1, &pVB, &Stride, &Offset);
	m_pOwner->GetContext()->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
	D3D11_MAPPED_SUBRESOURCE ms;


	m_pOwner->GetContext()->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	PARAMS Temp = m_Params;

	Temp.Delta = { 1 / (float)w,1 / (float)h,0,0 };
	Temp.RadialBlur.x = .01;
	//Temp.DirectionalBlur = { 1,0,.01f,0 };
	

	memcpy(ms.pData, &Temp, sizeof(PARAMS));
	m_pOwner->GetContext()->Unmap(m_pCB, 0);
	m_pOwner->GetContext()->VSSetConstantBuffers(0, 1, &m_pCB);
	m_pOwner->GetContext()->PSSetConstantBuffers(0, 1, &m_pCB);
	m_pOwner->GetContext()->DrawIndexed(nIndices, 0, 0);
	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pVB);

}
void CFX::Uninitialize()
{
	SAFE_RELEASE(m_pIL);
	SAFE_RELEASE(m_pVS);
	for (unsigned long i = 0; i < m_vecFX.size(); i++)
		SAFE_RELEASE(m_vecFX[i]);
	m_vecFX.clear();
	SAFE_RELEASE(m_pCB);
}