// Direct3D2016B.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Direct3D2016B.h"
#include "DXManager.h"
#include "DXBasicPainter.h"
#include "ImageBMP.h"
#include "Mesh.h"
#include "MeshMathSurface.h"
#include "FX.h"
#include <Windows.h>
#include <Windowsx.h>
#include <timeapi.h>
#include <sstream>
#define MAX_LOADSTRING 100

// Global Variables:
HWND g_hWnd;
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
CDXManager g_Manager;
CDXBasicPainter g_Painter(&g_Manager);
CFX g_FX(&g_Manager);
CImageBMP*      g_pSysTexture; //CPU
ID3D11Texture2D* g_pTexture;   //GPU
ID3D11Texture2D* g_pNormalMapTrue; 
ID3D11Texture2D* g_pEmissiveMap;

// Render Targets
ID3D11Texture2D* g_pRT0;			// Memoria
ID3D11ShaderResourceView* g_pSRV0;	// Input
ID3D11RenderTargetView* g_pRTV0;		// Output


MATRIX4D g_World;
MATRIX4D g_View;
MATRIX4D g_Projection;
CMeshMathSurface g_Surface;
ID3D11Texture2D* g_pNormalMap;
ID3D11Texture2D* g_pEnvMap;

int g_iFrames = 0;
double g_dStarttime = 0;
float g_fFps = 0.0f;
bool g_bChangeFPS = false;

bool g_bLeft = 0, g_bRight = 0;
bool g_bUp = 0, g_bDown = 0;
bool g_bForward= 0, g_bBackward = 0;
bool g_bTurnLeft = 0, g_bTurnRight = 0;
bool g_bTurnUp = 0, g_bTurnDown = 0, g_bTurnS = 0, g_bTurnS1 = 0;
bool g_onFirstMouseMove = 1;

int g_iWidth;
int g_iHeight;

int lastX, lastY;
int mouseX, mouseY;

#define VK_A 0x41
#define VK_Q 0x51
#define VK_J 0x4A
#define VK_L 0x4C
#define VK_K 0x4B
#define VK_I 0x49
#define VK_U 0x55
#define VK_O 0x4F

/*
float Plane(float x, float y)
{
	return 0.0f;
}

VECTOR4D PlaneNormalize	(float x, float y, float z)
{
	VECTOR4D Normal = {
		0,
		0,
		1,
		0
	};
	return Normalize(Normal);
}*/

float SinCos(float x, float y)
{
	return cos(5*x)*sin(5*y);
}

VECTOR4D SinCosNormal(float x, float y, float z)
{
	VECTOR4D Normal = {
		5 * sin(5 * x)*sin(5 * y),
		-5 * cos(5 * x)*cos(5 * y),
		1,
		0
	};
	return Normalize(Normal);
}
#define M_PI 3.14159265358979323846
VECTOR4D Sphere1(float u, float v)
{
	float r = 1.f;
	VECTOR4D v1;
	v1.x = r*cos(2 * M_PI *u)*sin(M_PI *v);
	v1.y = r*sin(2 * M_PI*u)*sin(M_PI*v);
	v1.z = r*cos(M_PI*v);
	v1.w = 1;
	return v1;
}

void UpdateCamera()
{
	// Control de camara
	MATRIX4D InvV = FastInverse(g_View);
	VECTOR4D XDir = { InvV.m00, InvV.m01, InvV.m02, 0 };
	VECTOR4D YDir = { InvV.m10, InvV.m11, InvV.m12, 0 };
	VECTOR4D ZDir = { InvV.m20, InvV.m21, InvV.m22, 0 };
	VECTOR4D EyePos = { InvV.m30, InvV.m31, InvV.m32, 1 };


	MATRIX4D O = InvV;
	O.m03 = 0;
	O.m13 = 0;
	O.m23 = 0;

	VECTOR4D Speed = { 0.1, 0.1, 0.1, 0 };
	bool movePos = false;
	if (g_bBackward)
	{
		EyePos = EyePos - ZDir*Speed;
		movePos = true;
	}
	if (g_bForward)
	{
		EyePos = EyePos + ZDir*Speed;
		movePos = true;
	}
	if (g_bLeft)
	{
		EyePos = EyePos - XDir*Speed;
		movePos = true;
	}
	if (g_bRight)
	{
		EyePos = EyePos + XDir*Speed;
		movePos = true;
	}
	if (g_bUp)
	{
		EyePos = EyePos + YDir*Speed;
		movePos = true;
	}
	if (g_bDown)
	{
		EyePos = EyePos - YDir*Speed;
		movePos = true;
	}

	if (movePos)
	{
		//g_onFirstMouseMove = true;
	}

	g_Painter.m_Params.lights[1].Position = EyePos;
	g_Painter.m_Params.lights[1].Direction = ZDir;
	float speed = .02;

	if (g_bTurnLeft)
	{
		MATRIX4D R = RotationAxis(-speed, YDir);
		O = O*R;
	}
	if (g_bTurnRight)
	{
		MATRIX4D R = RotationAxis(speed, YDir);
		O = O*R;
	}
	if (g_bTurnUp)
	{
		MATRIX4D R = RotationAxis(-speed, XDir);
		O = O*R;
	}
	if (g_bTurnDown)
	{
		MATRIX4D R = RotationAxis(speed, XDir);
		O = O*R;
	}
	if (g_bTurnS)
	{
		MATRIX4D R = RotationAxis(speed, ZDir);
		O = O*R;
	}
	if (g_bTurnS1)
	{
		MATRIX4D R = RotationAxis(-speed, ZDir);
		O = O*R;
	}

	
		
		if (g_onFirstMouseMove)
		{
			lastX = g_iWidth/2;
			lastY = g_iHeight/2;

			g_onFirstMouseMove = false;
		}
		else
		{
			
			if (mouseX - lastX != 0)
			{
				float diffX = (float)(mouseX)-(lastX);
				diffX /= g_iWidth/2;

				MATRIX4D R = RotationAxis(-diffX, YDir);
				//O = O*R;
				
			}
			
			if (mouseY - lastY != 0)
			{
				float diffY = (float)mouseY - lastY;
				diffY /= g_iHeight/2;
				MATRIX4D R = RotationAxis(-diffY, XDir);
				//O = O*R;
			}

			lastX = mouseX;
			lastY = mouseY;

		}


	InvV = O;

	InvV.m30 = EyePos.x;
	InvV.m31 = EyePos.y;
	InvV.m32 = EyePos.z;

	g_View = Orthogonalize(FastInverse(InvV));
}

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DIRECT3D2016B, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DIRECT3D2016B));


	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (g_bChangeFPS)
		{
			std::wostringstream ss;
			ss << g_fFps;
			wchar_t Title[1024];
			wsprintf(Title, L"Iteso Direct3D 2016B FPS: %s", ss.str().c_str());
			SetWindowText(g_hWnd, Title);
			g_bChangeFPS = false;
		}

	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIRECT3D2016B));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= 0; //(HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DIRECT3D2016B);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable

   g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!g_hWnd)
   {
      return FALSE;
   }
   IDXGIAdapter* pAdapter=g_Manager.EnumAndChooseAdapter(NULL);
   if (!pAdapter || !g_Manager.Initialize(g_hWnd, pAdapter) || !g_FX.Initialize())
   {
		   MessageBox(NULL,
			   L"No se ha podido inicializar Direct3D 11",
			   L"Error fatal", MB_ICONERROR);
		   SAFE_RELEASE(pAdapter);
		   return NULL;
   }
   wchar_t Title[1024];
   DXGI_ADAPTER_DESC dad;
   pAdapter->GetDesc(&dad);
   wsprintf(Title, L"Iteso Direct3D 2016B %s", dad.Description);
   SetWindowText(g_hWnd, Title);
   SAFE_RELEASE(pAdapter);
   if (!g_Painter.Initialize())
   {
	   MessageBox(NULL, L"No se pudo iniciar contexto de dibujo",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   } 
   g_pSysTexture = CImageBMP::
	   CreateBitmapFromFile("..\\Assets\\tela.bmp",NULL);
   if (!g_pSysTexture)
   {
	   MessageBox(NULL, L"No se pudo cargar textura desde archivo",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   }
   g_pTexture = g_pSysTexture->CreateTexture(&g_Manager);
   if (!g_pTexture)
   {
	   MessageBox(NULL, L"No se pudo cargar textura al GPU",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   }
   CImageBMP* pImage =  CImageBMP::CreateBitmapFromFile("..\\Assets\\ladrillo.bmp", NULL);
   
   if (!pImage)
   {
	   MessageBox(NULL, L"No se pudo cargar textura desde archivo",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   }

   g_pNormalMap = pImage->CreateTexture(&g_Manager);
   if (!g_pNormalMap)
   {
	   MessageBox(NULL, L"No se pudo cargar textura al GPU",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   }
   CImageBMP::DestroyBitmap(pImage);

   pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\SanPedro.bmp", NULL);

   if (!pImage)
   {
	   MessageBox(NULL, L"No se pudo cargar textura desde archivo",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   }

   g_pEnvMap = pImage->CreateTexture(&g_Manager);

   if (!g_pEnvMap)
   {
	   MessageBox(NULL, L"No se pudo cargar textura al GPU",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   }

   CImageBMP::DestroyBitmap(pImage);

   pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\Normal.bmp", NULL);

   if (!pImage)
   {
	   MessageBox(NULL, L"No se pudo cargar textura desde archivo",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   }

   g_pNormalMapTrue = pImage->CreateTexture(&g_Manager);

   if (!g_pNormalMapTrue)
   {
	   MessageBox(NULL, L"No se pudo cargar textura al GPU",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   }

   CImageBMP::DestroyBitmap(pImage);

   pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\Emissive.bmp", NULL);

   if (!pImage)
   {
	   MessageBox(NULL, L"No se pudo cargar textura desde archivo",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   }

   g_pEmissiveMap = pImage->CreateTexture(&g_Manager);

   if (!g_pEmissiveMap)
   {
	   MessageBox(NULL, L"No se pudo cargar textura al GPU",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   }

   CImageBMP::DestroyBitmap(pImage);

   // Init FPS
   g_iFrames = 0;
   g_dStarttime = GetTickCount();

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
#define SURFACE_RESOLUTION 50
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static float theta;

	switch (message)
	{
	case WM_CREATE:
		g_World = Identity();
		{
			VECTOR4D EyePos = { 6, 6, 6, 1 };
			VECTOR4D Target = { 0, 0, 0, 1 };
			VECTOR4D Up = { 0, 0, 1, 0 };
			g_View = View(EyePos, Target, Up);
			g_Projection = PerspectiveWidthHeightLH(0.05, 0.05, 0.1, 100);
			//g_Surface.BuildAnalyticSurface(SURFACE_RESOLUTION, SURFACE_RESOLUTION, -1, -1, 2.0f / (SURFACE_RESOLUTION - 1), 2.0f / (SURFACE_RESOLUTION - 1), SinCos, SinCosNormal);
			//g_Surface.BuildTextureCoords(0, 0, 1.0f / (SURFACE_RESOLUTION - 1), 1.0f / (SURFACE_RESOLUTION - 1));
			//g_Surface.BuildAnalyticSurface(SURFACE_RESOLUTION, SURFACE_RESOLUTION, -3, -3, 6.0f / (SURFACE_RESOLUTION - 1), 6.0f / (SURFACE_RESOLUTION - 1), Plane, PlaneNormalize);
			//g_Surface.BuildTextureCoords(0, 0, 1.0f / (SURFACE_RESOLUTION - 1), 1.0f / (SURFACE_RESOLUTION - 1));
			//g_Surface.BuildParametricSurface(SURFACE_RESOLUTION, SURFACE_RESOLUTION, 0, 0, 1.0f / (SURFACE_RESOLUTION - 1), 1.0f / (SURFACE_RESOLUTION - 1), Sphere1);
			//g_Surface.BuildTextureCoords(0, 0, 1.0f / (SURFACE_RESOLUTION - 1), 1.0f / (SURFACE_RESOLUTION - 1));
			g_Surface.LoadSuzanne();
			g_Surface.Optimize();
			g_Surface.BuildTangentSpaceFromTexCoordsIndexed(true);
			VECTOR4D White = { 1, 1, 1, 1 };
			g_Surface.SetColor(White, White, White, White);
		}
		SetTimer(hWnd, 1, 10, NULL);
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			theta += 0.01;
			InvalidateRect(hWnd, 0, false);
		default:
			break;
		}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		
		if (g_Manager.GetSwapChain())
		{
			// Crear Render Target Auxiliar
			
			ID3D11Texture2D* pBackBuffer = 0;
			g_Manager.GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
			//ID3D11Texture2D* pBackBuffer = NULL;
			//g_Manager.GetMainRTV()->GetResource((ID3D11Resource**) &pBackBuffer);
			D3D11_TEXTURE2D_DESC dtd; 
			pBackBuffer->GetDesc(&dtd);
			dtd.BindFlags |= (D3D11_BIND_SHADER_RESOURCE| D3D11_BIND_RENDER_TARGET);

			HRESULT hr =  g_Manager.GetDevice()->CreateTexture2D(&dtd, 0, &g_pRT0);
			hr =  g_Manager.GetDevice()->CreateShaderResourceView(g_pRT0, 0, &g_pSRV0);
			hr =  g_Manager.GetDevice()->CreateRenderTargetView(g_pRT0, 0 , &g_pRTV0);


			SAFE_RELEASE(pBackBuffer);
			  

			// Update FPS
			float currentTime = (float)GetTickCount();

			if (currentTime - g_dStarttime > 500)
			{
				float time = (currentTime - g_dStarttime)/1000; // Time in seconds
				g_fFps = (double)g_iFrames / (time);
				g_dStarttime = currentTime;
				g_iFrames = 0;
				g_bChangeFPS = true;
			}
			g_iFrames++;
			

			RECT rect;
			GetWindowRect(hWnd, &rect);
			g_iWidth = rect.right - rect.left;
			g_iHeight = rect.bottom - rect.top;
			MATRIX4D AC = Scaling((float)g_iHeight / g_iWidth, 1, 1);

			VECTOR4D DarkGray = { 0.25,0.25,0.25,1 };
			VECTOR4D White = { 1,1,1,1 };
			VECTOR4D Gray = { .5,.5,.5,0 };

			g_Painter.SetRenderTarget(g_pRTV0);
			//g_Painter.SetRenderTarget(g_Manager.GetMainRTV());
			g_Painter.m_Params.Material.Diffuse = Gray;
			g_Painter.m_Params.Material.Ambient = Gray;

			VECTOR4D NightBlue = { 0,0,.1, 0 };
			g_Manager.GetContext()->ClearRenderTargetView(g_pRTV0, (float*)&White);
			g_Manager.GetContext()->ClearRenderTargetView(g_Manager.GetMainRTV(), (float*)&NightBlue);
			g_Manager.GetContext()->ClearDepthStencilView(
				g_Manager.GetMainDSV(), 
				D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
				1.0f, 
				0);
			
			unsigned long TriangleIndices[3] = { 0, 1, 2 };
			g_World = Identity();//RotationY(theta);
			g_Painter.m_Params.World = g_World;
			g_Painter.m_Params.View = g_View;
			g_Painter.m_Params.Projection = g_Projection*AC;

			// Actualizar camara si fue movida
			UpdateCamera();

			VECTOR4D Color = { 0, 0, 0, 0 };
			g_Painter.m_Params.Brightness = Color;
			g_Painter.m_Params.Flags1 =    MAPPING_NORMAL_TRUE | MAPPING_DIFFUSE | MAPPING_EMISSIVE;
			ID3D11ShaderResourceView* pSRV = NULL;
			g_Manager.GetDevice()->CreateShaderResourceView(g_pTexture, NULL, &pSRV);
			g_Manager.GetContext()->PSSetShaderResources(0, 1, &pSRV);
			ID3D11ShaderResourceView* pSRVNormalMap = NULL;
			g_Manager.GetDevice()->CreateShaderResourceView(g_pNormalMap, NULL, &pSRVNormalMap);
			g_Manager.GetContext()->PSSetShaderResources(1, 1, &pSRVNormalMap);
			ID3D11ShaderResourceView* pSRVEnvMap = NULL;
			g_Manager.GetDevice()->CreateShaderResourceView(g_pEnvMap, NULL, &pSRVEnvMap);
			g_Manager.GetContext()->PSSetShaderResources(2, 1, &pSRVEnvMap);
			ID3D11ShaderResourceView* pSRVNormalMapTrue = NULL;
			g_Manager.GetDevice()->CreateShaderResourceView(g_pNormalMapTrue, NULL, &pSRVNormalMapTrue);
			g_Manager.GetContext()->PSSetShaderResources(3, 1, &pSRVNormalMapTrue);
			ID3D11ShaderResourceView* pSRVEmissiveMap = NULL;
			g_Manager.GetDevice()->CreateShaderResourceView(g_pEmissiveMap, NULL, &pSRVEmissiveMap);
			g_Manager.GetContext()->PSSetShaderResources(4, 1, &pSRVEmissiveMap);

			g_Painter.DrawIndexed(&g_Surface.m_Vertices[0], g_Surface.m_Vertices.size(), &g_Surface.m_Indices[0], g_Surface.m_Indices.size());

			// Set the main render target view
			//g_Manager.GetContext()->ClearState();
			//g_FX.SetRenderTarget(g_Manager.GetMainRTV());
			g_Manager.GetContext()->OMSetRenderTargets(1, &g_Manager.GetMainRTV(), g_Manager.GetMainDSV());
			g_Manager.GetContext()->PSSetShaderResources(0, 1, &g_pSRV0);
			g_FX.Process(g_iWidth, g_iHeight);

			g_Manager.GetSwapChain()->Present(1,0);
			SAFE_RELEASE(pSRV);
			SAFE_RELEASE(pSRVNormalMap);
			SAFE_RELEASE(pSRVEnvMap);
			SAFE_RELEASE(pSRVNormalMapTrue);
			SAFE_RELEASE(pSRVEmissiveMap);
			SAFE_RELEASE(g_pSRV0);
			SAFE_RELEASE(g_pRTV0);
			SAFE_RELEASE(g_pRT0);
		}
		ValidateRect(hWnd, NULL);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case VK_LEFT:
			g_bLeft = false;
			break;
		case VK_RIGHT:
			g_bRight = false;
			break;
		case VK_UP:
			g_bForward = false;
			break;
		case VK_DOWN:
			g_bBackward = false;
			break;
		case VK_A:
			g_bUp = false;
			break;
		case VK_Q:
			g_bDown = false;
			break;
		case VK_J:
			g_bTurnLeft = false;
			break;
		case VK_L:
			g_bTurnRight = false;
			break;
		case VK_K:
			g_bTurnDown = false; 
			break;
		case VK_I:
			g_bTurnUp = false;
			break;
		case VK_U:
			g_bTurnS = false;
			break;
		case VK_O:
			g_bTurnS1 = false;
			break;
		default:
			break;
		}

	}
	break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_LEFT:
			g_bLeft = true;
			break;
		case VK_RIGHT:
			g_bRight = true; 
			break;
		case VK_UP:
			g_bForward = true;
			break;
		case VK_DOWN:
			g_bBackward = true;
			break;
		case VK_A:
			g_bUp = true;
			break;
		case VK_Q:
			g_bDown = true; 
			break;
		case VK_J:
			g_bTurnLeft = true;
			break;
		case VK_L:
			g_bTurnRight = true;
			break;
		case VK_K:
			g_bTurnDown = true;
			break;
		case VK_I:
			g_bTurnUp = true;
			break;
		case VK_U:
			g_bTurnS = true;
			break;
		case VK_O:
			g_bTurnS1 = true;
			break;
		default:
			break;
		}

	}
	break;
	case WM_MOUSEMOVE:
	{
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);

	}
	break;
	case WM_SIZE:
	{
		g_iWidth = LOWORD(lParam);
		g_iHeight = HIWORD(lParam);
		g_Manager.Resize(g_iWidth, g_iHeight);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
