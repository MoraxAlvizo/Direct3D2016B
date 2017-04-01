#include "stdafx.h"
#include "Mesh.h"
#include "DXPainter.h"
#include "../Cut/VMesh.h"
#include "../Collisions/BVH.h"

ID3D11ComputeShader*     CMesh::s_pCSApplyTransform = NULL;
ID3D11Buffer*			 CMesh::s_pCBMesh= NULL;

void CMesh::CompileCSShaders(CDXManager * pManager)
{
	s_pCSApplyTransform = pManager->CompileComputeShader(L"..\\Shaders\\Mesh.hlsl", "main");

	// Create constant buffer Mesh.hlsl
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbd.ByteWidth = 16 * ((sizeof(PARAMS_MESH_CB) + 15) / 16);
	dbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dbd.Usage = D3D11_USAGE_DYNAMIC;
	pManager->GetDevice()->CreateBuffer(&dbd, 0, &s_pCBMesh);
}

void CMesh::CreateVertexAndIndexBuffer(CDXManager * m_pManager)
{
	//1.- Crear los buffer de vértices e indices en el GPU.
	
	D3D11_BUFFER_DESC dbd;
	memset(&dbd, 0, sizeof(dbd));
	dbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	dbd.ByteWidth = sizeof(CDXPainter::VERTEX)*(this->m_Vertices.size());
	dbd.CPUAccessFlags = 0;
	/*
	D3D11_USAGE_DEFAULT GPU:R/W CPU:None
	D3D11_USAGE_DYNAMIC GPU:R   CPU:W
	D3D11_USAGE_IMMUTABLE GPU:R CPU:W once
	D3D11_USAGE_STAGING   GPU:None CPU:W/R
	*/
	dbd.Usage = D3D11_USAGE_DEFAULT;
	dbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	dbd.StructureByteStride = sizeof(CDXPainter::VERTEX);
	dbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA dsd;
	dsd.pSysMem = &(this->m_Vertices[0]);
	dsd.SysMemPitch = 0;
	dsd.SysMemSlicePitch = 0;
	m_pManager->GetDevice()->CreateBuffer(
		&dbd, &dsd, &m_pVertexBuffer);

	
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;

	memset(&uavd, 0, sizeof(uavd));
	uavd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavd.Buffer.NumElements = (this->m_Vertices.size()* sizeof(CDXPainter::VERTEX)) / sizeof(VECTOR4D);
	m_pManager->GetDevice()->CreateUnorderedAccessView(m_pVertexBuffer, &uavd, &m_pUAVVertexBuffer);

	// Create SRV for vertex buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	memset(&srvd, 0, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvd.Buffer.NumElements = (this->m_Vertices.size() * sizeof(CDXPainter::VERTEX)) / sizeof(VECTOR4D);

	m_pManager->GetDevice()->CreateShaderResourceView(m_pVertexBuffer, &srvd, &m_pSRVVertexBuffer);

	// Create index buffer 
	dsd.pSysMem = &(this->m_Indices[0]);
	dbd.BindFlags = D3D11_BIND_INDEX_BUFFER | D3D11_BIND_SHADER_RESOURCE;
	dbd.ByteWidth = sizeof(unsigned long)*m_Indices.size();
	m_pManager->GetDevice()->CreateBuffer(
		&dbd, &dsd, &m_pIndexBuffer);

	memset(&srvd, 0, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R32_UINT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvd.Buffer.NumElements = m_Indices.size();
	m_pManager->GetDevice()->CreateShaderResourceView(m_pIndexBuffer, &srvd, &m_pSRVIndexBuffer);

	m_pPrimitivesBuffer = m_pManager->CreateLoadBuffer(NULL, sizeof(centroid), m_Indices.size() / 3);
	m_pManager->GetDevice()->CreateUnorderedAccessView(m_pPrimitivesBuffer, NULL, &m_pUAVPrimitiveBuffer);

}

void CMesh::Draw(CDXPainter * m_pPainter)
{
	m_pPainter->DrawIndexed2(this->m_pVertexBuffer,
		this->m_pIndexBuffer,
		this->m_Indices.size(),
		PAINTER_DRAW);

}

CMesh::CMesh()
{
	//m_World = Identity();

	this->m_pIndexBuffer = NULL;
	this->m_pVertexBuffer = NULL;
	this->m_BVH = NULL;
}


CMesh::~CMesh()
{
}

bool CMesh::RayCast(VECTOR4D& RayOrigin,
	VECTOR4D & RayDir,
	multimap<float,INTERSECTIONINFO>& Faces //Lista de caras que se intersectan
	)
{
	unsigned long nFaces = m_Indices.size() / 3;
	unsigned long nBaseIndex = 0;
	unsigned long nIntersectedFaces = 0;
	for (unsigned long iFace = 0; iFace < nFaces;iFace++)
	{
		VECTOR4D &V0 = m_Vertices[m_Indices[nBaseIndex+0]].Position;
		VECTOR4D &V1 = m_Vertices[m_Indices[nBaseIndex+1]].Position;
		VECTOR4D &V2 = m_Vertices[m_Indices[nBaseIndex+2]].Position;
		VECTOR4D Intersection;
		if (RayCastOnTriangle(V0, V1, V2, RayOrigin, RayDir, Intersection))
		{
			float dist = Magnity(Intersection - RayOrigin);
			INTERSECTIONINFO II;
			II.Face = iFace;
			II.LocalPosition = Intersection;
			Faces.insert(make_pair(dist, II));
			nIntersectedFaces++;
		}
		nBaseIndex += 3;
	}
	return nIntersectedFaces!=0;
}

void CMesh::VertexShade(CDXPainter::VERTEX(*pVS)(CDXPainter::VERTEX V))
{
	for (unsigned long int i = 0; i < m_Vertices.size(); i++)
		m_Vertices[i] = pVS(m_Vertices[i]);
}

bool RaySphereIntersect(
	VECTOR4D& RayOrigin,
	VECTOR4D& RayDirection, VECTOR4D& SphereCenter,
	float r)
{
	VECTOR4D RO = RayOrigin - SphereCenter;
	float a = Dot(RayDirection, RayDirection);
	float b = 2 * Dot(RayDirection, RO);
	float c = Dot(RO, RO) - r*r;
	float disc = b*b - 4*a*c;
	if (disc < 0)
		return false;
	return true;
}
bool CMesh::RayCast(
	VECTOR4D& RayOrigin,
	VECTOR4D& RayDir,
	multimap<float, unsigned long>& Vertices, float radius)
{
	for (unsigned long i = 0; i < m_Vertices.size(); i++)
	{
		if (RaySphereIntersect(RayOrigin, RayDir, m_Vertices[i].Position, radius))
		{
			float dist = Magnity(m_Vertices[i].Position - RayOrigin);
			Vertices.insert(make_pair(dist, i));
		}
	}
	return Vertices.size()!=0;
}
#include <fstream>
using namespace std;
void CMesh::LoadSuzanne()
{
	fstream in;
	m_Indices.clear();
	m_Vertices.clear();
	in.open("..\\Assets\\Monkey.vertex", ios::in|ios::binary);

	if (!in.is_open())
	{
		return;
	}

	unsigned long ulVetices;
	in.read((char*)&ulVetices, sizeof(unsigned long));
	m_Vertices.resize(ulVetices);

	for (unsigned long i = 0; i < ulVetices; i++)
	{
		VECTOR4D V;
		in.read((char*)&V, sizeof(VECTOR4D));
		m_Vertices[i].Position = V;
	}

	in.close();

	in.open("..\\Assets\\Monkey.normal", ios::in | ios::binary);

	if (!in.is_open())
	{
		return;
	}

	for (unsigned long i = 0; i < ulVetices; i++)
	{
		in.read((char*)&m_Vertices[i].Normal, sizeof(VECTOR4D));
	}

	in.close();
	in.open("..\\Assets\\Monkey.index", ios::in | ios::binary);

	if (!in.is_open())
		return;

	unsigned long ulIndices;
	in.read((char*)&ulIndices, sizeof(unsigned long));

	m_Indices.resize(ulIndices);

	in.read((char*)&m_Indices[0], sizeof(unsigned long) * ulIndices);
	in.close();

	// Generar espacio de coordenadas de textura
	// Estrategia: Vamos a generar el espacio de textura
	// a partir de las posiciones de los vertices

	for (unsigned long i = 0; i < m_Vertices.size(); i++)
	{
		VECTOR4D TexCoord = { 0,0,0,0 };
		TexCoord.x = m_Vertices[i].Position.x;
		TexCoord.y = m_Vertices[i].Position.z;
		TexCoord.z = m_Vertices[i].Position.y;
		TexCoord = Normalize(TexCoord);
		TexCoord.x = TexCoord.x * 0.5 + 0.5;
		TexCoord.y = TexCoord.y * 0.5 + 0.5;

		m_Vertices[i].TexCoord = TexCoord;
	}
	return;
}

void CMesh::BuildTangentSpaceFromTexCoordsIndexed(void)
{
	vector<int> Histogram;
	vector<CDXPainter::VERTEX> Accum;


	Accum.resize(m_Vertices.size());
	memset(&Accum[0], 0, sizeof(CDXPainter::VERTEX)*Accum.size());

	Histogram.resize(m_Vertices.size());
	memset(&Histogram[0], 0, sizeof(int)*Histogram.size());
	CDXPainter::VERTEX* pVertices = &m_Vertices[0];
	unsigned long* pIndices = &m_Indices[0];
	int* pHistogram = &Histogram[0];
	for (unsigned int i = 0; i<m_Indices.size(); i += 3)
	{
		VECTOR4D V0, V1, V2, T0, T1, T2;
		MATRIX4D InvS, Mq, Mt;
		InvS = Identity();
		Mq = Identity();
		//Tomar un triangulo
		V0 = pVertices[pIndices[i]].Position;
		V1 = pVertices[pIndices[i + 1]].Position;
		V2 = pVertices[pIndices[i + 2]].Position;
		//y sus coordenadas de textura para formar la base ortornormal en espacio de vertice (espacio tangente)
		T0 = pVertices[pIndices[i]].TexCoord;
		T1 = pVertices[pIndices[i + 1]].TexCoord;
		T2 = pVertices[pIndices[i + 2]].TexCoord;

		Mq.vec[0] = V1 - V0;
		Mq.vec[1] = V2 - V0;
		InvS.vec[0] = T1 - T0;
		InvS.vec[1] = T2 - T0;
		Inverse(InvS, InvS);
		Mt = InvS*Mq;
		for (int j = 0; j<3; j++)
		{
			VECTOR4D temp1;
			VECTOR4D N = pVertices[pIndices[i + j]].Normal; //Normal de vertice
			VECTOR4D T = Mt.vec[0];
			VECTOR4D B = Mt.vec[1];
			//Ortogonalizacion con respecto a la normal de vertice
			N = Normalize(N);
			T = T - N*Dot(N, T);
			T = Normalize(T);
			VECTOR4D temp2;
			temp2 = B - N*Dot(N, B);
			B = temp2 - T*Dot(T, B);
			B = Normalize(B);
			Accum[pIndices[i + j]].Normal = Accum[pIndices[i + j]].Normal + N;
			Accum[pIndices[i + j]].Tangent = Accum[pIndices[i + j]].Tangent + T;
			Accum[pIndices[i + j]].Binormal = Accum[pIndices[i + j]].Binormal + B;
			pHistogram[pIndices[i + j]]++;
		}
	}
	for (unsigned int i = 0; i<m_Vertices.size(); i++)
	{
		float invFreq = 1.0f / pHistogram[i];
		pVertices[i].Normal = Accum[i].Normal*invFreq;
		pVertices[i].Tangent = Accum[i].Tangent*invFreq;
		pVertices[i].Binormal = Accum[i].Binormal*invFreq;
		pVertices[i].Normal = Normalize(pVertices[i].Normal);
		pVertices[i].Tangent = Normalize(pVertices[i].Tangent);
		pVertices[i].Binormal = Normalize(pVertices[i].Binormal);
	}
}

void CMesh::BuildTangentSpaceFromTexCoordsIndexed(bool bGenerateNormal)
{
	vector<int> Histogram;
	vector<CDXPainter::VERTEX> Accum;
	Accum.resize(m_Vertices.size());
	memset(&Accum[0], 0, sizeof(CDXPainter::VERTEX)*Accum.size());
	Histogram.resize(m_Vertices.size());
	memset(&Histogram[0], 0, sizeof(int)*Histogram.size());
	CDXPainter::VERTEX* pVertices = &m_Vertices[0];
	unsigned long* pIndices = &m_Indices[0];
	int* pHistogram = &Histogram[0];
	for (unsigned int i = 0; i<m_Indices.size(); i += 3)
	{
		VECTOR4D V0, V1, V2, T0, T1, T2;
		MATRIX4D InvS, Mq, Mt;
		InvS = Identity();
		Mq = Identity();
		//Tomar un triangulo
		V0 = pVertices[pIndices[i]].Position;
		V1 = pVertices[pIndices[i + 1]].Position;
		V2 = pVertices[pIndices[i + 2]].Position;
		//y sus coordenadas de textura para formar la base ortornormal en espacio de v�rtice (espacio tangente)
		T0 = pVertices[pIndices[i]].TexCoord;
		T1 = pVertices[pIndices[i + 1]].TexCoord;
		T2 = pVertices[pIndices[i + 2]].TexCoord;
		Mq.vec[0] = V1 - V0;
		Mq.vec[1] = V2 - V0;
		InvS.vec[0] = T1 - T0;
		InvS.vec[1] = T2 - T0;
		Inverse(InvS, InvS);
		Mt = InvS*Mq;
		VECTOR4D T = Normalize(Mt.vec[0]);
		VECTOR4D B = Normalize(Mt.vec[1]);
		for (int j = 0; j<3; j++)
		{

			VECTOR4D N = Normalize(bGenerateNormal ? Cross3(V1 - V0, V2 - V0) : m_Vertices[pIndices[i + j]].Normal);
			//Ortogonalizaci�n con respecto a la normal de v�rtice
			T = Normalize(T - N*Dot(N, T));
			B = Normalize(B - N*Dot(N, B) - T*Dot(T, B));
			Accum[pIndices[i + j]].Normal = Accum[pIndices[i + j]].Normal + N;
			Accum[pIndices[i + j]].Tangent = Accum[pIndices[i + j]].Tangent + T;
			Accum[pIndices[i + j]].Binormal = Accum[pIndices[i + j]].Binormal + B;
			pHistogram[pIndices[i + j]]++;
		}
	}
	for (unsigned int i = 0; i<m_Vertices.size(); i++)
	{
		float invFreq = 1.0f / pHistogram[i];
		pVertices[i].Normal = Normalize(bGenerateNormal ? (Accum[i].Normal*invFreq) : pVertices[i].Normal);
		pVertices[i].Tangent = Normalize(Accum[i].Tangent*invFreq);
		pVertices[i].Binormal = Normalize(Accum[i].Binormal*invFreq);
	}
}

void CMesh::Optimize()
{
	//Remover vertices duplicados, Complejidad temporal O(N^2)
	vector<int> VertexRemoved;
	vector<CDXPainter::VERTEX> VertexOut;
	vector<int> VertexReplacedBy;
	int nVertexOut = 0;
	VertexOut.resize(m_Vertices.size());
	VertexReplacedBy.resize(m_Vertices.size());
	VertexRemoved.resize(m_Vertices.size());
	for (unsigned int i = 0; i < VertexReplacedBy.size(); i++) VertexReplacedBy[i] = i;
	for (auto &x : VertexRemoved) x = 0;
	//Para cada vertice, eliminar si existe otro igual en el resto del buffer
	for (unsigned int j = 0; j < m_Vertices.size() - 1; j++)
	{
		if (!VertexRemoved[j]) //Si no se ha removido, entonces, es �nico
		{
			VertexOut[nVertexOut] = m_Vertices[j];
			VertexReplacedBy[j] = nVertexOut;
			nVertexOut++;
		}
		else
			continue;
		//Para todos los demas vertices, compararlo con el de referencia [j]
		for (unsigned int i = j + 1; i < m_Vertices.size(); i++)
		{
			if (VertexRemoved[i]) continue;
			VECTOR4D dist = m_Vertices[i].Position - m_Vertices[j].Position;
			if (Dot(dist, dist) < 0.000001f)
			{
				VertexRemoved[i] = 1;
				VertexReplacedBy[i] = nVertexOut - 1;
			}
		}
	}
	for (auto &index : m_Indices) index = VertexReplacedBy[index];
	VertexOut.resize(nVertexOut);
	m_Vertices = VertexOut;
	m_Vertices.shrink_to_fit();
	//Eliminar triangulos cuya area se aproxime a cero.  Complejidad O(N)
	//(Estos triingulos no aportan informaci�n consistente para la generaci�n del
	//espacio tangencial y deben ser removidos)
	vector<unsigned long> Indices;
	Indices.resize(m_Indices.size());
	unsigned long IndicesOut = 0;
	for (int j = 0; j < m_Indices.size(); j += 3)
	{
		//Tomar un triangulo
		VECTOR4D V0, V1, V2;
		V0 = m_Vertices[m_Indices[j + 0]].Position;
		V1 = m_Vertices[m_Indices[j + 1]].Position;
		V2 = m_Vertices[m_Indices[j + 2]].Position;
		if (Magnity(Cross3(V1 - V0, V2 - V0)) > 0.000001f) //Si el doble del area del triangulo es mayor que 0.00001, considerarlo
		{
			Indices[IndicesOut++] = m_Indices[j + 0];
			Indices[IndicesOut++] = m_Indices[j + 1];
			Indices[IndicesOut++] = m_Indices[j + 2];
		}
	}
	Indices.resize(IndicesOut);
	m_Indices = Indices;
	m_Indices.shrink_to_fit();

}

void CMesh::CSApplyTranformation(MATRIX4D & t, CDXManager* pManager)
{
	D3D11_MAPPED_SUBRESOURCE ms;

	pManager->GetContext()->Map(s_pCBMesh, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	PARAMS_MESH_CB Temp = m_Params_Mesh_CB;

	Temp.Transformation = Transpose(t);

	memcpy(ms.pData, &Temp, sizeof(PARAMS_MESH_CB));
	pManager->GetContext()->Unmap(s_pCBMesh, 0);

	pManager->GetContext()->CSSetShader(s_pCSApplyTransform, NULL, NULL);
	pManager->GetContext()->CSSetConstantBuffers(0, 1, &s_pCBMesh);
	pManager->GetContext()->CSSetUnorderedAccessViews(0, 1, &this->m_pUAVVertexBuffer, NULL);

	int numGroups = (this->m_Vertices.size() / 1024) + 1;
	pManager->GetContext()->Dispatch(numGroups, 1, 1);

	/* Save result */
	pManager->CreateStoreBuffer(this->m_pVertexBuffer, sizeof(CDXPainter::VERTEX), this->m_Vertices.size() , &m_Vertices[0]);
}

void CMesh::GenerarCentroides()
{
	m_Centroides.resize(m_Indices.size()/3);
	for (int i = 0; i < m_Indices.size(); i += 3)
	{
		VECTOR4D A = m_Vertices[m_Indices[i]].Position;// *m_World;
		VECTOR4D B = m_Vertices[m_Indices[i + 1]].Position;// *m_World;
		VECTOR4D C = m_Vertices[m_Indices[i + 2]].Position;// *m_World;

		//m_Centroides[i / 3].id = i / 3;
		//m_Centroides[i / 3].code = 0;
		m_Centroides[i / 3].position = (A + B + C) / 3;
		//m_Centroides[i / 3].normal = Normalize(m_Centroides[i / 3].position);

		/* Get Max */
		m_Centroides[i / 3].max = MAX_VECTOR4D(A, B);
		m_Centroides[i / 3].max = MAX_VECTOR4D(m_Centroides[i / 3].max, C);

		/* Get Min */
		m_Centroides[i / 3].min = MIN_VECTOR4D(A, B);
		m_Centroides[i / 3].min = MIN_VECTOR4D(m_Centroides[i / 3].min, C);

	}
}

void CMesh::ResetColors()
{
	for (unsigned long i = 0; i < m_Vertices.size(); i++)
		m_Vertices[i].Color = { 0,0,0,0 };
}


void CMesh::MoveVertex(MATRIX4D Translation)
{
	m_Vertices[m_Indices[0]].Position = { float(rand() % 5), float(rand() % 5) ,float(rand() % 5),1 };
}

void CMesh::CreateMeshCollisionFromVMesh(CVMesh & vMesh)
{
	m_Indices = vMesh.m_Indices;
	m_Vertices = vMesh.m_Vertices;
}

void CMesh::ApplyTransformation(MATRIX4D & m)
{
	this->m_Box.max = this->m_Box.max * m;
	this->m_Box.min = this->m_Box.min * m;

	for (unsigned int i = 0; i < m_Vertices.size(); i++)
	{
		m_Vertices[i].Position = m_Vertices[i].Position * m;
	}
}