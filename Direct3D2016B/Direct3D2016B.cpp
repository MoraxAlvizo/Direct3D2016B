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
#include "DDSTextureLoader.h"
#include "Octree.h"
#include "MeshCollision.h"
#include "OctreeCube.h"

/* assimp include files. These three are usually needed. */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cfloat>

#define MAX_LOADSTRING 100

// Global Variables:
HWND g_hWnd;
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
CDXManager g_Manager;
CDXBasicPainter g_Painter(&g_Manager);
unsigned long g_lFlagsPainter = 0;//FOG_ENABLE;//MAPPING_NORMAL_TRUE | MAPPING_DIFFUSE;
bool g_bSky = false;
CFX g_FX(&g_Manager);
CImageBMP*      g_pSysTexture; //CPU
ID3D11Texture2D* g_pTexture;   //GPU
ID3D11Texture2D* g_pNormalMapTrue; 
ID3D11Texture2D* g_pEmissiveMap;
ID3D11Texture2D* g_pCubeMap;
HMENU g_hMenu, g_hSubMenu;

// Render Targets
ID3D11Texture2D* g_pRT0;			// Memoria
ID3D11ShaderResourceView* g_pSRV0;	// Input
ID3D11RenderTargetView* g_pRTV0;		// Output

ID3D11Texture2D* g_pRT1;			// Memoria
ID3D11ShaderResourceView* g_pSRV1;	// Input
ID3D11RenderTargetView* g_pRTV1;		// Output

COctreeCube* g_pOctree2 = NULL;

enum
{
	BP_MEDIO,
	BP_CUARTO,
	BP_OCTAVO,
	BP_SIZE
};

ID3D11ShaderResourceView* g_pSRVCubeMap;


ID3D11Texture2D* g_pRTBrightPass[BP_SIZE];			// Memoria
ID3D11ShaderResourceView* g_pSRVBrightPass[BP_SIZE];	// Input
ID3D11RenderTargetView* g_pRTVBrightPass[BP_SIZE];		// Output

MATRIX4D g_World;
MATRIX4D g_View;
MATRIX4D g_Projection;
CMeshMathSurface g_Surface;
vector<CMeshCollision> g_Scene;
CMeshMathSurface g_Mirrow;
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
bool g_bMoveSphere1 = 0;
bool g_bMoveSphere2 = 0;

float g_iWidth;
float g_iHeight;

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
#define VK_1 97
#define VK_2 98

// Add new popup menu
#define ADDPOPUPMENU(hSubMenu, hmenu, string) \
	AppendMenu(hmenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, string);

// Add a menu item
#define ADDMENUITEM(hSubMenu, hmenu, ID, string) \
	AppendMenu(hSubMenu, MF_STRING, ID, string);
// Crear menu
enum
{
	ID_MAPPING_DIFFUSE = 200,
	ID_MAPPING_NORMAL,
	ID_MAPPING_ENVIROMENTAL_FAST,
	ID_MAPPING_ENVIROMENTAL_SKY,
	ID_MAPPING_NORMAL_TRUE,
	ID_MAPPING_EMISSIVE,
	ID_MAPPING_SHADOW,
	ID_SKY_EFFECT,
	ID_FLOG_ENABLE,
	ID_MIRROW_ENABLE,
	ID_FX_EDGE_DETECT,
	ID_FX_RADIAN_BLUR, 
	ID_FX_DIRECTIONAL_BLUR, 
	ID_FX_GAUSS_BLUR , 
	ID_FX_BLOOM_EFFECT,
	ID_FX_NONE,
	ID_LIGHT_0,
	ID_LIGHT_1,
	ID_LIGHT_2,
	ID_LIGHT_3,
	ID_LIGHT_4,
	ID_LIGHT_5,
	ID_LIGHT_6,
	ID_LIGHT_7
};

long g_lFXIDEffect = ID_FX_NONE;

void CreateMainMenu(HWND hWnd)
{

	g_hMenu = CreateMenu();
	g_hSubMenu = CreatePopupMenu();
	AppendMenu(g_hSubMenu, MF_STRING, ID_MAPPING_DIFFUSE, L"&MAPPING DIFFUSE");
	CheckMenuItem(g_hSubMenu, ID_MAPPING_DIFFUSE, MF_UNCHECKED);
	AppendMenu(g_hSubMenu, MF_STRING, ID_MAPPING_NORMAL, L"&MAPPING NORMAL");
	CheckMenuItem(g_hSubMenu, ID_MAPPING_NORMAL, MF_UNCHECKED);
	AppendMenu(g_hSubMenu, MF_STRING, ID_MAPPING_ENVIROMENTAL_FAST, L"&MAPPING ENVIROMENTAL FAST");
	CheckMenuItem(g_hSubMenu, ID_MAPPING_ENVIROMENTAL_FAST, MF_UNCHECKED);
	AppendMenu(g_hSubMenu, MF_STRING, ID_MAPPING_ENVIROMENTAL_SKY, L"&MAPPING ENVIROMENTAL SKY");
	CheckMenuItem(g_hSubMenu, ID_MAPPING_ENVIROMENTAL_SKY, MF_UNCHECKED);
	AppendMenu(g_hSubMenu, MF_STRING, ID_MAPPING_NORMAL_TRUE, L"&MAPPING NORMAL TRUE");
	CheckMenuItem(g_hSubMenu, ID_MAPPING_NORMAL_TRUE, MF_UNCHECKED);
	AppendMenu(g_hSubMenu, MF_STRING, ID_MAPPING_EMISSIVE, L"&MAPPING EMISSIVE");
	CheckMenuItem(g_hSubMenu, ID_MAPPING_EMISSIVE, MF_UNCHECKED);
	AppendMenu(g_hSubMenu, MF_STRING, ID_MAPPING_SHADOW, L"&MAPPING SHADOW");
	CheckMenuItem(g_hSubMenu, ID_MAPPING_SHADOW, MF_UNCHECKED);
	AppendMenu(g_hSubMenu, MF_STRING, ID_SKY_EFFECT, L"&SKY");
	CheckMenuItem(g_hSubMenu, ID_SKY_EFFECT, MF_UNCHECKED);
	AppendMenu(g_hSubMenu, MF_STRING, ID_FLOG_ENABLE, L"&Flog");
	CheckMenuItem(g_hSubMenu, ID_FLOG_ENABLE, MF_UNCHECKED);
	AppendMenu(g_hSubMenu, MF_STRING, ID_MIRROW_ENABLE, L"&Mirror");
	CheckMenuItem(g_hSubMenu, ID_MIRROW_ENABLE, MF_UNCHECKED);
	AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT)g_hSubMenu, L"&3D Effects");


	g_hSubMenu = CreatePopupMenu();
	AppendMenu(g_hSubMenu, MF_STRING, ID_FX_EDGE_DETECT, L"&EDGE DETECT");
	AppendMenu(g_hSubMenu, MF_STRING, ID_FX_RADIAN_BLUR, L"&RADIAN BLUR");
	AppendMenu(g_hSubMenu, MF_STRING, ID_FX_DIRECTIONAL_BLUR, L"&DIRECTIONAL BLUR");
	AppendMenu(g_hSubMenu, MF_STRING, ID_FX_GAUSS_BLUR, L"&GAUSS BLUR");
	AppendMenu(g_hSubMenu, MF_STRING, ID_FX_BLOOM_EFFECT, L"&BLOOM EFFECT");
	AppendMenu(g_hSubMenu, MF_STRING, ID_FX_NONE, L"&NONE EFFECT");


	CheckMenuRadioItem(g_hSubMenu, ID_FX_EDGE_DETECT, ID_FX_NONE,
		ID_FX_NONE, MF_BYCOMMAND);

	AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT)g_hSubMenu, L"&FX Effects");

	g_hSubMenu = CreatePopupMenu();

	AppendMenu(g_hSubMenu, MF_STRING, ID_LIGHT_0, L"&LIGHT 0");
	AppendMenu(g_hSubMenu, MF_STRING, ID_LIGHT_1, L"&LIGHT 1");
	AppendMenu(g_hSubMenu, MF_STRING, ID_LIGHT_2, L"&LIGHT 2");
	AppendMenu(g_hSubMenu, MF_STRING, ID_LIGHT_3, L"&LIGHT 3");
	AppendMenu(g_hSubMenu, MF_STRING, ID_LIGHT_4, L"&LIGHT 4");
	AppendMenu(g_hSubMenu, MF_STRING, ID_LIGHT_5, L"&LIGHT 5");
	AppendMenu(g_hSubMenu, MF_STRING, ID_LIGHT_6, L"&LIGHT 6");
	AppendMenu(g_hSubMenu, MF_STRING, ID_LIGHT_7, L"&LIGHT 7");

	CheckMenuItem(g_hSubMenu, ID_LIGHT_0, MF_CHECKED);
	AppendMenu(g_hMenu, MF_STRING | MF_POPUP, (UINT)g_hSubMenu, L"&Lights");

	SetMenu(hWnd, g_hMenu);
}

float Plane(float x, float y)
{
	return -5.0f;
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
}

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

	// Set camara pos in params 
	g_Painter.m_Params.CameraPosition = EyePos;
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

				MATRIX4D R = RotationAxis(-speed*(diffX > 0 ? 1 : -1), YDir);
				//O = O*R;
				
			}
			
			if (mouseY - lastY != 0)
			{
				float diffY = (float)mouseY - lastY;
				diffY /= g_iHeight/2;
				
				MATRIX4D R = RotationAxis(-speed*(diffY > 0?1:0), XDir);
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
	//Get a console handle
	HWND myconsole = GetConsoleWindow();

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
	CreateMainMenu(g_hWnd);

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

   pImage = CImageBMP::CreateBitmapFromFile("..\\Assets\\cubemap.bmp", NULL);

   if (!pImage)
   {
	   MessageBox(NULL, L"No se pudo cargar textura desde archivo",
		   L"Verificar recursos sombreadores", MB_ICONERROR);
	   return FALSE;
   }

   g_pCubeMap = pImage->CreateTexture(&g_Manager);

   if (!g_pCubeMap)
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
	case WM_CHAR:
		if (wParam == 'p')
			g_lFlagsPainter ^= DRAW_OCTREE;
		
		break;
	case WM_CREATE:
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		g_World = Identity();
		{
			/* the global Assimp scene object */
			const struct aiScene* scene = aiImportFile("..\\Assets\\spheres.blend", aiProcessPreset_TargetRealtime_MaxQuality);

			g_Scene.resize(scene->mNumMeshes);
			for (unsigned long i = 0; i < scene->mNumMeshes; i++)
			{
				float maxX, maxY, maxZ;
				float minX, minY, minZ;

				maxX = maxY = maxZ = FLT_MIN;
				minX = minY = minZ = FLT_MAX;

				g_Scene[i].m_Vertices.resize(scene->mMeshes[i]->mNumVertices);
				for (unsigned long j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
				{
					g_Scene[i].m_Vertices[j].Position = {
						scene->mMeshes[i]->mVertices[j].x,
						scene->mMeshes[i]->mVertices[j].y,
						scene->mMeshes[i]->mVertices[j].z,
						1 };
					if (scene->mMeshes[i]->mVertices[j].x > maxX)
						maxX = scene->mMeshes[i]->mVertices[j].x ;
					if (scene->mMeshes[i]->mVertices[j].y > maxY)
						maxY = scene->mMeshes[i]->mVertices[j].y;
					if (scene->mMeshes[i]->mVertices[j].z > maxZ)
						maxZ = scene->mMeshes[i]->mVertices[j].z;

					if (scene->mMeshes[i]->mVertices[j].x < minX)
						minX = scene->mMeshes[i]->mVertices[j].x;
					if (scene->mMeshes[i]->mVertices[j].y < minY)
						minY = scene->mMeshes[i]->mVertices[j].y;
					if (scene->mMeshes[i]->mVertices[j].z < minZ)
						minZ = scene->mMeshes[i]->mVertices[j].z;
				}

				g_Scene[i].m_Box.min = { minX, minY, minZ, 1 };
				g_Scene[i].m_Box.max = { maxX, maxY, maxZ, 1 };

				g_Scene[i].m_octree = new COctree(g_Scene[i].m_Box.min, g_Scene[i].m_Box.max , 0, &g_Painter);
				g_Scene[i].m_octree->m_Color = { i%2? 1.f:0.f , 1,i % 3 ? 1.f : 0.f,0 };

				MATRIX4D t;
				t.m00 = scene->mRootNode->mChildren[i]->mTransformation.a1;
				t.m01 = scene->mRootNode->mChildren[i]->mTransformation.a2;
				t.m02 = scene->mRootNode->mChildren[i]->mTransformation.a3;
				t.m03 = scene->mRootNode->mChildren[i]->mTransformation.a4;
				t.m10 = scene->mRootNode->mChildren[i]->mTransformation.b1;
				t.m11 = scene->mRootNode->mChildren[i]->mTransformation.b2;
				t.m12 = scene->mRootNode->mChildren[i]->mTransformation.b3;
				t.m13 = scene->mRootNode->mChildren[i]->mTransformation.b4;
				t.m20 = scene->mRootNode->mChildren[i]->mTransformation.c1;
				t.m21 = scene->mRootNode->mChildren[i]->mTransformation.c2;
				t.m22 = scene->mRootNode->mChildren[i]->mTransformation.c3;
				t.m23 = scene->mRootNode->mChildren[i]->mTransformation.c4;
				t.m30 = scene->mRootNode->mChildren[i]->mTransformation.d1;
				t.m31 = scene->mRootNode->mChildren[i]->mTransformation.d2;
				t.m32 = scene->mRootNode->mChildren[i]->mTransformation.d3;
				t.m33 = scene->mRootNode->mChildren[i]->mTransformation.d4;

				g_Scene[i].m_World = Transpose(t);

				g_Scene[i].m_Indices.resize(scene->mMeshes[i]->mNumFaces * scene->mMeshes[i]->mFaces[0].mNumIndices);
				for (unsigned long j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
				{
					for (unsigned long k = 0; k < scene->mMeshes[i]->mFaces[j].mNumIndices; k++)
					{
						g_Scene[i].m_Indices[j*scene->mMeshes[i]->mFaces[j].mNumIndices + k] = scene->mMeshes[i]->mFaces[j].mIndices[k];
					}
					
				}
				for (unsigned long j = 0; j < g_Scene[i].m_Vertices.size(); j++)
				{
					VECTOR4D TexCoord = { 0,0,0,0 };
					TexCoord.x = g_Scene[i].m_Vertices[j].Position.x;
					TexCoord.y = g_Scene[i].m_Vertices[j].Position.z;
					TexCoord.z = g_Scene[i].m_Vertices[j].Position.y;
					TexCoord = Normalize(TexCoord);
					TexCoord.x = TexCoord.x * 0.5 + 0.5;
					TexCoord.y = TexCoord.y * 0.5 + 0.5;

					g_Scene[i].m_Vertices[j].TexCoord = TexCoord;
				}
				//g_Scene[i].Optimize();
				g_Scene[i].BuildTangentSpaceFromTexCoordsIndexed(true);
				g_Scene[i].GenerarCentroides();
			}

			VECTOR4D White = { 1, 1, 1, 1 };
			VECTOR4D EyePos = { 6, 10, 6, 1 };
			VECTOR4D Target = { 0, 0, 0, 1 };
			VECTOR4D Up = { 0, 0, 1, 0 };
			g_View = View(EyePos, Target, Up);
			g_Projection = PerspectiveWidthHeightLH(0.05, 0.05, 0.1, 100);
			//g_Surface.BuildAnalyticSurface(SURFACE_RESOLUTION, SURFACE_RESOLUTION, -1, -1, 2.0f / (SURFACE_RESOLUTION - 1), 2.0f / (SURFACE_RESOLUTION - 1), SinCos, SinCosNormal);
			//g_Surface.BuildTextureCoords(0, 0, 1.0f / (SURFACE_RESOLUTION - 1), 1.0f / (SURFACE_RESOLUTION - 1));
			g_Mirrow.BuildAnalyticSurface(2, 2, -5, -5, 10.0f / (2 - 1), 10.0f / (2 - 1), Plane, PlaneNormalize);
			g_Mirrow.BuildTextureCoords(0, 0, 1.0f / (2 - 1), 1.0f / (2 - 1));
			g_Mirrow.BuildTangentSpaceFromTexCoordsIndexed(false);
			g_Mirrow.SetColor(White, White, White, White);
			//g_Surface.BuildParametricSurface(SURFACE_RESOLUTION, SURFACE_RESOLUTION, 0, 0, 1.0f / (SURFACE_RESOLUTION - 1), 1.0f / (SURFACE_RESOLUTION - 1), Sphere1);
			//g_Surface.BuildTextureCoords(0, 0, 1.0f / (SURFACE_RESOLUTION - 1), 1.0f / (SURFACE_RESOLUTION - 1));
			g_Surface.LoadSuzanne();
			//g_Surface.Optimize();
			g_Surface.BuildTangentSpaceFromTexCoordsIndexed(true);
			
			g_Surface.SetColor(White, White, White, White);

			// Crear Render Target Auxiliar
			if (!g_pSRVCubeMap)
			{
				DirectX::CreateDDSTextureFromFile(g_Manager.GetDevice(), L"..\\Assets\\snowcube1024.dds", NULL, &g_pSRVCubeMap);
			}
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
		case ID_MAPPING_DIFFUSE :
			g_lFlagsPainter ^= MAPPING_DIFFUSE;
			if(g_lFlagsPainter & MAPPING_DIFFUSE)
				CheckMenuItem(g_hMenu, ID_MAPPING_DIFFUSE, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_MAPPING_DIFFUSE, MF_UNCHECKED);
			break;
		case ID_MAPPING_NORMAL:
			g_lFlagsPainter ^= MAPPING_NORMAL;
			if (g_lFlagsPainter & MAPPING_NORMAL)
				CheckMenuItem(g_hMenu, ID_MAPPING_NORMAL, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_MAPPING_NORMAL, MF_UNCHECKED);
			break;
		case ID_MAPPING_ENVIROMENTAL_FAST:
			g_lFlagsPainter ^= MAPPING_ENVIROMENTAL_FAST;
			if (g_lFlagsPainter & MAPPING_ENVIROMENTAL_FAST)
				CheckMenuItem(g_hMenu, ID_MAPPING_ENVIROMENTAL_FAST, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_MAPPING_ENVIROMENTAL_FAST, MF_UNCHECKED);
			break;
		case ID_MAPPING_ENVIROMENTAL_SKY:
			g_lFlagsPainter ^= MAPPING_ENVIROMENTAL_SKY;
			if (g_lFlagsPainter & MAPPING_ENVIROMENTAL_SKY)
				CheckMenuItem(g_hMenu, ID_MAPPING_ENVIROMENTAL_SKY, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_MAPPING_ENVIROMENTAL_SKY, MF_UNCHECKED);
			break;
		case ID_MAPPING_NORMAL_TRUE:
			g_lFlagsPainter ^= MAPPING_NORMAL_TRUE;
			if (g_lFlagsPainter & MAPPING_NORMAL_TRUE)
				CheckMenuItem(g_hMenu, ID_MAPPING_NORMAL_TRUE, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_MAPPING_NORMAL_TRUE, MF_UNCHECKED);
			break;
		case ID_MAPPING_EMISSIVE:
			g_lFlagsPainter ^= MAPPING_EMISSIVE;
			if (g_lFlagsPainter & MAPPING_EMISSIVE)
				CheckMenuItem(g_hMenu, ID_MAPPING_EMISSIVE, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_MAPPING_EMISSIVE, MF_UNCHECKED);
			break;
		case ID_MAPPING_SHADOW:
			g_lFlagsPainter ^= MAPPING_SHADOW;
			if (g_lFlagsPainter & MAPPING_SHADOW)
				CheckMenuItem(g_hMenu, ID_MAPPING_SHADOW, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_MAPPING_SHADOW, MF_UNCHECKED);
			break;
		case ID_FLOG_ENABLE:
			g_lFlagsPainter ^= FOG_ENABLE;
			if (g_lFlagsPainter & FOG_ENABLE)
				CheckMenuItem(g_hMenu, ID_MAPPING_SHADOW, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_MAPPING_SHADOW, MF_UNCHECKED);
			break;
		case ID_MIRROW_ENABLE:
			g_lFlagsPainter ^= DRAW_MIRROR;
			if (g_lFlagsPainter & DRAW_MIRROR)
				CheckMenuItem(g_hMenu, ID_MIRROW_ENABLE, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_MIRROW_ENABLE, MF_UNCHECKED);
			break;
		case ID_SKY_EFFECT:
			if (g_bSky)
			{
				g_bSky = false;
				CheckMenuItem(g_hMenu, ID_SKY_EFFECT, MF_UNCHECKED);
			}	
			else
			{
				g_bSky = true;
				CheckMenuItem(g_hMenu, ID_SKY_EFFECT, MF_CHECKED);
			}
			break;
		case ID_FX_EDGE_DETECT:
			CheckMenuRadioItem(g_hMenu, ID_FX_EDGE_DETECT, ID_FX_NONE,
				ID_FX_EDGE_DETECT, MF_BYCOMMAND);
			g_lFXIDEffect = wmId;
			break;
		case ID_FX_RADIAN_BLUR:
			CheckMenuRadioItem(g_hMenu, ID_FX_EDGE_DETECT, ID_FX_NONE,
				ID_FX_RADIAN_BLUR, MF_BYCOMMAND);
			g_lFXIDEffect = wmId;
			break;
		case ID_FX_DIRECTIONAL_BLUR:
			CheckMenuRadioItem(g_hMenu, ID_FX_EDGE_DETECT, ID_FX_NONE,
				ID_FX_DIRECTIONAL_BLUR, MF_BYCOMMAND);
			g_lFXIDEffect = wmId;
			break;
		case ID_FX_GAUSS_BLUR:
			CheckMenuRadioItem(g_hMenu, ID_FX_EDGE_DETECT, ID_FX_NONE,
				ID_FX_GAUSS_BLUR, MF_BYCOMMAND);
			g_lFXIDEffect = wmId;
			break;
		case ID_FX_BLOOM_EFFECT:
			CheckMenuRadioItem(g_hMenu, ID_FX_EDGE_DETECT, ID_FX_NONE,
				ID_FX_BLOOM_EFFECT, MF_BYCOMMAND);
			g_lFXIDEffect = wmId;
			break;
		case ID_FX_NONE:
			CheckMenuRadioItem(g_hMenu, ID_FX_EDGE_DETECT, ID_FX_NONE,
				ID_FX_NONE, MF_BYCOMMAND);
			g_lFXIDEffect = wmId;
			break;
		case ID_LIGHT_0:
			g_Painter.m_Params.lights[0].Flags ^= LIGHT_ON;
			if (g_Painter.m_Params.lights[0].Flags & LIGHT_ON)
				CheckMenuItem(g_hMenu, ID_LIGHT_0, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_LIGHT_0, MF_UNCHECKED);
			break;
		case ID_LIGHT_1:
			g_Painter.m_Params.lights[1].Flags ^= LIGHT_ON;
			if (g_Painter.m_Params.lights[1].Flags & LIGHT_ON)
				CheckMenuItem(g_hMenu, ID_LIGHT_1, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_LIGHT_1, MF_UNCHECKED);
			break;
		case ID_LIGHT_2:
			g_Painter.m_Params.lights[2].Flags ^= LIGHT_ON;
			if (g_Painter.m_Params.lights[2].Flags & LIGHT_ON)
				CheckMenuItem(g_hMenu, ID_LIGHT_2, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_LIGHT_2, MF_UNCHECKED);
			break;
		case ID_LIGHT_3:
			g_Painter.m_Params.lights[3].Flags ^= LIGHT_ON;
			if (g_Painter.m_Params.lights[3].Flags & LIGHT_ON)
				CheckMenuItem(g_hMenu, ID_LIGHT_3, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_LIGHT_3, MF_UNCHECKED);
			break;
		case ID_LIGHT_4:
			g_Painter.m_Params.lights[4].Flags ^= LIGHT_ON;
			if (g_Painter.m_Params.lights[4].Flags & LIGHT_ON)
				CheckMenuItem(g_hMenu, ID_LIGHT_4, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_LIGHT_4, MF_UNCHECKED);
			break;
		case ID_LIGHT_5:
			g_Painter.m_Params.lights[5].Flags ^= LIGHT_ON;
			if (g_Painter.m_Params.lights[5].Flags & LIGHT_ON)
				CheckMenuItem(g_hMenu, ID_LIGHT_5, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_LIGHT_5, MF_UNCHECKED);
			break;
		case ID_LIGHT_6:
			g_Painter.m_Params.lights[6].Flags ^= LIGHT_ON;
			if (g_Painter.m_Params.lights[6].Flags & LIGHT_ON)
				CheckMenuItem(g_hMenu, ID_LIGHT_6, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_LIGHT_6, MF_UNCHECKED);
			break;
		case ID_LIGHT_7:
			g_Painter.m_Params.lights[7].Flags ^= LIGHT_ON;
			if (g_Painter.m_Params.lights[7].Flags & LIGHT_ON)
				CheckMenuItem(g_hMenu, ID_LIGHT_7, MF_CHECKED);
			else
				CheckMenuItem(g_hMenu, ID_LIGHT_7, MF_UNCHECKED);
			break;
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
			if (!g_pOctree2)
			{
				g_pOctree2 = new COctreeCube({ -BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2 , 0 },
				{ BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2 }, 0);

				//Create objects
				//int i = 1;
				for (unsigned long i = 0; i < g_Scene.size(); i++)
				{
					g_pOctree2->addObject(&g_Scene[i], 
										  g_Scene[i].m_Box.min * g_Scene[i].m_World,
										  g_Scene[i].m_Box.max * g_Scene[i].m_World);
				}
					

				/*centroids = g_Scene[1].getCentroides();
				for (int z = 0; z < centroids.size(); z++)
				{
				p_octree2->add((Point*)&(centroids[z].position));
				}*/
			}

			ID3D11Texture2D* pBackBuffer = 0;
			g_Manager.GetSwapChain()->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
			//ID3D11Texture2D* pBackBuffer = NULL;
			//g_Manager.GetMainRTV()->GetResource((ID3D11Resource**) &pBackBuffer);

			D3D11_TEXTURE2D_DESC dtd;
			pBackBuffer->GetDesc(&dtd);
			dtd.BindFlags |= (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);

			g_iWidth = dtd.Width;
			g_iHeight = dtd.Height;

			HRESULT hr = g_Manager.GetDevice()->CreateTexture2D(&dtd, 0, &g_pRT0);
			hr = g_Manager.GetDevice()->CreateShaderResourceView(g_pRT0, 0, &g_pSRV0);
			hr = g_Manager.GetDevice()->CreateRenderTargetView(g_pRT0, 0, &g_pRTV0);

			hr = g_Manager.GetDevice()->CreateTexture2D(&dtd, 0, &g_pRT1);
			hr = g_Manager.GetDevice()->CreateShaderResourceView(g_pRT1, 0, &g_pSRV1);
			hr = g_Manager.GetDevice()->CreateRenderTargetView(g_pRT1, 0, &g_pRTV1);

#define RECIPROCO (1.f/2.f)

			for (unsigned long i = 0; i < BP_SIZE; i++)
			{
				dtd.Width = dtd.Width * RECIPROCO;
				dtd.Height = dtd.Height * RECIPROCO;

				hr = g_Manager.GetDevice()->CreateTexture2D(&dtd, 0, &g_pRTBrightPass[i]);
				hr = g_Manager.GetDevice()->CreateShaderResourceView(g_pRTBrightPass[i], 0, &g_pSRVBrightPass[i]);
				hr = g_Manager.GetDevice()->CreateRenderTargetView(g_pRTBrightPass[i], 0, &g_pRTVBrightPass[i]);
			}

			SAFE_RELEASE(pBackBuffer);


			// Update FPS
			float currentTime = (float)GetTickCount();

			if (currentTime - g_dStarttime > 500)
			{
				float time = (currentTime - g_dStarttime) / 1000; // Time in seconds
				g_fFps = (double)g_iFrames / (time);
				g_dStarttime = currentTime;
				g_iFrames = 0;
				g_bChangeFPS = true;
			}
			g_iFrames++;


			/*RECT rect;
			GetWindowRect(hWnd, &rect);
			g_iWidth = rect.right - rect.left;
			g_iHeight = rect.bottom - rect.top;*/
			MATRIX4D AC = Scaling((float)g_iHeight / g_iWidth, 1, 1);

			VECTOR4D DarkGray = { 0.25,0.25,0.25,1 };
			VECTOR4D White = { 1,1,1,1 };
			VECTOR4D Gray = { .5,.5,.5,0 };

			g_Painter.SetRenderTarget(g_lFXIDEffect != ID_FX_NONE ? g_pRTV0 : g_Manager.GetMainRTV());
			//g_Painter.SetRenderTarget(g_Manager.GetMainRTV());
			g_Painter.m_Params.Material.Diffuse = Gray;
			g_Painter.m_Params.Material.Ambient = Gray;

			VECTOR4D NightBlue = { 0,0,.1, 0 };
			g_Manager.GetContext()->ClearRenderTargetView(g_pRTV0, (float*)&NightBlue);
			g_Manager.GetContext()->ClearRenderTargetView(g_Manager.GetMainRTV(), (float*)&NightBlue);
			g_Manager.GetContext()->ClearDepthStencilView(
				g_Manager.GetMainDSV(),
				D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
				1.0f,
				0);

			unsigned long TriangleIndices[3] = { 0, 1, 2 };
			g_World = Identity();
			g_Painter.m_Params.World = g_World;
			g_Painter.m_Params.View = g_View;
			g_Painter.m_Params.Projection = g_Projection*AC;

			// Actualizar camara si fue movida
			UpdateCamera();

			VECTOR4D Color = { 0, 0, 0, 0 };
			g_Painter.m_Params.Brightness = Color;
			g_Painter.m_Params.Flags1 = 0;
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

			// Set cube texture 
			g_Manager.GetContext()->PSSetShaderResources(6, 1, &g_pSRVCubeMap);
			g_Painter.m_Params.Flags1 = g_lFlagsPainter;


			/////////////////////////////////////////////////////////////////
			// DRAW MIRROR
			//////////////////////////////////////////////////////////////////
			if (g_lFlagsPainter & DRAW_MIRROR)
			{
				VECTOR4D Plane = { 0,0,1,5 };
				MATRIX4D Mirror = ReflectionMatrix(Plane);
				MATRIX4D OldView = g_Painter.m_Params.View;

				// Draw plane to mask 
				g_Manager.GetContext()->RSSetState(g_Painter.GetDrawLHRState());
				g_Painter.DrawIndexed(&g_Mirrow.m_Vertices[0], g_Mirrow.m_Vertices.size(), &g_Mirrow.m_Indices[0], g_Mirrow.m_Indices.size(), PAINTER_DRAW_MARK);

				// Limpiar la profuncidad
				g_Manager.GetContext()->ClearDepthStencilView(
					g_Manager.GetMainDSV(),
					D3D11_CLEAR_DEPTH,
					1.0f,
					0);
				// Dibujar en espejo
				g_World = RotationX(theta);//Identity();
				g_Painter.m_Params.World = g_World;
				g_Painter.m_Params.View = Mirror*g_Painter.m_Params.View;
				g_Painter.m_Params.Brightness = Color;
				
				g_Manager.GetContext()->RSSetState(g_Painter.GetDrawRHRState());

				for (unsigned long i = 0; i < g_Scene.size(); i++)
				{
					g_Painter.m_Params.World = g_Scene[i].m_World;
					g_Painter.DrawIndexed(&g_Scene[i].m_Vertices[0], g_Scene[i].m_Vertices.size(), &g_Scene[i].m_Indices[0], g_Scene[i].m_Indices.size(), PAINTER_DRAW_ON_MARK);
				}

				g_Painter.m_Params.View = OldView;
			}

			if (g_lFlagsPainter & MAPPING_SHADOW)
			{
				VECTOR4D LightPos = { 0, 0, 20, 1 };
				VECTOR4D Target = { 0, 0, 0, 1 };
				VECTOR4D Up = { 0, 1, 0, 0 };
				g_Painter.m_Params.LightView = View(LightPos, Target, Up);
				g_Painter.m_Params.LightProjection = PerspectiveWidthHeightLH(1, 1, 1, 100);


				g_Manager.GetContext()->RSSetState(g_Painter.GetDrawLHRState());
				// Dibujar mapa de sombras
				g_Painter.ClearShadow();
				g_Painter.DrawIndexed(&g_Surface.m_Vertices[0], g_Surface.m_Vertices.size(), &g_Surface.m_Indices[0], g_Surface.m_Indices.size(), PAINTER_DRAW, true);

			}

			// Dibujar mundo real
			//g_Painter.DrawIndexed(&g_Surface.m_Vertices[0], g_Surface.m_Vertices.size(), &g_Surface.m_Indices[0], g_Surface.m_Indices.size(), PAINTER_DRAW);
			g_Manager.GetContext()->RSSetState(g_Painter.GetDrawLHRState());

			if(g_bMoveSphere1)
				g_Scene[1].m_World = g_Scene[1].m_World * Translation(0,0, -0.1);
			if (g_bMoveSphere2)
				g_Scene[2].m_World = g_Scene[2].m_World * Translation(0,0, -0.1);
			for (unsigned long i = 0; i < g_Scene.size(); i++)
			{
				g_Painter.m_Params.World =  g_Scene[i].m_World;
				g_Painter.DrawIndexed(&g_Scene[i].m_Vertices[0], g_Scene[i].m_Vertices.size(), &g_Scene[i].m_Indices[0], g_Scene[i].m_Indices.size(), PAINTER_DRAW);
			}

			if (g_lFlagsPainter & DRAW_OCTREE)
			{
				//g_Painter.m_Params.World = Identity();
				g_Painter.m_Params.Flags1 = DRAW_JUST_WITH_COLOR;
				g_pOctree2->DrawOctree(&g_Painter);
				g_pOctree2->printCHildren(0);

				for (unsigned long i = 0; i < g_Scene.size(); i++)
				{
					g_Painter.m_Params.World = g_Scene[i].m_World;
					g_Scene[i].m_octree->DrawOctree();
					
				}
			}
			
			
			//g_FX.SetRenderTarget(g_Manager.GetMainRTV());
			if (g_bSky)
			{
				g_Manager.GetContext()->PSSetShaderResources(4, 1, &g_pSRVCubeMap);
				g_FX.m_Params.WVP = Scaling(50, 50, 50) *g_View * g_Projection *AC;  
				g_Manager.GetContext()->RSSetState(g_Painter.GetDrawLHRState());
				g_FX.SetRenderTarget(g_lFXIDEffect != ID_FX_NONE ? g_pRTV0 : g_Manager.GetMainRTV());
				g_FX.Process(1, 7, g_iWidth, g_iHeight);

			}
			switch (g_lFXIDEffect)
			{
			case ID_FX_EDGE_DETECT:
			{
				g_FX.SetRenderTarget(g_Manager.GetMainRTV());
				g_FX.SetInput(g_pSRV0);
				g_FX.Process(0,0, g_iWidth, g_iHeight);
			}
			break;
			case ID_FX_RADIAN_BLUR:
			{
				g_FX.SetRenderTarget(g_Manager.GetMainRTV());
				g_FX.SetInput(g_pSRV0);
				g_FX.m_Params.RadialBlur.x = .01;
				g_FX.Process(0,1, g_iWidth, g_iHeight);
			}
			break;
			case ID_FX_DIRECTIONAL_BLUR:
			{
				g_FX.SetRenderTarget(g_Manager.GetMainRTV());
				g_FX.SetInput(g_pSRV0);
				g_FX.m_Params.DirectionalBlur = { 1,0,.01f,0 };
				g_FX.Process(0,2, g_iWidth, g_iHeight);
			}
			break;
			case ID_FX_GAUSS_BLUR:
			{
				g_FX.SetRenderTarget(g_pRTV1);
				g_FX.SetInput(g_pSRV0);
				g_FX.m_Params.DirectionalBlur.x = cos(theta);
				g_FX.m_Params.DirectionalBlur.y = sin(theta);
				g_FX.m_Params.DirectionalBlur.z = 0.005;
				g_FX.Process(0,3,g_iWidth, g_iHeight);

				g_FX.SetRenderTarget(g_Manager.GetMainRTV());
				g_FX.SetInput(g_pSRV1);
				g_FX.m_Params.DirectionalBlur.x = cos(theta);
				g_FX.m_Params.DirectionalBlur.y = sin(theta);
				g_FX.m_Params.DirectionalBlur.z = 0.005;
				g_FX.Process(0,4, g_iWidth, g_iHeight);
			}
			break;
			case ID_FX_BLOOM_EFFECT:
			{
				float w = g_iWidth, h = g_iHeight;

				for (unsigned long i = 0; i < BP_SIZE; i++)
				{
					w *= RECIPROCO;
					h *= RECIPROCO;
					g_FX.SetRenderTarget(g_pRTVBrightPass[i]);
					g_FX.SetInput(g_pSRV0);
					g_FX.SetInputBrightPassed(NULL);
					g_FX.m_Params.Umbral.x = .85;
					g_FX.Process(0,5, (float)w, (float)h);
				}

				g_FX.SetRenderTarget(g_Manager.GetMainRTV());
				g_FX.SetInput(g_pSRV0);
				g_FX.SetInputBrightPassed(g_pSRVBrightPass);
				g_FX.Process(0,6, g_iWidth, g_iHeight);

			}
			break;

			default:
				break;
			}

			g_Manager.GetSwapChain()->Present(1,0);
			SAFE_RELEASE(pSRV);
			SAFE_RELEASE(pSRVNormalMap);
			SAFE_RELEASE(pSRVEnvMap);
			SAFE_RELEASE(pSRVNormalMapTrue);
			SAFE_RELEASE(pSRVEmissiveMap);
			SAFE_RELEASE(g_pSRV0);
			SAFE_RELEASE(g_pRTV0);
			SAFE_RELEASE(g_pRT0);
			SAFE_RELEASE(g_pSRV1);
			SAFE_RELEASE(g_pRTV1);
			SAFE_RELEASE(g_pRT1);
			for (unsigned long i = 0; i < BP_SIZE; i++)
			{
				SAFE_RELEASE(g_pSRVBrightPass[i]);
				SAFE_RELEASE(g_pRTVBrightPass[i]);
				SAFE_RELEASE(g_pRTBrightPass[i]);
			}


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
		case VK_1:
			g_bMoveSphere1 = false;
			break;
		case VK_2:
			g_bMoveSphere2 = false;
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
		case VK_1:
			g_bMoveSphere1 = true;
			break;
		case VK_2:
			g_bMoveSphere2 = true;
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
