#include "stdafx.h"
#include "VMesh.h"
#include <fstream>
#include <string>
#include <iostream>

#define GET_KEY(id1, id2, key) \
{\
	union\
	{\
	struct {\
		long _id1;\
		long _id2;\
	};\
	long long _key;\
	};\
	\
	_id1 = id1;\
	_id2 = id2;\
	\
	if (_id1 > _id2)\
	{\
		long _aux = _id1;\
		_id1 = _id2;\
		_id2 = _aux;\
	}\
	\
	key = _key;\
}
ID3D11ComputeShader*     CVMesh::s_pCSInitForce = NULL;
ID3D11ComputeShader*     CVMesh::s_pCSVolumePreservation = NULL;
ID3D11ComputeShader*     CVMesh::s_pCSComputeForces = NULL;
ID3D11ComputeShader*     CVMesh::s_pCSApplyForces = NULL;
ID3D11ComputeShader*     CVMesh::s_pCSComputeNormals = NULL;

ID3D11Buffer*			 CVMesh::s_pCBMassSpring = NULL;

void CVMesh::CompileCSShaders(CDXManager * pManager)
{
	s_pCSInitForce = pManager->CompileComputeShader(L"..\\Shaders\\MassSpring.hlsl", "InitForce");
	s_pCSVolumePreservation = pManager->CompileComputeShader(L"..\\Shaders\\MassSpring.hlsl", "VolumePreservation");
	s_pCSComputeForces = pManager->CompileComputeShader(L"..\\Shaders\\MassSpring.hlsl", "ComputeForces");
	s_pCSApplyForces = pManager->CompileComputeShader(L"..\\Shaders\\MassSpring.hlsl", "ApplyForces");
	s_pCSComputeNormals = pManager->CompileComputeShader(L"..\\Shaders\\MassSpring.hlsl", "ComputeNormals");

	// Create constant buffer MassSpring.hlsl
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.ByteWidth = 16 * ((sizeof(PARAMS_MASS_SPRING_CB) + 15) / 16);
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	pManager->GetDevice()->CreateBuffer(&dbd, 0, &s_pCBMassSpring);
}

void CVMesh::CreateTetraindexAndMassSpringBuffers(CDXManager * m_pManager)
{
	SAFE_RELEASE(m_pUAVMassSpringBuffer);
	SAFE_RELEASE(m_pUAVTetraIndices);
	SAFE_RELEASE(m_pSRVMassSpringBuffer);
	SAFE_RELEASE(m_pSRVTetraIndices);
	SAFE_RELEASE(m_pMassSpringBuffer);
	SAFE_RELEASE(m_pTetraIndices);


	m_MassSpringGPU.resize(m_MassSpring.size());

	for (unsigned long i = 0; i < m_MassSpringGPU.size(); i++)
	{
		m_MassSpringGPU[i].fuerza = m_MassSpring[i].Fuerza;
		m_MassSpringGPU[i].masa = m_MassSpring[i].Masa;
		m_MassSpringGPU[i].velocity = m_MassSpring[i].Velocity;
		m_MassSpringGPU[i].numVecinos = 0;

		for (set<long>::iterator it = m_MassSpring[i].vecinos.begin(); it != m_MassSpring[i].vecinos.end(); ++it)
		{
			long vecino = *it;
			// Agregar distancia 
			long long key;
			GET_KEY(i, vecino, key);

			m_MassSpringGPU[i].vecinos[m_MassSpringGPU[i].numVecinos].idVecino = vecino;
			m_MassSpringGPU[i].vecinos[m_MassSpringGPU[i].numVecinos].distancia = m_MassSpring[i].distancia[key];

			m_MassSpringGPU[i].numVecinos++;
		}
	}

	m_pMassSpringBuffer = m_pManager->CreateLoadBuffer(&m_MassSpringGPU[0], sizeof(MassSpringGPU), m_MassSpringGPU.size());
	m_pTetraIndices = m_pManager->CreateLoadBuffer(&m_IndicesTetrahedros[0], sizeof(Tetrahedron), m_IndicesTetrahedros.size());

	// Create UAV 
	m_pManager->GetDevice()->CreateUnorderedAccessView(m_pMassSpringBuffer, NULL, &m_pUAVMassSpringBuffer);
	m_pManager->GetDevice()->CreateUnorderedAccessView(m_pTetraIndices, NULL, &m_pUAVTetraIndices);

	// Create SRV 
	m_pManager->GetDevice()->CreateShaderResourceView(m_pMassSpringBuffer, NULL, &m_pSRVMassSpringBuffer);
	m_pManager->GetDevice()->CreateShaderResourceView(m_pTetraIndices, NULL, &m_pSRVTetraIndices);
}

CVMesh::CVMesh()
{
	m_pMassSpringBuffer = NULL;
	m_pTetraIndices = NULL;

	// UAV For vertex buffer  and primbuffer
	m_pUAVMassSpringBuffer = NULL;
	m_pUAVTetraIndices = NULL;

	// SRV For vertex buffer and index buffer
	m_pSRVMassSpringBuffer = NULL;
	m_pSRVTetraIndices = NULL;
}


CVMesh::~CVMesh()
{
}

void CVMesh::LoadMSHFile(char * filename)
{
	bool node = 0;
	bool element = 0;
	long long int numberNodes = 0;
	long long int numberIndex = 0;
	ifstream tourus("..\\Assets\\cube.msh");
	string line;
	if (tourus.is_open())
	{
		while (getline(tourus, line))
		{
			if (line == "$Nodes")
			{
				tourus >> numberNodes;
				m_Vertices.resize(numberNodes+1);
				for (int i = 1; i < numberNodes+1; i++)
				{
					int indice;
					tourus >> indice;
					tourus >> m_Vertices[i].Position.x;
					tourus >> m_Vertices[i].Position.y;
					tourus >> m_Vertices[i].Position.z;
					
					//m_Vertices[i].Position = m_Vertices[i].Position * 2.f;
					m_Vertices[i].Position.w = 1;
				}
			}

			else if (line == "$Elements")
			{
				tourus >> numberIndex;
				m_IndicesTetrahedros.resize(numberIndex);
				for (int i = 0; i < numberIndex; i++)
				{
					int v0;
					int v1;
					int v2;
					int v3;
					int v4;
					int offset = i;
					tourus >> v0;
					tourus >> v1;
					tourus >> v2;
					tourus >> v3;
					tourus >> v4;
					tourus >> m_IndicesTetrahedros[offset].indexes[0];
					tourus >> m_IndicesTetrahedros[offset].indexes[1];
					tourus >> m_IndicesTetrahedros[offset].indexes[2];
					tourus >> m_IndicesTetrahedros[offset].indexes[3];

				}

			}

		}
	}
	tourus.close();

	m_IndicesDibujarTetrahedros.resize(m_IndicesTetrahedros.size()*3*4);

	for (unsigned long int i = 0, j = 0; i < m_IndicesTetrahedros.size(); i++, j += 12)
	{
		int v0, v1, v2, v3;
		v0 = m_IndicesTetrahedros[i].indexes[0];
		v1 = m_IndicesTetrahedros[i].indexes[1];
		v2 = m_IndicesTetrahedros[i].indexes[2];
		v3 = m_IndicesTetrahedros[i].indexes[3];

		m_IndicesDibujarTetrahedros[j] = v0;
		m_IndicesDibujarTetrahedros[j+1] = v2;
		m_IndicesDibujarTetrahedros[j + 2] = v1;

		m_IndicesDibujarTetrahedros[j + 3] = v2;
		m_IndicesDibujarTetrahedros[j + 4] = v3;
		m_IndicesDibujarTetrahedros[j + 5] = v1;

		m_IndicesDibujarTetrahedros[j + 6] = v3;
		m_IndicesDibujarTetrahedros[j + 7] = v0;
		m_IndicesDibujarTetrahedros[j + 8] = v1;

		m_IndicesDibujarTetrahedros[j + 9] = v0;
		m_IndicesDibujarTetrahedros[j + 10] = v3;
		m_IndicesDibujarTetrahedros[j + 11] = v2;
	}


	for (unsigned long j = 0; j < m_Vertices.size(); j++)
	{
		VECTOR4D TexCoord = { 0,0,0,0 };
		TexCoord.x = m_Vertices[j].Position.x;
		TexCoord.y = m_Vertices[j].Position.z;
		TexCoord.z = m_Vertices[j].Position.y;
		TexCoord = Normalize(TexCoord);
		TexCoord.x = TexCoord.x * 0.5 + 0.5;
		TexCoord.y = TexCoord.y * 0.5 + 0.5;

		m_Vertices[j].TexCoord = TexCoord;
	}
	//m_Indices = m_IndicesDibujarTetrahedros;

	CreateSurfaceMesh();
	BuildTangentSpaceFromTexCoordsIndexed(true);

}

#define MASA (4)
#define INITIALIZE_SPEED {0,0,0,0}
#define K (10000)
#define Kv (10000)
#define DELTA_T (0.01)

void CVMesh::CreateNeighbors()
{
	for (unsigned long i = 0; i < m_IndicesTetrahedros.size(); i ++)
	{
		// 1. Sacar los 4 vertices
		// 2. Ver si ya existen 
		//	2.1 Si no existe agregarlo 
		//  2.2 Si existe no hacer nada
		// 3. Crear lista vecinos
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				if (j != k)
				{

					// Agregar a la lista
					m_MassSpring[m_IndicesTetrahedros[i].indexes[j]].vecinos.insert(m_IndicesTetrahedros[i].indexes[k]);
					// Agregar distancia 
					long long key;
					GET_KEY(m_IndicesTetrahedros[i].indexes[j], m_IndicesTetrahedros[i].indexes[k], key);

					m_MassSpring[m_IndicesTetrahedros[i].indexes[j]].distancia[key] =
						fabs(
							Magnity(m_Vertices[m_IndicesTetrahedros[i].indexes[j]].Position -
								    m_Vertices[m_IndicesTetrahedros[i].indexes[k]].Position
							)
						);
				}
			}
		}

		/* Compute C */
		unsigned long i1, i2, i3, i4;

		i1 = m_IndicesTetrahedros[i].indexes[0];
		i2 = m_IndicesTetrahedros[i].indexes[1];
		i3 = m_IndicesTetrahedros[i].indexes[2];
		i4 = m_IndicesTetrahedros[i].indexes[3];

		VECTOR4D x1, x2, x3, x4;
		x1 = m_Vertices[i1].Position;
		x2 = m_Vertices[i2].Position;
		x3 = m_Vertices[i3].Position;
		x4 = m_Vertices[i4].Position;

		VECTOR4D e1, e2, e3;
		e1 = x2 - x1;
		e2 = x3 - x1;
		e3 = x4 - x1;

		float V = (1 / 6.f) * Dot(e1, Cross3(e2, e3));
		/* Hacer el Constrain una solo vez */
		VECTOR4D C = ((1 / 6.f) * e1 * (Cross3(e2, e3))) - V;
		C.w = 1;
		m_IndicesTetrahedros[i].C = C;
	}
}

void CVMesh::InitializaMassSpring()
{
	// initialization
	//(1) forall particles i
	//(2) initialize xi
	//	, vi and mi
	//	(3) endfor
	m_MassSpring.resize(m_Vertices.size());

	for (unsigned i = 0; i < m_MassSpring.size(); i++)
	{
		m_MassSpring[i].Masa = MASA;
		m_MassSpring[i].Velocity = INITIALIZE_SPEED;
	}

	CreateNeighbors();

	/* Create buffer for collision forces */
	m_CollisionForces.resize(m_MassSpring.size());

	memset(&m_CollisionForces[0], 0, m_CollisionForces.size()*sizeof(VECTOR4D));
}

void CVMesh::ApplyForces(VECTOR4D Gravity, VECTOR4D ExternalForce)
{
	for (unsigned long i = 0; i < m_MassSpring.size(); i++)
	{
		m_MassSpring[i].Fuerza = { 0,0,0,0 };
	}

	/* Preservacion de volumn */
	
	for (unsigned long i = 0; i < m_IndicesTetrahedros.size(); i++)
	{
		unsigned long i1, i2, i3, i4;

		i1 = m_IndicesTetrahedros[i].indexes[0];
		i2 = m_IndicesTetrahedros[i].indexes[1];
		i3 = m_IndicesTetrahedros[i].indexes[2];
		i4 = m_IndicesTetrahedros[i].indexes[3];

		VECTOR4D x1, x2, x3, x4;
		x1 = m_Vertices[i1].Position;
		x2 = m_Vertices[i2].Position;
		x3 = m_Vertices[i3].Position;
		x4 = m_Vertices[i4].Position;

		VECTOR4D e1, e2, e3;
		e1 = x2 - x1;
		e2 = x3 - x1;
		e3 = x4 - x1;

		float V = (1/6.f) * Dot(e1 ,Cross3(e2, e3));
		/* Hacer el Constrain una solo vez */
		VECTOR4D C = ( (1 / 6.f) * e1 * (Cross3(e2, e3)) ) - V;

		VECTOR4D F1, F2, F3, F4;

		F1 = Kv*C*(Cross3(e2 - e1, e3 - e1));
		F2 = Kv*C*Cross3(e3, e2);
		F3 = Kv*C*Cross3(e1, e3);
		F4 = Kv*C*Cross3(e2, e1);

		m_MassSpring[i1].Fuerza = m_MassSpring[i1].Fuerza + F1;
		m_MassSpring[i2].Fuerza = m_MassSpring[i2].Fuerza + F2;
		m_MassSpring[i3].Fuerza = m_MassSpring[i3].Fuerza + F3;
		m_MassSpring[i4].Fuerza = m_MassSpring[i4].Fuerza + F4;

	}
	
	for (unsigned long i = 0; i < m_Vertices.size(); i++)
	{
		VECTOR4D F = {0,0,0,0};

		for (auto vecino : m_MassSpring[i].vecinos) 
		{
			VECTOR4D V = m_Vertices[vecino].Position - m_Vertices[i].Position;

			float M = fabs(Magnity(V));
			
			long long Key;
			GET_KEY(i, vecino, Key);

			float L = m_MassSpring[i].distancia[Key];
			F = F + ((K * (M - L)) * (Normalize(V))) ;

		}
		/* Agregar preservacion de volumen */
		m_MassSpring[i].Fuerza =  m_MassSpring[i].Fuerza +  F  + ExternalForce + Gravity;

	}
//#define POS 128
//
//	printf("Vertice[1] Position = [%f][%f][%f] Fuerza = [%f][%f][%f] Velocidad = [%f][%f][%f]\n", 
//		m_Vertices[POS].Position.x,
//		m_Vertices[POS].Position.y,
//		m_Vertices[POS].Position.z,
//		m_MassSpring[POS].Fuerza.x,
//		m_MassSpring[POS].Fuerza.y,
//		m_MassSpring[POS].Fuerza.z,
//		m_MassSpring[POS].Velocity.x,
//		m_MassSpring[POS].Velocity.y,
//		m_MassSpring[POS].Velocity.z);

	for (unsigned long i = 0; i < m_Vertices.size(); i++)
	{
		if (i == 1)
			continue;

		m_MassSpring[i].Velocity = m_MassSpring[i].Velocity + (DELTA_T * (m_MassSpring[i].Fuerza / m_MassSpring[i].Masa));
		m_Vertices[i].Position = m_Vertices[i].Position + (DELTA_T * m_MassSpring[i].Velocity);
		m_Vertices[i].Position.w = 1;

	}
	
	//CreateMeshCollisionFromVMesh(this);
}

void CVMesh::CSApplyForces(CDXManager * pManager, VECTOR4D Gravity, VECTOR4D ExternalForce)
{
	auto pCtx = pManager->GetContext();
	/********* Init Force ****/
	/* Create collision buffers */
	ID3D11Buffer* pCollisionBuffer = pManager->CreateLoadBuffer(&m_CollisionForces[0], sizeof(VECTOR4D), m_CollisionForces.size());
	ID3D11ShaderResourceView* pSRVCollisionForces;

	pManager->GetDevice()->CreateShaderResourceView(pCollisionBuffer, NULL, &pSRVCollisionForces);

	/* Set Compute Shader */
	pCtx->CSSetShader(CVMesh::s_pCSInitForce, 0, 0);

	/* Update constant buffer */
	m_CB_MASS_SPRING.Delta_t = DELTA_T;
	m_CB_MASS_SPRING.Gravity = Gravity;
	m_CB_MASS_SPRING.Ki = K;

	pManager->UpdateConstantBuffer(s_pCBMassSpring, &m_CB_MASS_SPRING, sizeof(CVMesh::PARAMS_MASS_SPRING_CB));

	/* Set Constant buffer */
	pCtx->CSSetConstantBuffers(0, 1, &s_pCBMassSpring);

	/* Set UAV */
	pCtx->CSSetUnorderedAccessViews(0, 1, &(this->m_pUAVMassSpringBuffer), NULL);
	pCtx->CSSetUnorderedAccessViews(1, 1, &(this->m_pUAVVertexBuffer), NULL);

	/* Set SRV */
	pCtx->CSSetShaderResources(0, 1, &(this->m_pSRVTetraIndices));
	pCtx->CSSetShaderResources(1, 1, &(pSRVCollisionForces));
	pCtx->CSSetShaderResources(2, 1, &(this->m_pSRVVisualNeighbors));
	pCtx->CSSetShaderResources(3, 1, &(this->m_pSRVIndexBuffer));

	pCtx->Dispatch((m_Vertices.size() + 511) /512, 1, 1);

	/********* Preservetion Volumn ******/
	/* Set Compute Shader */
	pCtx->CSSetShader(CVMesh::s_pCSVolumePreservation, 0, 0);
	pCtx->Dispatch(((m_IndicesTetrahedros.size()) + 511) / 512, 1, 1);

	/********* Compute Forces ******/
	/* Set Compute Shader */
	pCtx->CSSetShader(CVMesh::s_pCSComputeForces, 0, 0);
	pCtx->Dispatch((m_Vertices.size() + 511) / 512, 1, 1);

	/********* Apply forces ******/
	/* Set Compute Shader */
	pCtx->CSSetShader(CVMesh::s_pCSApplyForces, 0, 0);
	pCtx->Dispatch((m_Vertices.size() + 511) / 512, 1, 1);

	/********* Compute normals ******/
	/* Set Compute Shader */
	pCtx->CSSetShader(CVMesh::s_pCSComputeNormals, 0, 0);
	pCtx->Dispatch((m_Vertices.size() + 511) / 512, 1, 1);

	/* Save result */
	pManager->CreateStoreBuffer(this->m_pVertexBuffer, sizeof(CDXPainter::VERTEX), this->m_Vertices.size(), &m_Vertices[0]);
	/* Save Mass spting */
	pManager->CreateStoreBuffer(this->m_pMassSpringBuffer, sizeof(MassSpringGPU), this->m_MassSpringGPU.size(), &m_MassSpringGPU[0]);

	SAFE_RELEASE(pSRVCollisionForces);
	SAFE_RELEASE(pCollisionBuffer);
}

struct TriangleSurface
{
	int id[3];
	int realTriangle[3];
	int counter;

};

#define VMESH_IS_SAME_ID( id1, id2 ) \
	(id1[0] == id2[0] && id1[1] == id2[1] && id1[2] == id2[2])

void CVMesh::CreateSurfaceMesh()
{
	vector<TriangleSurface> counterFaces;

	for (unsigned long i = 0; i < this->m_IndicesDibujarTetrahedros.size(); i += 3)
	{
		TriangleSurface newFace;
		newFace.realTriangle[0] = newFace.id[0] = this->m_IndicesDibujarTetrahedros[i];
		newFace.realTriangle[1] = newFace.id[1] = this->m_IndicesDibujarTetrahedros[i + 1];
		newFace.realTriangle[2] = newFace.id[2] = this->m_IndicesDibujarTetrahedros[i + 2];

		// ordenar hacer una llave 
		for (unsigned int j = 0; j < 3; j++)
		{
			for (unsigned int k = 0; k < 3; k++)
			{
				if (newFace.id[j] > newFace.id[k])
				{
					int aux = newFace.id[j];
					newFace.id[j] = newFace.id[k];
					newFace.id[k] = aux;
				}
			}
		}
		bool alreadyExist = false;
		for (unsigned int j = 0; j < counterFaces.size(); j++)
		{
			if (VMESH_IS_SAME_ID(newFace.id, counterFaces[j].id))
			{
				alreadyExist = true;
				counterFaces[j].counter++;
				break;
			}
		}

		if (!alreadyExist)
		{
			newFace.counter = 0;
			counterFaces.push_back(newFace);
		}
	}
	m_Indices.clear();

	for (unsigned long i = 0; i < counterFaces.size(); i++)
	{
		if (counterFaces[i].counter == 0)
		{
			m_Indices.push_back(counterFaces[i].realTriangle[0]);
			m_Indices.push_back(counterFaces[i].realTriangle[1]);
			m_Indices.push_back(counterFaces[i].realTriangle[2]);
		}
	}

	//m_Vertices = vMesh.m_Vertices;
}
