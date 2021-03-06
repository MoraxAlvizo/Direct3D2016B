#pragma once
#include <dxgi.h>
#include <d3d11.h>

#define SAFE_RELEASE(X) {if((X)){ (X)->Release();(X)=NULL;}} 

class CDXManager
{
	IDXGISwapChain* m_pSwapChain;
	ID3D11Device*   m_pDevice;  //Fabricas de recursos GPU/CPU
	ID3D11DeviceContext* m_pContext; //Comandos GPU/CPU
	ID3D11RenderTargetView* m_pRTV; // Main Render Target
	ID3D11Texture2D* m_pDepthStencil; // Buffer Z
	ID3D11DepthStencilView* m_pDSV; // Depth Stencil View
public:
	static IDXGIAdapter* EnumAndChooseAdapter(HWND hWnd, wchar_t* name);
	bool Initialize(HWND hWnd, IDXGIAdapter* pAdapter);
	void Resize(int cx, int cy);
	void Uninitialize(void);
	IDXGISwapChain* GetSwapChain(){ return m_pSwapChain; }
	ID3D11Device* GetDevice(){ return m_pDevice; }
	ID3D11DeviceContext* GetContext(){ return m_pContext; }
	ID3D11RenderTargetView *&GetMainRTV() { return m_pRTV; }
	ID3D11DepthStencilView*& GetMainDSV() { return m_pDSV; }

	ID3D10Blob* CompileShader(
		wchar_t* pszFileName, char* pszTarget,
		char* pszEntryPoint); //DXIL
	ID3D11VertexShader* CompileVertexShader(
		wchar_t* pszFileName, char*pszEntryPoint,
		ID3D10Blob** ppOutDXIL);
	ID3D11PixelShader* CompilePixelShader(
		wchar_t* pszFileName, char*pszEntryPoint);
	ID3D11ComputeShader* CompileComputeShader(
		wchar_t* pszFileName, char* pszEntryPoint);

	// CPU -> GPU
	ID3D11Buffer* CreateLoadBuffer(
		void *pData,
		unsigned long ulElementSize,
		unsigned long ulElements);
	// GPU -> CPU
	void CreateStoreBuffer(
		ID3D11Buffer* pISB,
		unsigned long ulElementSize,
		unsigned long ulElements,
		void* pOutData);

	ID3D11Buffer * CreateConstantBuffer(unsigned long ulBytes);
	void UpdateConstantBuffer(ID3D11Buffer * pCB, void * pSource, unsigned long ulBytes);

	CDXManager();
	~CDXManager();
};

