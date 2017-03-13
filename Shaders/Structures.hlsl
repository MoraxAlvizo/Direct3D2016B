/* 

Estructuras usadas en los shaders

*/

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
	bool isLeaf;
	uint offset;
	uint axis;
};
