#pragma once

#include <vector>
#include "Graphics\Mesh.h"
#include "Graphics\DXBasicPainter.h"

using namespace std;
#define GAME_MAP_SIZE 11

/* Map values */
#define GAME_MAP_WALL		0x01
#define GAME_MAP_FLOOR		0x02
#define GAME_MAP_PLAYER		0x04
#define GAME_MAP_TARGET		0x08
#define GAME_MAP_CONTAINER	0x10

/* Move player flags */
#define PLAYER_MOVE_LESS_X 0x01
#define PLAYER_MOVE_MORE_X 0x02
#define PLAYER_MOVE_LESS_Y 0x04
#define PLAYER_MOVE_MORE_Y 0x08

/* Meshes */
#define GM_CILINDRO_PATH	"..\\Assets\\Meshes\\casa.blend"
#define GM_CUBE_PATH		"..\\Assets\\Meshes\\cube.blend"
#define GM_PLANE_PATH		"..\\Assets\\Meshes\\plane.blend"
#define GM_SPHERE_PATH		"..\\Assets\\Meshes\\perro3.blend"
#define GM_SUZZANE_PATH		"..\\Assets\\Meshes\\human.blend"

enum
{
	GAME_MAP_CILINDRO,
	GAME_MAP_CUBE,
	GAME_MAP_PLANE,
	GAME_MAP_SPHERE,
	GAME_MAP_SUZZANE,
	GAME_MAP_SUZZANE_BLUE,
	GAME_MAP_MESHES_SIZE
};

/* Meshes hamburgesa */
#define HAM_PARTE_ARRIBA_PATH	"..\\Assets\\Meshes\\partearriba.blend"
#define HAM_PARTE_ABAJO_PATH	"..\\Assets\\Meshes\\parteabajo.blend"
#define HAM_CARNE_PATH			"..\\Assets\\Meshes\\parteabajo.blend"
#define HAM_QUESO_PATH			"..\\Assets\\Meshes\\queso.blend"

enum
{
	HAM_PARTE_ARRIBA,
	HAM_PARTE_ABAJO,
	HAM_CARNE,
	HAM_QUESO,
	HAM_MESHES_SIZE
};


union ViewMap
{
	unsigned long array[GAME_MAP_SIZE*GAME_MAP_SIZE];
	unsigned long map [GAME_MAP_SIZE][GAME_MAP_SIZE];
};

/* Player states */
#define PLAYER_STATE_NOT_MOVE	0x01
#define PLAYER_STATE_MOVING		0x02
#define MAX_STEPS	 			10.f
struct Position
{
	int x, y;
};

struct Player
{
	int id;
	Position pos;
	Position nextPos;
	Position targetPos;
	Position dir;
	Position nextDir;
	int idTarget;
	unsigned long state;
	int step;

};

#define TARGET_STATE_IN_PLACE	0x01
#define TARGET_STATE_PICK_UP	0x02
#define TARGET_STATE_DROP		0x04
#define TARGET_STATE_NOT_MOVE	0x08
#define TARGET_STATE_MOVING		0x10

#define GAME_MAP_LEVELS 3

struct Target
{
	Position pos;
	Position nextPos;
	Position nextDir;
	unsigned long state;
	int step;
};

struct Container
{
	Position pos;
	int counter;
};


class CGameMap
{
public:
	CGameMap();
	~CGameMap();
	void LoadMeshes();
	void DrawMap(CDXBasicPainter* m_pPainter);
	bool MovePlayer(int id, int move);
	bool GetTarget(int id);
	bool DropTarget(int id);
	int LookForTarget(Position pos);
	bool PlayerHasTarget(int id) { return m_Players[id].idTarget != -1; }
	bool stillAreTargetsInMap();
	void setColorTo(int idMesh, VECTOR4D& Color);
	void setColorToHam(int idMesh, VECTOR4D& Color);
	Position GetPlayerPos(int id);
	ViewMap m_pGameMap[GAME_MAP_LEVELS];

	void LoadLevel(int level, int numPlayer);
	void ResetLevel(int numPlayers) { this->LoadLevel(m_iLevel, numPlayers); }
	void LoadNextLevel(int numPlayers) { this->LoadLevel(++m_iLevel, numPlayers); }
	bool HasMoreLevels() { return m_iLevel < (GAME_MAP_LEVELS - 1);  }
private:
	vector<CMesh>		m_Meshes;
	vector<CMesh>		m_HamMeshes;
	vector<Player>		m_Players;
	vector<Target>		m_Targets;
	vector<Container>	m_Containers;
	long				m_iLevel;
	
};

