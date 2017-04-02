#include "Structures.hlsl"

#define NUM_THREAS_PER_GROUP 512

// UAV 
RWStructuredBuffer<MassSpring> g_MassSpringBuffer:register(u0);
RWStructuredBuffer<Vertex> g_Vertices:register(u1);
// SRV 
StructuredBuffer<uint4> g_TetraIndices:register(t0);


cbuffer PARAMS:register(b0)
{
	float4 Gravity;
	float K;
	float Delta_t;
};

[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void InitForce(uint3 id:SV_DispatchThreadID)
{
	g_MassSpringBuffer[id.x].fuerza = float4(0, 0, 0, 0);
}

[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void VolumePreservation(uint3 id:SV_DispatchThreadID)
{
	uint i1, i2, i3, i4;

	i1 = g_TetraIndices[id.x].x;
	i2 = g_TetraIndices[id.x].y;
	i3 = g_TetraIndices[id.x].x;
	i4 = g_TetraIndices[id.x].w;

	float3 x1, x2, x3, x4;
	x1 = g_Vertices[i1].Position.xyz;
	x2 = g_Vertices[i2].Position.xyz;
	x3 = g_Vertices[i3].Position.xyz;
	x4 = g_Vertices[i4].Position.xyz;

	float3 e1, e2, e3;
	e1 = x2 - x1;
	e2 = x3 - x1;
	e3 = x4 - x1;

	float V = (1 / 6.f) * dot(e1, cross(e2, e3));
	float3 C = ((1 / 6.f) * e1 * (cross(e2, e3))) - V;

	float3 F1, F2, F3, F4;

	F1 = K*C*(cross(e2 - e1, e3 - e1));
	F2 = K*C*cross(e3, e2);
	F3 = K*C*cross(e1, e3);
	F4 = K*C*cross(e2, e1);

	g_MassSpringBuffer[i1].fuerza = g_MassSpringBuffer[i1].fuerza + float4(F1,1);
	g_MassSpringBuffer[i2].fuerza = g_MassSpringBuffer[i2].fuerza + float4(F2, 1);
	g_MassSpringBuffer[i3].fuerza = g_MassSpringBuffer[i3].fuerza + float4(F3, 1);
	g_MassSpringBuffer[i4].fuerza = g_MassSpringBuffer[i4].fuerza + float4(F4, 1);
}

[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void ComputeForces(uint3 id:SV_DispatchThreadID)
{
	float4 F = float4( 0,0,0,0 );
	uint numVecinos = g_MassSpringBuffer[id.x].numVecinos;
	for (uint i = 0; i < numVecinos; i++) //auto vecino : g_MassSpringBuffer[i].vecinos)
	{
		Vecino vecino = g_MassSpringBuffer[id.x].vecinos[i];
		float4 V = g_Vertices[vecino.idVecino].Position - g_Vertices[id.x].Position;
		float M = length(V);
		float L = vecino.distance;

		F = F + ((K * (M - L)) * (V/M));
	}

	g_MassSpringBuffer[id.x].fuerza =  g_MassSpringBuffer[id.x].fuerza + F + Gravity;
}

[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void ApplyForces(uint3 id:SV_DispatchThreadID)
{
	if (id.x == 1)
		return;

	g_MassSpringBuffer[id.x].velocity = g_MassSpringBuffer[id.x].velocity + (Delta_t * (g_MassSpringBuffer[id.x].fuerza / g_MassSpringBuffer[id.x].masa));
	g_Vertices[id.x].Position = g_Vertices[id.x].Position + (Delta_t * g_MassSpringBuffer[id.x].velocity);
	g_Vertices[id.x].Position.w = 1;
}
