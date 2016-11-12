#include "stdafx.h"
#include "FX.h"




D3D11_INPUT_ELEMENT_DESC CFX::VERTEX::InputLayout[] =
{
	{ "POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "TEXCOORD",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,16,D3D11_INPUT_PER_VERTEX_DATA,0 },
	{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,32,D3D11_INPUT_PER_VERTEX_DATA,0 }
};

#define M_PI 3.14159265358979323846
VECTOR4D Sphere(float u, float v)
{
	float r = 1.f;
	VECTOR4D v1;
	v1.x = r*cos(2 * M_PI *u)*sin(M_PI *v);
	v1.y = r*sin(2 * M_PI*u)*sin(M_PI*v);
	v1.z = r*cos(M_PI*v);
	v1.w = 1;
	return v1;
}

CFX::CFX(CDXManager* pOwner)
{
	m_pOwner = pOwner;
	m_pCB = NULL;
	m_pRTVOutput = NULL;
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
#define SURFACE_RESOLUTION 5
	m_Sphere.BuildParametricSurface(SURFACE_RESOLUTION, SURFACE_RESOLUTION, 0, 0, 1.0f / (SURFACE_RESOLUTION - 1), 1.0f / (SURFACE_RESOLUTION - 1), Sphere);

	m_vSphere.resize(m_Sphere.m_Vertices.size());

	for (unsigned long i = 0; i < m_Sphere.m_Vertices.size(); i++)
		m_vSphere[i].Position = m_Sphere.m_Vertices[i].Position;

	m_lIndicesSphere.resize(m_Sphere.m_Indices.size());

	for (unsigned long i = 0; i < m_Sphere.m_Indices.size(); i++)
		m_lIndicesSphere[i] = m_Sphere.m_Indices[i];
}


CFX::~CFX()
{
}

bool CFX::Initialize()
{
	Uninitialize();
	ID3D10Blob* pVSCode = NULL;
	ID3D11VertexShader *pVS = m_pOwner->CompileVertexShader(
		L"..\\Shaders\\FX.hlsl", "VSMain", &pVSCode);
	if (!pVS)
	{
		SAFE_RELEASE(pVSCode);
		return false;
	}

	m_vecVS.push_back(pVS);

	HRESULT hr =
		m_pOwner->GetDevice()->CreateInputLayout(
			VERTEX::InputLayout,
			sizeof(VERTEX::InputLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &m_pIL);
	SAFE_RELEASE(pVSCode);

	if (FAILED(hr))
	{
		SAFE_RELEASE(pVS);
		return false;
	}

	pVS = m_pOwner->CompileVertexShader(
		L"..\\Shaders\\FX.hlsl", "VSSky", &pVSCode);
	if (!pVS)
	{
		SAFE_RELEASE(pVSCode);
		return false;
	}
	SAFE_RELEASE(pVSCode);

	m_vecVS.push_back(pVS);

	char* Effects[] = { "PSEdgeDetect" ,			//0
						"PSRadianBlur",				//1
						"PSDirectionalBlur",		//2
						"PSGaussHorizontalBlur",	//3
						"PSGaussVerticalBlur",		//4
						"PSBrightPass",				//5
						"PSMerged",					//6
						"PSSky",					//7
						"PSNone"
						};
	for (auto FXName : Effects)
	{
		ID3D11PixelShader *pPS = m_pOwner->CompilePixelShader(
			L"..\\Shaders\\FX.hlsl", FXName);
		if (!pPS)
		{
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

	D3D11_DEPTH_STENCIL_DESC ddsd;

	ddsd.DepthEnable = true;
	ddsd.StencilEnable = true;
	ddsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ddsd.DepthFunc = D3D11_COMPARISON_LESS;
	ddsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	ddsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	ddsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	ddsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ddsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	ddsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	ddsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ddsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	ddsd.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	ddsd.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;

	m_pOwner->GetDevice()->CreateDepthStencilState(&ddsd, &m_pDSSDrawOnNoMask);
	return true;
}
void CFX::Process(unsigned long vsEffect, unsigned long psEffect, float w, float h, unsigned long flags)
{
	unsigned long nVertices = psEffect == 7 ? m_vSphere.size() :4;
	unsigned long nIndices = psEffect == 7 ? m_lIndicesSphere.size() :6;

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
	dsd.pSysMem = psEffect == 7 ? &m_vSphere[0] : flags & FX_FLAGS_USE_IMG_BUFFR ? m_vImg : m_vFrame;
	dsd.SysMemPitch = 0;
	dsd.SysMemSlicePitch = 0;
	m_pOwner->GetDevice()->CreateBuffer(
		&dbd, &dsd, &pVB);

	dsd.pSysMem = psEffect == 7 ? &m_lIndicesSphere[0] : flags & FX_FLAGS_USE_IMG_BUFFR ? m_lIndicesImg : m_lIndicesFrame;
	dbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dbd.ByteWidth = sizeof(unsigned long)*nIndices;
	m_pOwner->GetDevice()->CreateBuffer(
		&dbd, &dsd, &pIB);
	//2.- Instalar el VS , PS , IL
	m_pOwner->GetContext()->IASetInputLayout(m_pIL);
	m_pOwner->GetContext()->VSSetShader(m_vecVS[vsEffect], 0, 0);
	m_pOwner->GetContext()->PSSetShader(m_vecFX[psEffect], 0, 0);
	//3.- Definir el puerto de visión y la topologia
	// a dibujar

	D3D11_VIEWPORT ViewPort;

	ViewPort.Width = (float)w;
	ViewPort.Height = (float)h;
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;
	ViewPort.MaxDepth = 1.0f;
	ViewPort.MinDepth = 0.0f;
	m_pOwner->GetContext()->RSSetViewports(1, &ViewPort);
	m_pOwner->GetContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//4.- Configurar la salida. aqui lo omitimos por que ya fue seteado
	if(psEffect == 7)
		m_pOwner->GetContext()->OMSetDepthStencilState(m_pDSSDrawOnNoMask, 0x01);
	m_pOwner->GetContext()->OMSetRenderTargets(1, &m_pRTVOutput, psEffect == 7 ? m_pOwner->GetMainDSV():0);

	//4.1 Setear el Render Target anterior ahora como textura
	m_pOwner->GetContext()->PSSetShaderResources(0, 1, &m_pSRVInput0);
	if(m_pSRVBrightPassed != NULL)
		m_pOwner->GetContext()->PSSetShaderResources(1, 3, m_pSRVBrightPassed);

	//5.- Dibujar
	unsigned int Offset = 0;
	unsigned int Stride = sizeof(VERTEX);
	m_pOwner->GetContext()->IASetVertexBuffers(0, 1, &pVB, &Stride, &Offset);
	m_pOwner->GetContext()->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
	D3D11_MAPPED_SUBRESOURCE ms;


	m_pOwner->GetContext()->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	PARAMS Temp = m_Params;

	Temp.WVP = Transpose(m_Params.WVP);
	Temp.Delta = { 1 / (float)w,1 / (float)h,0,0 };

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
	for (unsigned long i = 0; i < m_vecVS.size(); i++)
		SAFE_RELEASE(m_vecVS[i]);
	m_vecVS.clear();
	for (unsigned long i = 0; i < m_vecFX.size(); i++)
		SAFE_RELEASE(m_vecFX[i]);
	m_vecFX.clear();
	SAFE_RELEASE(m_pCB);
}