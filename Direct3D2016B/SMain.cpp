#include "stdafx.h"
#include "SMain.h"
#include "HSM\EventWin32.h"
#include "HSM\StateMachineManager.h"

CSMain::CSMain()
{
}


CSMain::~CSMain()
{
	m_hWnd = NULL;
	m_pDXManager = NULL;
	m_pDXPainter = NULL;
	m_bInitCorrect = TRUE;
	m_FX = NULL;
	m_pSndManager = NULL;
	m_pInputManager = NULL;
}

void CSMain::OnEntry(void)
{ 
	m_pDXManager = new CDXManager();
	m_pDXPainter = new CDXBasicPainter(m_pDXManager);
	m_FX = new CFX(m_pDXManager);
	

	/* Load init parameters */
	ReadInitFile();
	printParams();

	/* Print where I am */
	printf("[HCM] %s:OnEntry\n", GetClassString());
	printf("Init DirectX 11\n");

	if (!m_pDXManager->Initialize(m_hWnd, CDXManager::EnumAndChooseAdapter(NULL,m_Params.gpu)))
	{
		m_bInitCorrect = FALSE;
		MessageBox(m_hWnd, L"No se pudo inicializar DirectX11", L"Error", MB_ICONERROR);
	}

	if (!m_pDXPainter->Initialize())
	{
		m_bInitCorrect = FALSE;
		MessageBox(m_hWnd, L"No se pudo inicializar shaders", L"Error", MB_ICONERROR);
	}

	if (!m_FX->Initialize())
	{
		MessageBox(NULL,
			L"No se ha podido inicializar FX",
			L"Error fatal", MB_ICONERROR);
		return;
	}

	printf("Sound init \n");
	CSndFactory* pFactory = new CSndFactory();
	m_pSndManager =  (CSndManager*) pFactory->CreateObject(L"CSndManager");

	if (!m_pSndManager->InitSoundEngine(m_hWnd))
	{
		MessageBox(NULL,
			L"No se ha podido inicializar Sound",
			L"Error fatal", MB_ICONERROR);
		return;
	}

	printf("Input Init \n");
	m_pInputManager = new CInputManager();
	if (!m_pInputManager->InitializeDirectInputSession(m_hInstance))
	{
		MessageBox(NULL,
			L"No se ha podido inicializar Input Manager",
			L"Error fatal", MB_ICONERROR);
		return;
	}
	if (!m_pInputManager->ConfigureDevices(m_hWnd))
	{
		MessageBox(NULL,
			L"Unable to aquire input devices",
			L"Error fatal", MB_ICONERROR);
		return;
	}
}

unsigned long CSMain::OnEvent(CEventBase * pEvent)
{
	if (APP_LOOP == pEvent->m_ulEventType)
	{
		if (m_pSndManager)
			m_pSndManager->RemoveAllSndFxStopped();

		for (long iSource = 0; iSource < m_pInputManager->GetDeviceCount(); iSource++)
		{
			DIJOYSTATE2 js2;
			if (m_pInputManager->ReadState(js2, iSource))
			{
				CInputEvent *pInput = new CInputEvent(iSource, 0, js2);
				m_pSMOwner->PostEvent(pInput);
			}
		}

	}
	else if (EVENT_WIN32 == pEvent->m_ulEventType)
	{
		CEventWin32* pWin32 = (CEventWin32*)pEvent;
		switch (pWin32->m_msg)
		{
		case WM_SIZE: 
		{
			m_pDXManager->Resize(LOWORD(pWin32->m_lParam), HIWORD(pWin32->m_lParam));
		}
		break;
		case WM_CLOSE: 
			m_pSMOwner->Transition(CLSID_CStateNull);
			return 0;
		default:
			break;
		}
		
	}
	return __super::OnEvent(pEvent);
}

void CSMain::OnExit(void)
{
	printf("[HCM] %s:OnExit\n", GetClassString());
	m_pDXPainter->Uninitialize();
	m_pDXManager->Uninitialize();
	m_pSndManager->UnitializeSoundEngine();
	m_FX->Uninitialize();
	m_pInputManager->FinalizeDirectInputSession();
	SAFE_DELETE(m_pDXPainter);
	SAFE_DELETE(m_pDXManager);
	SAFE_DELETE(m_FX);
	
	CSndFactory Factory;
	Factory.DestroyObject(m_pSndManager);

	SAFE_DELETE(m_pInputManager);

}


#define INIT_FILE_TAG_USER			0x001
#define INIT_FILE_TAG_SCENE			0x002
#define INIT_FILE_TAG_FX			0x004
#define INIT_FILE_TAG_3D_EFFECTS	0x008
#define INIT_FILE_CUT_METHOD		0x010
#define INIT_FILE_COLLISION_SCENE	0x020
#define INIT_FILE_COLLISION_OBJECT	0x040
#define INIT_FILE_PLANE_CUT			0x080
#define INIT_FILE_GPU				0x100

HRESULT CSMain::WriteAttributes(IXmlReader* pReader, long tag, wchar_t* localTag)
{
	const WCHAR* pwszPrefix;
	const WCHAR* pwszLocalName;
	const WCHAR* pwszValue;
	HRESULT hr = pReader->MoveToFirstAttribute();
	long counter = 0;
	static long pointPos = 0;
	float x, y, z;

	if (S_FALSE == hr)
		return hr;
	if (S_OK != hr)
	{
		wprintf(L"Error moving to first attribute, error is %08.8lx", hr);
		return hr;
	}
	else
	{
		while (TRUE)
		{
			if (!pReader->IsDefault())
			{
				UINT cwchPrefix;
				if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
				{
					wprintf(L"Error getting prefix, error is %08.8lx", hr);
					return hr;
				}
				if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
				{
					wprintf(L"Error getting local name, error is %08.8lx", hr);
					return hr;
				}
				if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
				{
					wprintf(L"Error getting value, error is %08.8lx", hr);
					return hr;
				}

				if (tag == INIT_FILE_TAG_3D_EFFECTS)
				{
					unsigned long bitToEnable = 0;

					if (wcscmp(localTag, L"EMISSIVE") == 0)
						bitToEnable = MAPPING_EMISSIVE;
					else if (wcscmp(localTag, L"DIFFUSE") == 0)
						bitToEnable = MAPPING_DIFFUSE;
					else if (wcscmp(localTag, L"NORMALMAP") == 0)
						bitToEnable = MAPPING_NORMAL_TRUE;
					else if (wcscmp(localTag, L"SHADOW") == 0)
						bitToEnable = MAPPING_SHADOW;
					else if (wcscmp(localTag, L"NORMAL") == 0)
						bitToEnable = MAPPING_NORMAL;
					else if (wcscmp(localTag, L"ENVIROMENTFAST") == 0)
						bitToEnable = MAPPING_ENVIROMENTAL_FAST;
					else if (wcscmp(localTag, L"ENVIROMENTSKY") == 0)
						bitToEnable = MAPPING_ENVIROMENTAL_SKY;
					else if (wcscmp(localTag, L"FOG") == 0)
						bitToEnable = FOG_ENABLE;


					if (wcscmp(pwszLocalName, L"value") == 0)
					{
						if (wcscmp(pwszValue, L"on") == 0)
							m_Params.PainterFlags |= bitToEnable;
					}
					else
						printf("Something is wrong with initfile \n");
					
				}
				else if (tag == INIT_FILE_PLANE_CUT)
				{
					
					if (counter == 0 && wcscmp(pwszLocalName, L"x") == 0)
						x = wcstof(pwszValue, (wchar_t**)&pwszValue);
					if (counter == 1 && wcscmp(pwszLocalName, L"y") == 0)
						y = wcstof(pwszValue, (wchar_t**)&pwszValue);
					if (counter == 2 && wcscmp(pwszLocalName, L"z") == 0)
						z = wcstof(pwszValue, (wchar_t**)&pwszValue);
					
					counter++;
				}

			}

			if (S_OK != pReader->MoveToNextAttribute())
				break;
		}
	}
	if (tag == INIT_FILE_PLANE_CUT)
	{
		m_Params.PlaneCut[pointPos] = { x,y,z,1 };
		pointPos++;
	}
		

	return hr;
}

void CSMain::printParams()
{
	printf("------------ Init parameters ----------------\n\n");

	wprintf(L"USER: %s \n",m_Params.user);
	wprintf(L"GPU: %s \n", m_Params.gpu);
	wprintf(L"SCENE: %s \n", m_Params.scene);

	switch (m_Params.FXEffect)
	{
	case FX_BRIGHT_PASS:
		printf("FXEffect: FX_BRIGHT_PASS \n");
		break;
	case FX_DIRECTIONAL_BLUR:
		printf("FXEffect: FX_DIRECTIONAL_BLUR \n");
		break;
	case FX_EDGE_DETECT:
		printf("FXEffect: FX_EDGE_DETECT \n");
		break;
	case FX_GAUSS_HORIZONTAL_BLUR:
		printf("FXEffect: FX_GAUSS_HORIZONTAL_BLUR \n");
		break;
	case FX_GAUSS_VERTICAL_BLUR:
		printf("FXEffect: FX_GAUSS_VERTICAL_BLUR \n");
		break;
	case FX_MERGED:
		printf("FXEffect: FX_MERGED \n");
		break;
	case FX_RADIAN_BLUR:
		printf("FXEffect: FX_RADIAN_BLUR \n");
		break;
	case FX_SKY:
		printf("FXEffect: FX_SKY \n");
		break;
	default:
		break;
	}
	printf("PainterFlags: \n\n");

	for (unsigned long i = 1; m_Params.PainterFlags >= i ; i<<=1)
	{
		if((m_Params.PainterFlags & i) == MAPPING_DIFFUSE)
			printf("\tMAPPING_DIFFUSE\n");
		else if ((m_Params.PainterFlags & i) == MAPPING_EMISSIVE)
			printf("\tMAPPING_EMISSIVE\n");
		else if ((m_Params.PainterFlags & i) == MAPPING_NORMAL_TRUE)
			printf("\tMAPPING_NORMAL_TRUE\n");
		else if ((m_Params.PainterFlags & i) == MAPPING_NORMAL)
			printf("\tMAPPING_NORMAL\n");
		else if ((m_Params.PainterFlags & i) == MAPPING_SHADOW)
			printf("\tMAPPING_SHADOW\n");
		else if ((m_Params.PainterFlags & i) == MAPPING_ENVIROMENTAL_SKY)
			printf("\tMAPPING_ENVIROMENTAL_SKY\n");
		else if ((m_Params.PainterFlags & i) == FOG_ENABLE)
			printf("\tFOG_ENABLE\n");

	}
	printf("\n");
	printf("CUT METHOD: %s \n", m_Params.MethodCutting & CUTTING_FEM ? "FEM" : m_Params.MethodCutting & CUTTING_FEM ? "XFEM" : "NONE");
	printf("COLLISION METHOD SCENE: %s \n", m_Params.MethodCollisionScene & COLLISION_BVH ? "BVH" : m_Params.MethodCollisionScene & COLLISION_OCTREE ? "OCTREE" : "NONE");
	printf("COLLISION METHOD OBJECT: %s \n", m_Params.MethodCollisionPerObject & COLLISION_BVH ? "BVH" : m_Params.MethodCollisionPerObject & COLLISION_BVH ? "OCTREE" : "NONE");
	printf("PLANE CUT:\n");

	for (unsigned long i = 0; i < 4;i++)
		printf("\tPoint[%i] = { %f, %f, %f } \n", i, m_Params.PlaneCut[i].x, m_Params.PlaneCut[i].y, m_Params.PlaneCut[i].z);
	printf("\n---------------------------------------------\n\n");
}

wchar_t* removeWhitespace(wchar_t * buf)
{
	wchar_t* end = NULL;
	while (*buf == L' ')
		buf++;

	end = buf;
	while (*end != L'\0' && *end != L' ')
		end++;

	*end = L'\0';
	return buf;
}

void CSMain::ReadInitFile()
{
	//Open read-only input stream
	IStream *pFileStream = NULL;
	HRESULT hr;
	IXmlReader *pReader = NULL;
	XmlNodeType nodeType;
	const wchar_t* pwszPrefix = NULL;
	const wchar_t* pwszLocalName;
	const wchar_t* pwszValue;
	UINT cwchPrefix;
	long depth = 0;
	long tag = 0;

	if (FAILED(hr = SHCreateStreamOnFile(L"..\\Assets\\initfile.xml", STGM_DIRECT, &pFileStream)))
	{
		wprintf(L"Error creating file reader, error is %08.8lx", hr);
	}

	
	if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, NULL)))
	{
		wprintf(L"Error creating xml reader, error is %08.8lx", hr);
	}

	if (FAILED(hr = pReader->SetInput(pFileStream)))
	{
		wprintf(L"Error setting input for reader, error is %08.8lx", hr);

	}
	//read until there are no more nodes
	while (S_OK == (hr = pReader->Read(&nodeType)))
	{
		switch (nodeType)
		{
		case XmlNodeType_XmlDeclaration:
			wprintf(L"XmlDeclaration\n");
			if (FAILED(hr = WriteAttributes(pReader,tag,(wchar_t*) pwszPrefix)))
			{
				wprintf(L"Error writing attributes, error is %08.8lx", hr);
			}
			break;
		case XmlNodeType_Element:
			
			if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
			{
				wprintf(L"Error getting prefix, error is %08.8lx", hr);
			}
			if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
			{
				wprintf(L"Error getting local name, error is %08.8lx", hr);
			}
			depth++;
			if (depth == 1 &&  wcscmp(pwszLocalName, L"CollisionEngine") != 0)
				wprintf(L"Wrong file config: Waiting \"CollisionEngine\" Tag: %s ", pwszLocalName);

			if (depth == 2)
			{
				if (wcscmp(pwszLocalName, L"User") == 0)
					tag = INIT_FILE_TAG_USER;
				else if (wcscmp(pwszLocalName, L"Scene") == 0)
					tag = INIT_FILE_TAG_SCENE;
				else if (wcscmp(pwszLocalName, L"Effect") == 0)
					tag = INIT_FILE_TAG_FX;
				else if (wcscmp(pwszLocalName, L"Effects3d") == 0)
					tag = INIT_FILE_TAG_3D_EFFECTS;
				else if (wcscmp(pwszLocalName, L"MethodCutting") == 0)
					tag = INIT_FILE_CUT_METHOD;
				else if (wcscmp(pwszLocalName, L"MethodCollisionScene") == 0)
					tag = INIT_FILE_COLLISION_SCENE;
				else if (wcscmp(pwszLocalName, L"MethodCollisionPerObject") == 0)
					tag = INIT_FILE_COLLISION_OBJECT;
				else if (wcscmp(pwszLocalName, L"PlaneCut") == 0)
					tag = INIT_FILE_PLANE_CUT;
				else if (wcscmp(pwszLocalName, L"GPU") == 0)
					tag = INIT_FILE_GPU;
				else
					tag = 0;
			}
			if (depth == 3 && (tag == INIT_FILE_TAG_3D_EFFECTS || tag == INIT_FILE_PLANE_CUT))
			{
				if (FAILED(hr = WriteAttributes(pReader, tag, (wchar_t*)pwszLocalName)))
					wprintf(L"Error writing attributes, error is %08.8lx", hr);
				depth--;
			}
				

			/*if (pReader->IsEmptyElement())*/
				

			break;
		case XmlNodeType_EndElement:
			depth--;
			
			if (depth < 0)
			{
				printf("Depth is %i, something is wron with initfile.xml\n", depth);
			}
			if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
			{
				wprintf(L"Error getting prefix, error is %08.8lx", hr);
			}
			if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
			{
				wprintf(L"Error getting local name, error is %08.8lx", hr);
			}
			break;
			
		case XmlNodeType_Text:
		//case XmlNodeType_Whitespace:
			if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
				wprintf(L"Error getting value, error is %08.8lx", hr);

			
			switch (tag)
			{
			case INIT_FILE_TAG_USER:
				pwszValue = removeWhitespace((wchar_t*)pwszValue);
				wcscpy(this->m_Params.user, pwszValue);
				break;
			case INIT_FILE_GPU:
				pwszValue = removeWhitespace((wchar_t*)pwszValue);
				wcscpy(this->m_Params.gpu, pwszValue);
				break;
			case INIT_FILE_TAG_SCENE:
				pwszValue = removeWhitespace((wchar_t*)pwszValue);
				wcscpy(this->m_Params.scene, pwszValue);
				break;
			case INIT_FILE_TAG_FX:
				this->m_Params.FXEffect =  wcstol(pwszValue,(wchar_t**) &pwszValue,10);
				break;
			case INIT_FILE_TAG_3D_EFFECTS:
				break;
			case INIT_FILE_CUT_METHOD:
				pwszValue = removeWhitespace((wchar_t*)pwszValue);
				if (wcsncmp(pwszValue, L"FEM", sizeof(L"FEM")) == 0)
					m_Params.MethodCutting = CUTTING_FEM;
				else if (wcsncmp(pwszValue, L"XFEM", sizeof(L"XFEM")) == 0)
					m_Params.MethodCutting = CUTTING_XFEM;
				else /* Default */
					m_Params.MethodCutting = CUTTING_FEM;
				break;
			case INIT_FILE_COLLISION_SCENE:
				pwszValue = removeWhitespace((wchar_t*)pwszValue);
				if (wcsncmp(pwszValue, L"OCTREE", sizeof(L"OCTREE")) == 0)
					m_Params.MethodCollisionScene = COLLISION_OCTREE;
				else if (wcsncmp(pwszValue, L"BVH", sizeof(L"BVH")) == 0)
					m_Params.MethodCollisionScene = COLLISION_BVH;
				else /* Default */
					m_Params.MethodCollisionScene = COLLISION_OCTREE;
				break;
			case INIT_FILE_COLLISION_OBJECT:
				pwszValue = removeWhitespace((wchar_t* )pwszValue);
				if (wcsncmp(pwszValue, L"OCTREE", sizeof(L"OCTREE")) == 0)
					m_Params.MethodCollisionPerObject = COLLISION_OCTREE;
				else if (wcsncmp(pwszValue, L"BVH", sizeof(L"BVH")) == 0)
					m_Params.MethodCollisionPerObject = COLLISION_BVH;
				else /* Default */
					m_Params.MethodCollisionPerObject = COLLISION_OCTREE;
				break;
			case INIT_FILE_PLANE_CUT:
				break;
			}
			break;
		case XmlNodeType_CDATA:
			if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
			{
				wprintf(L"Error getting value, error is %08.8lx", hr);

			}
			wprintf(L"CDATA: %s\n", pwszValue);
			break;
		case XmlNodeType_ProcessingInstruction:
			if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
			{
				wprintf(L"Error getting name, error is %08.8lx", hr);
			}
			if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
			{
				wprintf(L"Error getting value, error is %08.8lx", hr);
			}
			wprintf(L"Processing Instruction name:%s value:%s\n", pwszLocalName, pwszValue);
			break;
		case XmlNodeType_Comment:
			if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
			{
				wprintf(L"Error getting value, error is %08.8lx", hr);
			}
			wprintf(L"Comment: %s\n", pwszValue);
			break;
		case XmlNodeType_DocumentType:
			wprintf(L"DOCTYPE is not printed\n");
			break;
		}
	}
}
