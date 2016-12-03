#include "stdafx.h"
#include "GameMap.h"

/* assimp include files. */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

CGameMap::CGameMap()
{

}


CGameMap::~CGameMap()
{
}

void CGameMap::LoadMeshes()
{
	m_pGameMap[0] =
	{
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_WALL, GAME_MAP_WALL, GAME_MAP_WALL, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_WALL, GAME_MAP_WALL ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL
	};

	m_pGameMap[1] =
	{
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL
	};

	m_pGameMap[2] =
	{
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_WALL, GAME_MAP_WALL ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL
	};

	char* fileMeshes[GAME_MAP_MESHES_SIZE] = {
		GM_CILINDRO_PATH ,			//0
		GM_CUBE_PATH,				//1
		GM_PLANE_PATH,				//2
		GM_SPHERE_PATH,				//3
		GM_SUZZANE_PATH,			//4
	};

	m_Meshes.resize(GAME_MAP_MESHES_SIZE);

	for (unsigned int i = 0; i < GAME_MAP_MESHES_SIZE; i++)
	{
		/* the global Assimp scene object */
		//const struct aiScene* scene = aiImportFile(fileMeshes[i], aiProcessPreset_TargetRealtime_Fast);
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(fileMeshes[i], aiProcessPreset_TargetRealtime_Fast);

		MATRIX4D t;
		t.m00 = scene->mRootNode->mChildren[0]->mTransformation.a1;
		t.m01 = scene->mRootNode->mChildren[0]->mTransformation.a2;
		t.m02 = scene->mRootNode->mChildren[0]->mTransformation.a3;
		t.m03 = scene->mRootNode->mChildren[0]->mTransformation.a4;
		t.m10 = scene->mRootNode->mChildren[0]->mTransformation.b1;
		t.m11 = scene->mRootNode->mChildren[0]->mTransformation.b2;
		t.m12 = scene->mRootNode->mChildren[0]->mTransformation.b3;
		t.m13 = scene->mRootNode->mChildren[0]->mTransformation.b4;
		t.m20 = scene->mRootNode->mChildren[0]->mTransformation.c1;
		t.m21 = scene->mRootNode->mChildren[0]->mTransformation.c2;
		t.m22 = scene->mRootNode->mChildren[0]->mTransformation.c3;
		t.m23 = scene->mRootNode->mChildren[0]->mTransformation.c4;
		t.m30 = scene->mRootNode->mChildren[0]->mTransformation.d1;
		t.m31 = scene->mRootNode->mChildren[0]->mTransformation.d2;
		t.m32 = scene->mRootNode->mChildren[0]->mTransformation.d3;
		t.m33 = scene->mRootNode->mChildren[0]->mTransformation.d4;


		m_Meshes[i].m_World = Transpose(t);

		m_Meshes[i].m_Vertices.resize(scene->mMeshes[0]->mNumVertices);
		for (unsigned long j = 0; j < scene->mMeshes[0]->mNumVertices; j++)
		{
			m_Meshes[i].m_Vertices[j].Position = {
				scene->mMeshes[0]->mVertices[j].x,
				scene->mMeshes[0]->mVertices[j].y,
				scene->mMeshes[0]->mVertices[j].z,
				1 };

			m_Meshes[i].m_Vertices[j].Position = m_Meshes[i].m_Vertices[j].Position;
			
		}
		

		m_Meshes[i].m_Indices.resize(scene->mMeshes[0]->mNumFaces * scene->mMeshes[0]->mFaces[0].mNumIndices);
		for (unsigned long j = 0; j < scene->mMeshes[0]->mNumFaces; j++)
		{
			for (unsigned long k = 0; k < scene->mMeshes[0]->mFaces[j].mNumIndices; k++)
			{
				m_Meshes[i].m_Indices[j*scene->mMeshes[0]->mFaces[j].mNumIndices + k] = scene->mMeshes[0]->mFaces[j].mIndices[k];
			}
		}

		for (unsigned long j = 0; j < m_Meshes[i].m_Vertices.size(); j++)
		{
			VECTOR4D TexCoord = { 0,0,0,0 };
			if (i == GAME_MAP_FLOOR)
			{
				TexCoord.x = m_Meshes[i].m_Vertices[j].Position.y;
				TexCoord.y = m_Meshes[i].m_Vertices[j].Position.x;
			}
			else
			{
				TexCoord.x = m_Meshes[i].m_Vertices[j].Position.x;
				TexCoord.y = m_Meshes[i].m_Vertices[j].Position.y;
			}
			
			TexCoord.z = m_Meshes[i].m_Vertices[j].Position.y;
			TexCoord = Normalize(TexCoord);
			TexCoord.x = TexCoord.x * 0.5 + 0.5;
			TexCoord.y = TexCoord.y * 0.5 + 0.5;

			m_Meshes[i].m_Vertices[j].TexCoord = TexCoord;
		}

		//m_Meshes[i].Optimize();
		m_Meshes[i].BuildTangentSpaceFromTexCoordsIndexed(true);
		//m_Meshes[i].GenerarCentroides();

		strcpy(m_Meshes[i].m_cName, scene->mMeshes[0]->mName.C_Str());
	}

	/* Load color */
	VECTOR4D red = { 1,0,0,0 };
	VECTOR4D yellow = { 1,1,0,0 };
	VECTOR4D green = { 0,1,0,0 };
	VECTOR4D blue = { 0,0,1,0 };
	VECTOR4D white = { 1,1,1,0 };
	VECTOR4D browm = { 0.886, 0.494, 0.172 ,0 };
	setColorTo(GAME_MAP_CUBE, browm);
	setColorTo(GAME_MAP_PLANE, green);
	setColorTo(GAME_MAP_SPHERE, white);
	setColorTo(GAME_MAP_CILINDRO, yellow);
	setColorTo(GAME_MAP_SUZZANE, white);

}

void CGameMap::DrawMap(CDXBasicPainter * m_pPainter)
{
	for (unsigned long i = 0; i < GAME_MAP_SIZE; i++)
	{
		for (unsigned long j = 0; j < GAME_MAP_SIZE; j++)
		{
			if (m_pGameMap[m_iLevel].map[i][j] & GAME_MAP_WALL)
			{
				m_pPainter->m_Params.World = Translation(i * 2, j * 2, 0) * m_Meshes[GAME_MAP_CUBE].m_World;
				m_pPainter->DrawIndexed(
					&m_Meshes[GAME_MAP_CUBE].m_Vertices[0],
					m_Meshes[GAME_MAP_CUBE].m_Vertices.size(),
					&m_Meshes[GAME_MAP_CUBE].m_Indices[0],
					m_Meshes[GAME_MAP_CUBE].m_Indices.size(),
					PAINTER_DRAW);
			}

			if (m_pGameMap[m_iLevel].map[i][j] & GAME_MAP_FLOOR)
			{
				m_pPainter->m_Params.World = Translation(i * 2, j * 2, -1)* m_Meshes[GAME_MAP_PLANE].m_World;
				m_pPainter->m_Params.Flags1 |= MAPPING_NORMAL_TRUE;
				m_pPainter->DrawIndexed(
					&m_Meshes[GAME_MAP_PLANE].m_Vertices[0],
					m_Meshes[GAME_MAP_PLANE].m_Vertices.size(),
					&m_Meshes[GAME_MAP_PLANE].m_Indices[0],
					m_Meshes[GAME_MAP_PLANE].m_Indices.size(),
					PAINTER_DRAW);
				m_pPainter->m_Params.Flags1 &= ~MAPPING_NORMAL_TRUE;
			}
		}
	}
	
	for (unsigned long i = 0; i < m_Containers.size(); i++)
	{
		Position containerPos = m_Containers[i].pos;
		m_pPainter->m_Params.World = Translation(containerPos.x * 2, containerPos.y * 2, 0)* m_Meshes[GAME_MAP_CILINDRO].m_World;
		m_pPainter->DrawIndexed(
			&m_Meshes[GAME_MAP_CILINDRO].m_Vertices[0],
			m_Meshes[GAME_MAP_CILINDRO].m_Vertices.size(),
			&m_Meshes[GAME_MAP_CILINDRO].m_Indices[0],
			m_Meshes[GAME_MAP_CILINDRO].m_Indices.size(),
			PAINTER_DRAW);

		/*for (unsigned long j = 0; j < m_Containers[i].counter; j++)
		{
			m_pPainter->m_Params.World = Scaling(.2, .2, .2)*Translation((containerPos.x) * 2, (containerPos.y) * 2, (j+1)*2)* m_Meshes[GAME_MAP_SPHERE].m_World;
			m_pPainter->DrawIndexed(
				&m_Meshes[GAME_MAP_SPHERE].m_Vertices[0],
				m_Meshes[GAME_MAP_SPHERE].m_Vertices.size(),
				&m_Meshes[GAME_MAP_SPHERE].m_Indices[0],
				m_Meshes[GAME_MAP_SPHERE].m_Indices.size(),
				PAINTER_DRAW);
		}*/
	}

	for (unsigned long i = 0; i < m_Targets.size(); i++)
	{
		//MATRIX4D sc = Scaling(.2, .2, .2);
		MATRIX4D sc = RotationZ(3.1416 * 1.5 * m_Targets[i].nextDir.x) *
			RotationZ(3.1416 * 3.0 * m_Targets[i].nextDir.y) *
			/*RotationZ(3.1416 * 1.5) */ Scaling(.2, .2, .2);

		if (m_Targets[i].state & TARGET_STATE_DROP)
			continue;
		if (m_Targets[i].state & TARGET_STATE_IN_PLACE ||
			m_Targets[i].state & TARGET_STATE_NOT_MOVE)
		{
			m_pPainter->m_Params.World = RotationZ(3.1416 * 3) * sc *Translation(m_Targets[i].pos.x * 2, m_Targets[i].pos.y * 2, 0)* m_Meshes[GAME_MAP_SPHERE].m_World;
			m_pPainter->DrawIndexed(
				&m_Meshes[GAME_MAP_SPHERE].m_Vertices[0],
				m_Meshes[GAME_MAP_SPHERE].m_Vertices.size(),
				&m_Meshes[GAME_MAP_SPHERE].m_Indices[0],
				m_Meshes[GAME_MAP_SPHERE].m_Indices.size(),
				PAINTER_DRAW);
		}
		else if (m_Targets[i].state & TARGET_STATE_MOVING)
		{
			++m_Targets[i].step;

			if (m_Targets[i].nextPos.x != m_Targets[i].pos.x)
			{
				int dir = m_Targets[i].nextPos.x - m_Targets[i].pos.x > 0 ? 1 : -1;
				Position posTarget = m_Targets[i].pos;
				m_pPainter->m_Params.World = RotationZ(3.1416 * 3) *
					sc*
					Translation((posTarget.x * 2.f) + ((m_Targets[i].step / MAX_STEPS) * dir)*2.f, posTarget.y * 2, 0) *
					m_Meshes[GAME_MAP_SPHERE].m_World;

			}
			if (m_Targets[i].nextPos.y != m_Targets[i].pos.y)
			{
				int dir = m_Targets[i].nextPos.y - m_Targets[i].pos.y > 0 ? 1 : -1;
				Position posTarget = m_Targets[i].pos;
				m_pPainter->m_Params.World = RotationZ(3.1416 * 3) *
					sc*
					Translation((posTarget.x * 2.f), (posTarget.y * 2.f) + ((m_Targets[i].step / MAX_STEPS) * dir)*2.f, 0) *
					m_Meshes[GAME_MAP_SPHERE].m_World;
			}
			m_pPainter->DrawIndexed(
				&m_Meshes[GAME_MAP_SPHERE].m_Vertices[0],
				m_Meshes[GAME_MAP_SPHERE].m_Vertices.size(),
				&m_Meshes[GAME_MAP_SPHERE].m_Indices[0],
				m_Meshes[GAME_MAP_SPHERE].m_Indices.size(),
				PAINTER_DRAW);


			if (m_Targets[i].step == MAX_STEPS)
			{
				m_pGameMap[m_iLevel].map[m_Targets[i].pos.x][m_Targets[i].pos.y] &= ~GAME_MAP_TARGET;
				m_Targets[i].pos = m_Targets[i].nextPos;
				m_Targets[i].step = 0;
				m_Targets[i].state &= ~TARGET_STATE_MOVING;
				m_Targets[i].state |= TARGET_STATE_NOT_MOVE;
				m_pGameMap[m_iLevel].map[m_Targets[i].pos.x][m_Targets[i].pos.y] |= GAME_MAP_TARGET;
			}
		}
		
	}

	for (unsigned long i = 0; i < m_Players.size(); i++)
	{
		

		if (m_Players[i].state & PLAYER_STATE_NOT_MOVE)
		{
			MATRIX4D sc = RotationZ(3.1416 * 1.5 * m_Players[i].dir.x) *
				RotationZ(3.1416 * 3.0 * m_Players[i].dir.y) *
				RotationZ(3.1416 * 1.5) * Scaling(.5, .5, .5);

			Position posPlayer = m_Players[i].pos;

			m_pPainter->m_Params.World = sc* Translation(posPlayer.x * 2, posPlayer.y * 2, 0) * m_Meshes[GAME_MAP_SUZZANE].m_World;
			m_pPainter->DrawIndexed(
				&m_Meshes[GAME_MAP_SUZZANE].m_Vertices[0],
				m_Meshes[GAME_MAP_SUZZANE].m_Vertices.size(),
				&m_Meshes[GAME_MAP_SUZZANE].m_Indices[0],
				m_Meshes[GAME_MAP_SUZZANE].m_Indices.size(),
				PAINTER_DRAW);
		}
		else if (m_Players[i].state & PLAYER_STATE_MOVING)
		{
			MATRIX4D sc = RotationZ(3.1416 * 1.5 * m_Players[i].nextDir.x) *
				RotationZ(3.1416 * 3.0  * m_Players[i].nextDir.y) *
				RotationZ(3.1416 * 1.5) * Scaling(.5, .5, .5);
			++m_Players[i].step;

			if (m_Players[i].nextPos.x != m_Players[i].pos.x)
			{
				int dir = m_Players[i].nextPos.x - m_Players[i].pos.x > 0 ? 1 : -1;
				Position posPlayer = m_Players[i].pos;
				m_pPainter->m_Params.World = 
					sc*
					Translation((posPlayer.x * 2.f) + ((m_Players[i].step / MAX_STEPS) * dir)*2.f, posPlayer.y * 2, 0) *
					m_Meshes[GAME_MAP_SUZZANE].m_World;

			}
			if (m_Players[i].nextPos.y != m_Players[i].pos.y)
			{
				int dir = m_Players[i].nextPos.y - m_Players[i].pos.y > 0 ? 1 : -1;
				Position posPlayer = m_Players[i].pos;
				m_pPainter->m_Params.World = 
					sc*
					Translation((posPlayer.x * 2.f), (posPlayer.y * 2.f) + ((m_Players[i].step / MAX_STEPS) * dir)*2.f, 0) *
					m_Meshes[GAME_MAP_SUZZANE].m_World;
			}
			m_pPainter->DrawIndexed(
				&m_Meshes[GAME_MAP_SUZZANE].m_Vertices[0],
				m_Meshes[GAME_MAP_SUZZANE].m_Vertices.size(),
				&m_Meshes[GAME_MAP_SUZZANE].m_Indices[0],
				m_Meshes[GAME_MAP_SUZZANE].m_Indices.size(),
				PAINTER_DRAW);


			if (m_Players[i].step == MAX_STEPS)
			{
				m_pGameMap[m_iLevel].map[m_Players[i].pos.x][m_Players[i].pos.y] &= ~GAME_MAP_PLAYER;
				m_Players[i].pos = m_Players[i].nextPos;
				m_Players[i].step = 0;
				m_Players[i].state &= ~PLAYER_STATE_MOVING;
				m_Players[i].state |= PLAYER_STATE_NOT_MOVE;
				m_Players[i].dir = m_Players[i].nextDir;

				m_pGameMap[m_iLevel].map[m_Players[i].pos.x][m_Players[i].pos.y] |= GAME_MAP_PLAYER;

			}
		}
	}
}

bool CGameMap::MovePlayer(int id, int move)
{
	bool canmove = false;
	if (id >= m_Players.size())
		return false;
	if (m_Players[id].state & PLAYER_STATE_NOT_MOVE)
	{
		if (move & PLAYER_MOVE_LESS_X)
		{

			if (m_pGameMap[m_iLevel].map[m_Players[id].pos.x - 1][m_Players[id].pos.y] & GAME_MAP_FLOOR &&
				!(m_pGameMap[m_iLevel].map[m_Players[id].pos.x - 1][m_Players[id].pos.y] & GAME_MAP_PLAYER) &&
				!(m_pGameMap[m_iLevel].map[m_Players[id].pos.x - 1][m_Players[id].pos.y] & GAME_MAP_CONTAINER))
			{
				if (m_pGameMap[m_iLevel].map[m_Players[id].pos.x - 1][m_Players[id].pos.y] & GAME_MAP_TARGET)
				{
					if (m_Players[id].idTarget != -1)
					{
						Position targetPos = m_Targets[m_Players[id].idTarget].pos;
						if (targetPos.x == (m_Players[id].pos.x - 1) &&
							targetPos.y == m_Players[id].pos.y)
						{
							m_Players[id].nextPos.x = m_Players[id].pos.x - 1;
							m_Players[id].nextDir = { 1,0 };
							m_Players[id].nextPos.y = m_Players[id].pos.y;
							m_Players[id].state &= ~PLAYER_STATE_NOT_MOVE;
							m_Players[id].state |= PLAYER_STATE_MOVING;
							canmove =  true;
						}
					}
					
				}
				else
				{
					m_Players[id].nextPos.x = m_Players[id].pos.x - 1;
					m_Players[id].nextPos.y = m_Players[id].pos.y;
					m_Players[id].nextDir = { 1,0 };
					m_Players[id].state &= ~PLAYER_STATE_NOT_MOVE;
					m_Players[id].state |= PLAYER_STATE_MOVING;
					canmove = true;
				}
			}
		}
		if (move & PLAYER_MOVE_MORE_X)
		{
			if (m_pGameMap[m_iLevel].map[m_Players[id].pos.x + 1][m_Players[id].pos.y] & GAME_MAP_FLOOR &&
				!(m_pGameMap[m_iLevel].map[m_Players[id].pos.x + 1][m_Players[id].pos.y] & GAME_MAP_PLAYER) &&
				!(m_pGameMap[m_iLevel].map[m_Players[id].pos.x + 1][m_Players[id].pos.y] & GAME_MAP_CONTAINER))
			{
				if (m_pGameMap[m_iLevel].map[m_Players[id].pos.x + 1][m_Players[id].pos.y] & GAME_MAP_TARGET)
				{
					if (m_Players[id].idTarget != -1)
					{
						Position targetPos = m_Targets[m_Players[id].idTarget].pos;
						if (targetPos.x == (m_Players[id].pos.x + 1) &&
							targetPos.y == m_Players[id].pos.y)
						{
							m_Players[id].nextPos.x = m_Players[id].pos.x + 1;
							m_Players[id].nextDir = { -1,0 };
							m_Players[id].nextPos.y = m_Players[id].pos.y;
							m_Players[id].state &= ~PLAYER_STATE_NOT_MOVE;
							m_Players[id].state |= PLAYER_STATE_MOVING;
							canmove = true;
						}
					}
					
				}
				else
				{
					m_Players[id].nextPos.x = m_Players[id].pos.x + 1;
					m_Players[id].nextPos.y = m_Players[id].pos.y;
					m_Players[id].nextDir = { -1,0 };
					m_Players[id].state &= ~PLAYER_STATE_NOT_MOVE;
					m_Players[id].state |= PLAYER_STATE_MOVING;
					canmove = true;
				}
				
			}

		}
		if (move & PLAYER_MOVE_LESS_Y)
		{
			if (m_pGameMap[m_iLevel].map[m_Players[id].pos.x][m_Players[id].pos.y - 1] & GAME_MAP_FLOOR &&
				!(m_pGameMap[m_iLevel].map[m_Players[id].pos.x ][m_Players[id].pos.y-1] & GAME_MAP_PLAYER) &&
				!(m_pGameMap[m_iLevel].map[m_Players[id].pos.x ][m_Players[id].pos.y-1] & GAME_MAP_CONTAINER))
			{
				if (m_pGameMap[m_iLevel].map[m_Players[id].pos.x][m_Players[id].pos.y - 1] & GAME_MAP_TARGET)
				{
					if (m_Players[id].idTarget != -1)
					{
						Position targetPos = m_Targets[m_Players[id].idTarget].pos;
						if (targetPos.x == m_Players[id].pos.x &&
							targetPos.y == (m_Players[id].pos.y - 1))
						{
							m_Players[id].nextPos.x = m_Players[id].pos.x;
							m_Players[id].nextPos.y = m_Players[id].pos.y - 1;
							m_Players[id].nextDir = { 0, 1 };
							m_Players[id].state &= ~PLAYER_STATE_NOT_MOVE;
							m_Players[id].state |= PLAYER_STATE_MOVING;
							canmove = true;
						}
					}
				}
				else
				{
					m_Players[id].nextPos.x = m_Players[id].pos.x;
					m_Players[id].nextPos.y = m_Players[id].pos.y - 1;
					m_Players[id].nextDir = { 0, 1 };
					m_Players[id].state &= ~PLAYER_STATE_NOT_MOVE;
					m_Players[id].state |= PLAYER_STATE_MOVING;
					canmove = true;
				}
				
			}
		}
		if (move & PLAYER_MOVE_MORE_Y)
		{
			if (m_pGameMap[m_iLevel].map[m_Players[id].pos.x][m_Players[id].pos.y + 1] & GAME_MAP_FLOOR &&
				!(m_pGameMap[m_iLevel].map[m_Players[id].pos.x][m_Players[id].pos.y + 1] & GAME_MAP_PLAYER) &&
				!(m_pGameMap[m_iLevel].map[m_Players[id].pos.x][m_Players[id].pos.y + 1] & GAME_MAP_CONTAINER))
			{
				if (m_pGameMap[m_iLevel].map[m_Players[id].pos.x][m_Players[id].pos.y + 1] & GAME_MAP_TARGET)
				{
					if (m_Players[id].idTarget != -1)
					{
						Position targetPos = m_Targets[m_Players[id].idTarget].pos;
						if (targetPos.x == m_Players[id].pos.x &&
							targetPos.y == (m_Players[id].pos.y + 1))
						{
							m_Players[id].nextPos.x = m_Players[id].pos.x;
							m_Players[id].nextPos.y = m_Players[id].pos.y + 1;
							m_Players[id].nextDir = { 0, 0 };
							m_Players[id].state &= ~PLAYER_STATE_NOT_MOVE;
							m_Players[id].state |= PLAYER_STATE_MOVING;
							canmove = true;
						}
					}
					
				}
				else
				{
					m_Players[id].nextPos.x = m_Players[id].pos.x;
					m_Players[id].nextPos.y = m_Players[id].pos.y + 1;
					m_Players[id].nextDir = { 0, 0 };
					m_Players[id].state &= ~PLAYER_STATE_NOT_MOVE;
					m_Players[id].state |= PLAYER_STATE_MOVING;
					canmove = true;
				}
			}
		}
		if (m_Players[id].state & PLAYER_STATE_MOVING && m_Players[id].idTarget != -1)
		{
			m_Targets[m_Players[id].idTarget].state &= ~TARGET_STATE_NOT_MOVE;
			m_Targets[m_Players[id].idTarget].state |= TARGET_STATE_MOVING;
			m_Targets[m_Players[id].idTarget].step = 0;
			m_Targets[m_Players[id].idTarget].nextPos = m_Players[id].pos;
			m_Targets[m_Players[id].idTarget].nextDir = m_Players[id].dir;
		}
		//return false;
	}
	else if (m_Players[id].state & PLAYER_STATE_MOVING)
	{
		canmove = true;
	}
	return canmove;
	
}

bool CGameMap::GetTarget(int id)
{
	Position posPlayer = m_Players[id].pos;
	if (!(m_Players[id].state & PLAYER_STATE_MOVING) &&
		m_Players[id].idTarget == -1)
	{
		if (m_pGameMap[m_iLevel].map[posPlayer.x + 1][posPlayer.y] & GAME_MAP_TARGET)
		{
			Position posTarget;
			posTarget.x = posPlayer.x + 1;
			posTarget.y = posPlayer.y;

			int idTarget = LookForTarget(posTarget);
			m_Players[id].idTarget = idTarget;
			m_Targets[idTarget].state = 0;
			m_Targets[idTarget].state = TARGET_STATE_PICK_UP | TARGET_STATE_NOT_MOVE;
			return true;
		}
		if (m_pGameMap[m_iLevel].map[posPlayer.x - 1][posPlayer.y] & GAME_MAP_TARGET)
		{
			Position posTarget;
			posTarget.x = posPlayer.x - 1;
			posTarget.y = posPlayer.y;

			int idTarget = LookForTarget(posTarget);
			m_Players[id].idTarget = idTarget;
			m_Targets[idTarget].state = 0;
			m_Targets[idTarget].state = TARGET_STATE_PICK_UP | TARGET_STATE_NOT_MOVE;
			return true;
		}
		if (m_pGameMap[m_iLevel].map[posPlayer.x][posPlayer.y + 1] & GAME_MAP_TARGET)
		{
			Position posTarget;
			posTarget.x = posPlayer.x;
			posTarget.y = posPlayer.y+1;

			int idTarget = LookForTarget(posTarget);
			m_Players[id].idTarget = idTarget;
			m_Targets[idTarget].state = 0;
			m_Targets[idTarget].state = TARGET_STATE_PICK_UP | TARGET_STATE_NOT_MOVE;
			return true;

		}
		if (m_pGameMap[m_iLevel].map[posPlayer.x][posPlayer.y - 1] & GAME_MAP_TARGET)
		{
			Position posTarget;
			posTarget.x = posPlayer.x;
			posTarget.y = posPlayer.y-1;

			int idTarget = LookForTarget(posTarget);
			m_Players[id].idTarget = idTarget;
			m_Targets[idTarget].state = 0;
			m_Targets[idTarget].state = TARGET_STATE_PICK_UP | TARGET_STATE_NOT_MOVE;
			return true;
		}
	}
	return false;
}

bool CGameMap::DropTarget(int id)
{
	Position posPlayer = m_Players[id].pos;
	if (!(m_Players[id].state & PLAYER_STATE_MOVING) && m_Players[id].idTarget != -1)
	{
		if (m_pGameMap[m_iLevel].map[posPlayer.x + 1][posPlayer.y] & GAME_MAP_CONTAINER ||
			m_pGameMap[m_iLevel].map[posPlayer.x - 1][posPlayer.y] & GAME_MAP_CONTAINER ||
			m_pGameMap[m_iLevel].map[posPlayer.x][posPlayer.y + 1] & GAME_MAP_CONTAINER ||
			m_pGameMap[m_iLevel].map[posPlayer.x][posPlayer.y - 1] & GAME_MAP_CONTAINER)
		{
			int idTarget = m_Players[id].idTarget;
			Position targetPos = m_Targets[idTarget].pos;
			m_pGameMap[m_iLevel].map[targetPos.x][targetPos.y] &= ~GAME_MAP_TARGET;
			m_Players[id].idTarget = -1;
			m_Targets[idTarget].state = 0;
			m_Targets[idTarget].state = TARGET_STATE_DROP;
			m_Containers[0].counter++;
			return true;
		}
	}
	return false;
}

int CGameMap::LookForTarget(Position pos)
{
	for (unsigned long i = 0; i < m_Targets.size(); i++)
	{
		if (pos.x == m_Targets[i].pos.x &&
			pos.y == m_Targets[i].pos.y)
			return i;
	}
	return -1;
}

bool CGameMap::stillAreTargetsInMap()
{
	
	return !(m_Containers[0].counter == m_Targets.size());
}

void CGameMap::setColorToHam(int idMesh, VECTOR4D & Color)
{
	for (unsigned int i = 0; i < m_HamMeshes[idMesh].m_Vertices.size(); i++)
	{
		m_HamMeshes[idMesh].m_Vertices[i].Color = Color;
	}
}

void CGameMap::setColorTo(int idMesh, VECTOR4D & Color)
{
	for (unsigned int i = 0; i < m_Meshes[idMesh].m_Vertices.size(); i++)
	{
		m_Meshes[idMesh].m_Vertices[i].Color = Color;
	}
}

Position CGameMap::GetPlayerPos(int id)
{
	return m_Players[id].pos;
}

void CGameMap::LoadLevel(int level, int numPlayers)
{
	m_pGameMap[0] =
	{
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_WALL, GAME_MAP_WALL, GAME_MAP_WALL, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_WALL, GAME_MAP_WALL ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL
	};

	m_pGameMap[1] =
	{
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL
	};

	m_pGameMap[2] =
	{
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_FLOOR , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_WALL, GAME_MAP_WALL, GAME_MAP_WALL ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL , GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR, GAME_MAP_FLOOR ,GAME_MAP_WALL ,
		GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL , GAME_MAP_WALL  ,GAME_MAP_WALL
	};

	m_iLevel = level;
	/* Init players */
	m_Players.resize(numPlayers);
	m_Players[0].id = 1;
	m_Players[0].pos.x = 1;
	m_Players[0].pos.y = 1;
	m_Players[0].step = 0;
	m_Players[0].idTarget = -1;
	m_Players[0].state |= PLAYER_STATE_NOT_MOVE;
	m_Players[0].dir = {0,1};
	m_Players[0].nextDir = { 0,1 };

	m_pGameMap[level].map[1][1] |= GAME_MAP_PLAYER;

	if (numPlayers > 1)
	{
		m_Players[1].id = 2;
		m_Players[1].pos.x = 1;
		m_Players[1].pos.y = 2;
		m_Players[1].step = 0;
		m_Players[1].idTarget = -1;
		m_Players[1].state |= PLAYER_STATE_NOT_MOVE;
		m_Players[1].dir = { 0,1 };
		m_Players[1].nextDir = { 0,1 };
		m_pGameMap[level].map[1][2] |= GAME_MAP_PLAYER;
	}

	/* Init targets */
	m_Targets.resize(3);
	m_Targets[0].state = TARGET_STATE_IN_PLACE;
	m_Targets[0].pos.x = 9;
	m_Targets[0].pos.y = 9;
	m_Targets[0].nextDir = { 0,2 };

	m_pGameMap[level].map[9][9] |= GAME_MAP_TARGET;

	m_Targets[1].state = TARGET_STATE_IN_PLACE;
	m_Targets[1].pos.x = 9;
	m_Targets[1].pos.y = 4;
	m_Targets[1].nextDir = { 0,0 };

	m_pGameMap[level].map[9][4] |= GAME_MAP_TARGET;

	m_Targets[2].state = TARGET_STATE_IN_PLACE;
	m_Targets[2].pos.x = 4;
	m_Targets[2].pos.y = 9;
	m_Targets[2].nextDir = { 1,0 };

	m_pGameMap[level].map[4][9] |= GAME_MAP_TARGET;

	/* Init containers */
	m_Containers.resize(1);
	m_Containers[0].counter = 0;
	m_Containers[0].pos.x = 8;
	m_Containers[0].pos.y = 1;
	m_pGameMap[level].map[8][1] |= GAME_MAP_CONTAINER;
}
