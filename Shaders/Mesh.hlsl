#define NUM_THREAS_PER_GROUP 1024

struct VERTEX_INPUT
{
	float4 Position:POSITION;
	float4 Normal:NORMAL;
	float4 Tanget : TANGENT;
	float4 Binormal : BINORMAL;
	float4 Color:COLOR;
	float4 TexCoord : TEXCOORD;
};

RWStructuredBuffer<VERTEX_INPUT> vertexBuffer:register(u0);

cbuffer PARAMS:register(b0)
{
	matrix Transformation; //Model to World Trnasform
};

[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void main(uint3 id:SV_DispatchThreadID)
{
	vertexBuffer[id.x].Position = mul(vertexBuffer[id.x].Position , Transformation);
}