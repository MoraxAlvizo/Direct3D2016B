/* 

Estructuras usadas en los shaders

*/

struct VERTEX_INPUT
{
	float4 Position:POSITION;
	float4 Normal:NORMAL;
	float4 Tanget : TANGENT;
	float4 Binormal : BINORMAL;
	float4 Color:COLOR;
	float4 TexCoord : TEXCOORD;
};

struct VERTEX_OUTPUT
{
	float4 Position : SV_Position;
	float4 PositionNonProjected:POSITION;
	float4 Normal:NORMAL;
	float4 Color:COLOR;
	float4 A:NORMAL1, B : NORMAL2, C : NORMAL3;
	float4 TexCoord : TEXCOORD;
	float4 LightPosition : POSITION1;
	float4 ReflectionVector : TEXCOORD1;
	float4 FogAmount : TEXCOORD2;
};

struct Vertex
{
	float4 Position:POSITION;
	float4 Normal:NORMAL;
	float4 Tanget : TANGENT;
	float4 Binormal : BINORMAL;
	float4 Color:COLOR;
	float4 TexCoord : TEXCOORD;
};

struct Primitive
{
	uint id;
	float4 position;
	float4 max;
	float4 min;
};

struct IDPrimitive
{
	uint id;
	float4 position;
};


struct Box
{
	float4 min;
	float4 max;
	uint idPrimitive;
	int numPrimitives; /* -1 means that it is a invalid node */
	uint offset;
	uint axis;
	bool isLeaf;
};

struct Vecino
{
	uint idVecino;
	float distance;
};
#define NUM_VECINOS 24

struct MassSpring
{
	float4 velocity;
	float4 fuerza;
	Vecino vecinos[NUM_VECINOS];
	uint numVecinos;
	float masa;
};

struct VecinosVisuales
{
	uint size;
	uint idVecinos[NUM_VECINOS];
};

struct Tetrahedron
{
	uint4 indexes;
	float4 C;
};

struct Collision
{
	float4 newPosition;
	float4 newVelocity;
	uint numHits;
	uint3 padding;
};