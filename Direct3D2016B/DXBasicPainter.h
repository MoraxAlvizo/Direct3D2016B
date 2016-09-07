#pragma once
#include "DXManager.h"
#include "Matrix4D.h"

class CDXBasicPainter
{
protected:
	CDXManager* m_pManager;
	ID3D11InputLayout* m_pIL;
	ID3D11VertexShader* m_pVS;
	ID3D11PixelShader* m_pPS;
	ID3D11Buffer*      m_pCB;
	ID3D11RenderTargetView* m_pRTV;
	ID3D11RasterizerState* m_pDrawLH;
	ID3D11RasterizerState* m_pDrawRH;
	ID3D11DepthStencilState* m_pDSSMask;
	ID3D11DepthStencilState* m_pDSSDrawOnMask;
	ID3D11DepthStencilState* m_pDSSDrawOnNoMask;
	ID3D11DepthStencilState* m_pDSSDraw;
	
	//Soporte para Sombres
	ID3D11ShaderResourceView* m_pSRVShadowMap;
	ID3D11DepthStencilView*   m_pDSVShadowMap;  // Para producir buffer de profundidad
	ID3D11RenderTargetView*   m_pRTVShadowMap;   // Para producir mapa de sombras
	ID3D11VertexShader*		  m_pVSShadow;
	ID3D11PixelShader*		  m_pPSShadow;

#define PAINTER_DRAW_MARK			0x01
#define PAINTER_DRAW_ON_MARK		0x02
#define PAINTER_DRAW_ON_NOT_MARK	0x04
#define PAINTER_DRAW				0x08
#define PAINTER_WITH_LINESTRIP      0x10

public:
	void ClearShadow();
	void SetRenderTarget(ID3D11RenderTargetView* pRTV) { m_pRTV = pRTV; }
	ID3D11RasterizerState* GetDrawRHRState() { return m_pDrawRH; }
	ID3D11RasterizerState* GetDrawLHRState() { return m_pDrawLH; }
	struct MATERIAL
	{
		VECTOR4D Ambient;
		VECTOR4D Diffuse;
		VECTOR4D Specular;
		VECTOR4D Emissive;
		VECTOR4D Power;
	};
	struct LIGHT
	{
		struct
		{
			unsigned long Flags, Type, notused0, notused1;

			// flags
			#define LIGHT_ON 0x01
			
			// Types
			#define LIGHT_DIRECTIONAL 0 
			#define LIGHT_POINT 1
			#define LIGHT_SPOT 2
		};
		VECTOR4D Ambient;
		VECTOR4D Diffuse;
		VECTOR4D Specular;
		VECTOR4D Attenuation; // 1/(x+(y*d)+(z*(d^2)))
		VECTOR4D Position;
		VECTOR4D Direction;
		VECTOR4D Factors; // x: Power Spotlight

	};
	struct PARAMS
	{
		struct
		{
			unsigned int Flags1, notused0, notused1, notused2;
		};
		//Flags1
#define LIGHTING_AMBIENT			0x001
#define LIGHTING_DIFFUSE			0x002
#define LIGHTING_SPECULAR			0x004
#define LIGHTING_EMISSIVE			0x008
#define MAPPING_DIFFUSE				0x010
#define MAPPING_NORMAL				0x020
#define MAPPING_ENVIROMENTAL_FAST	0x040
#define MAPPING_NORMAL_TRUE			0x080
#define MAPPING_EMISSIVE			0x100
#define MAPPING_SHADOW				0x200
#define MAPPING_ENVIROMENTAL_SKY	0x400
#define FOG_ENABLE              	0x800
#define DRAW_JUST_WITH_COLOR		0x1000
#define DRAW_MIRROR					0x2000
#define DRAW_OCTREE					0x4000

#define SHADOW_MAP_RESOLUTION	1024 // resolucion de textura
		MATRIX4D World;
		MATRIX4D View;
		MATRIX4D Projection;
		MATRIX4D LightView;
		MATRIX4D LightProjection;
		VECTOR4D Brightness;
		VECTOR4D CameraPosition;
		MATERIAL Material;
		LIGHT lights[8];

	}m_Params;
	struct VERTEX
	{
		VECTOR4D Position;
		VECTOR4D Normal;
		VECTOR4D Tangent;
		VECTOR4D Binormal;
		VECTOR4D Color;
		VECTOR4D TexCoord;
		static D3D11_INPUT_ELEMENT_DESC InputLayout[];
	};
	CDXBasicPainter(CDXManager* pOwner);
	bool Initialize();
	void Uninitialize();

	void DrawIndexed(VERTEX* pVertices,
		unsigned long nVertices,
		unsigned long *pIndices,
		unsigned long nIndices, 
		unsigned long flags, 
		bool bShadow = false);

	
	~CDXBasicPainter();
};

