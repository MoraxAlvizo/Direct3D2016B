#include "stdafx.h"
#include "DXBasicPainter.h"


CDXBasicPainter::CDXBasicPainter(CDXManager* pOwner)
{
	m_pManager = pOwner;
	m_pIL = NULL;
	m_pVS = NULL;
	m_pPS = NULL;
	m_pCB = NULL;
	m_Params.World = m_Params.View = m_Params.Projection = Identity();
	VECTOR4D Zero = { 0, 0, 0, 0 };
	m_Params.Brightness = Zero;
	
	MATERIAL MatDef = { 
		{1,1,1,1},  // Ambient
		{1,1,1,1},	// Diffuse
		{1,1,1,1},	// Specular
		{0,0,0,0},	// Emissive
		{100,0,0,0}  // Power
	};
	m_Params.Material = MatDef;
	memset(m_Params.lights, 0, sizeof(m_Params.lights));

	LIGHT LightDef = { 
		{LIGHT_ON, LIGHT_DIRECTIONAL,0,0 },
		{0.1,0.1,0.1,0}, 
		{0.5,0.5,0.5,0.5},
		{1,1,1,0},
		{1,0,0,0},
		{0,0,0,1},
		{0,0,-1,0},
		{1,0,0,0}
	};


	LIGHT LightDef2 = {
		{ LIGHT_ON, LIGHT_SPOT,0,0 },  // Flags
		{ 0.1,0.1,0.1,0 },				// Ambient 
		{ 5,5,5,5 },			// Diffuse
		{ 5,5,5,0 },					// Specular
		{ 1,.05,0.05,0 },					// Attenuation
		{ 0,0,1,1 },					// Position
		{ 0,0,0,0 },					// Direction
		{ 30,0,0,0 }						// Factors 
	};

	LIGHT LightDef3 = {
		{ LIGHT_ON, LIGHT_SPOT,0,0 },	// Flags
		{ 0.1,0.1,0.1,0 },				// Ambient 
		{ 1,1,1,1 },					// Diffuse
		{ 1,1,0.7,0 },					// Specular
		{ 1,0,0,0 },					// Attenuation
		{ 0,0,4,1 },					// Position
		{ 0,0,-1,0 },					// Direction
		{ 30,0,0,0 }						// Factors 
	};

	m_Params.lights[0] = LightDef;
	m_Params.lights[1] = LightDef2;
	//m_Params.lights[2] = LightDef3;
}

void CDXBasicPainter::Uninitialize()
{
	SAFE_RELEASE(m_pIL);
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pCB);
}

D3D11_INPUT_ELEMENT_DESC CDXBasicPainter::VERTEX::InputLayout[] =
{ 
	{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }, 
	{"NORMAL",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,16,D3D11_INPUT_PER_VERTEX_DATA,0}, 
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

bool CDXBasicPainter::Initialize()
{
	Uninitialize();
	ID3D10Blob* pVSCode = NULL;
	m_pVS=m_pManager->CompileVertexShader(
		L"..\\Shaders\\BasicShader.hlsl", "VSMain",&pVSCode);
	if (!m_pVS)
	{
		SAFE_RELEASE(pVSCode);
		return false;
	}
	HRESULT hr=
	m_pManager->GetDevice()->CreateInputLayout(
		VERTEX::InputLayout,
		sizeof(VERTEX::InputLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &m_pIL);
	SAFE_RELEASE(pVSCode);
	if (FAILED(hr))
	{
		SAFE_RELEASE(m_pVS);
		return false;
	}
	m_pPS = m_pManager->CompilePixelShader(
		L"..\\Shaders\\BasicShader.hlsl", "PSMain");
	if (!m_pPS)
	{
		SAFE_RELEASE(m_pVS);
		SAFE_RELEASE(m_pIL);
		return false;
	}
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.ByteWidth = 16 * ((sizeof(PARAMS) + 15) / 16);
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	m_pManager->GetDevice()->CreateBuffer(&dbd, 0, &m_pCB);
	return true;
}
CDXBasicPainter::~CDXBasicPainter()
{
	Uninitialize();
}

void CDXBasicPainter::DrawIndexed(VERTEX* pVertices, unsigned long nVertices,
	unsigned long* pIndices, unsigned long nIndices)
{
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
	dsd.pSysMem = pVertices;
	dsd.SysMemPitch = 0;
	dsd.SysMemSlicePitch = 0;
	m_pManager->GetDevice()->CreateBuffer(
		&dbd, &dsd, &pVB);
	dsd.pSysMem = pIndices;
	dbd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	dbd.ByteWidth = sizeof(unsigned long)*nIndices;
	m_pManager->GetDevice()->CreateBuffer(
		&dbd, &dsd, &pIB);
	//2.- Instalar el VS , PS , IL
	m_pManager->GetContext()->IASetInputLayout(m_pIL);
	m_pManager->GetContext()->VSSetShader(m_pVS, 0, 0);
	m_pManager->GetContext()->PSSetShader(m_pPS, 0, 0);
	//3.- Definir el puerto de visión y la topologia
	// a dibujar
	D3D11_VIEWPORT ViewPort;
	ID3D11Texture2D* pBackBuffer=NULL;
	D3D11_TEXTURE2D_DESC dtd;
	m_pManager->GetSwapChain()->GetBuffer(0
		, IID_ID3D11Texture2D, (void**)&pBackBuffer);
	pBackBuffer->GetDesc(&dtd);
	ViewPort.Width = (float)dtd.Width;
	ViewPort.Height = (float)dtd.Height;
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;
	ViewPort.MaxDepth = 1.0f;
	ViewPort.MinDepth = 0.0f;
	m_pManager->GetContext()->RSSetViewports(1, &ViewPort);
	m_pManager->GetContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//4.- Configurar la salida

	m_pManager->GetContext()->OMSetRenderTargets(1,
		&m_pManager->GetMainRTV(), m_pManager->GetMainDSV());
	SAFE_RELEASE(pBackBuffer);

	//5.- Dibujar
	unsigned int Offset = 0;
	unsigned int Stride = sizeof(VERTEX);
	m_pManager->GetContext()->IASetVertexBuffers(0, 1, &pVB, &Stride, &Offset);
	m_pManager->GetContext()->IASetIndexBuffer(pIB, DXGI_FORMAT_R32_UINT, 0);
	D3D11_MAPPED_SUBRESOURCE ms;
	m_pManager->GetContext()->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD,0, &ms);
	PARAMS Temp = m_Params;

	Temp.World = Transpose(m_Params.World);
	Temp.View = Transpose(m_Params.View);
	Temp.Projection = Transpose(m_Params.Projection);

	for (int i = 0; i < 8; i++)
	{
		Temp.lights[i].Position = m_Params.lights[i].Position*m_Params.View;
		Temp.lights[i].Direction = m_Params.lights[i].Direction*m_Params.View;

	}

	memcpy(ms.pData, &Temp, sizeof(PARAMS));
	m_pManager->GetContext()->Unmap(m_pCB, 0);
	m_pManager->GetContext()->VSSetConstantBuffers(0, 1, &m_pCB);
	m_pManager->GetContext()->PSSetConstantBuffers(0, 1, &m_pCB);
	m_pManager->GetContext()->DrawIndexed(nIndices, 0, 0);
	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pVB);
}
