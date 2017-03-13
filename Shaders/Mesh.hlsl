#include "Structures.hlsl"

#define NUM_THREAS_PER_GROUP 1024

RWStructuredBuffer<Vertex> vertexBuffer:register(u0);

cbuffer PARAMS:register(b0)
{
	matrix Transformation; //Model to World Trnasform
};

[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void main(uint3 id:SV_DispatchThreadID)
{
	vertexBuffer[id.x].Position = mul(vertexBuffer[id.x].Position , Transformation);
}