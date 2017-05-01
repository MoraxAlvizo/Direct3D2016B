#include "Structures.hlsl"

#define NUM_THREAS_PER_GROUP 512

// UAV 
RWStructuredBuffer<MassSpring> g_MassSpringBuffer:register(u0);
RWStructuredBuffer<Vertex> g_Vertices:register(u1);
RWStructuredBuffer<int> g_StillInCollision:register(u2);
// SRV 
StructuredBuffer<Tetrahedron> g_TetraIndices:register(t0);
StructuredBuffer<Collision> g_CollisionForces:register(t1);
StructuredBuffer<VecinosVisuales> g_VisualNeighbors:register(t2);
StructuredBuffer<uint> g_VisualIndexes:register(t3);


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

#define Kv (1000)

[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void VolumePreservation(uint3 id:SV_DispatchThreadID)
{
	uint i1, i2, i3, i4;

	i1 = g_TetraIndices[id.x].indexes.x;
	i2 = g_TetraIndices[id.x].indexes.y;
	i3 = g_TetraIndices[id.x].indexes.z;
	i4 = g_TetraIndices[id.x].indexes.w;

	float3 x1, x2, x3, x4;
	x1 = g_Vertices[i1].Position.xyz;
	x2 = g_Vertices[i2].Position.xyz;
	x3 = g_Vertices[i3].Position.xyz;
	x4 = g_Vertices[i4].Position.xyz;

	float3 e1, e2, e3;
	e1 = x2 - x1;
	e2 = x3 - x1;
	e3 = x4 - x1;

	//float V = (1 / 6.f) * dot(e1, cross(e2, e3));
	float3 C = g_TetraIndices[id.x].C.xyz;// ((1 / 6.f) * e1 * (cross(e2, e3))) - V;

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

	g_MassSpringBuffer[id.x].fuerza =  g_MassSpringBuffer[id.x].fuerza +  F + Gravity;
}

[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void ApplyForces(uint3 id:SV_DispatchThreadID)
{
	
	/*if (id.x == 1)
		return;*/

	Collision collision = g_CollisionForces[id.x];

	if (collision.numHits != 0)
	{
		g_MassSpringBuffer[id.x].velocity = (collision.newVelocity / (float)collision.numHits);
	}
	else
	{
		g_MassSpringBuffer[id.x].velocity = g_MassSpringBuffer[id.x].velocity + (Delta_t * (g_MassSpringBuffer[id.x].fuerza / g_MassSpringBuffer[id.x].masa));
	}

	g_Vertices[id.x].Position = g_Vertices[id.x].Position + (Delta_t * g_MassSpringBuffer[id.x].velocity);
	g_Vertices[id.x].Position.w = 1;
}

[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void ComputeNormals(uint3 id:SV_DispatchThreadID)
{
	float4 Normal = float4(0, 0, 0, 0);
	VecinosVisuales v = g_VisualNeighbors[id.x];

	for (uint i = 0; i < v.size; i++)
	{
		uint t = v.idVecinos[i] * 3;

		float3 V0 = g_Vertices[g_VisualIndexes[t]].Position.xyz;
		float3 V1 = g_Vertices[g_VisualIndexes[t + 1]].Position.xyz;
		float3 V2 = g_Vertices[g_VisualIndexes[t + 2]].Position.xyz;

		Normal += float4(normalize(cross(V1 - V0, V2 - V0)), 0);
	}

	g_Vertices[id.x].Normal = Normal*(1.f / v.size);
}
