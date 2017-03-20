#include "Structures.hlsl"

#define NUM_THREAS_PER_GROUP 512
#define EXCLUSIVE 0
#define INCLUSIVE 1
#define WARP_SIZE 32
#define IDENTITY 0
#define INT_MIN -2147483647 
#define INT_MAX 2147483647

#define SET_MAX_VEC4D (float3((float)INT_MAX,(float)INT_MAX,(float)INT_MAX))
#define SET_MIN_VEC4D (float3((float)INT_MIN,(float)INT_MIN,(float)INT_MIN))

cbuffer CB_BVH:register(b0)
{
	uint level;
	uint numPrimitives;
	uint suma;
};

StructuredBuffer<Vertex> g_Vertices:register(t0);
StructuredBuffer<uint> g_Indices:register(t1);

RWStructuredBuffer<Primitive> g_Primitives:register(u0);
RWStructuredBuffer<Box> g_BVH:register(u1);

[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void Prebuild(uint3 id:SV_DispatchThreadID)
{
	uint i = id.x;
	float4 A = g_Vertices[g_Indices[i*3]].Position;
	float4 B = g_Vertices[g_Indices[i*3 + 1]].Position;
	float4 C = g_Vertices[g_Indices[i*3 + 2]].Position;

	g_Primitives[i].id = i;
	g_Primitives[i].position = (A + B + C) / 3;

	/* Get Max */
	g_Primitives[i].max = max(max(A, B), C);

	/* Get Min */
	g_Primitives[i].min = min(min(A, B), C);

	if (i == 0)
	{
		/* Iniciar nodo raiz */
		g_BVH[1].numPrimitives = numPrimitives;
		g_BVH[1].offset = 0;
	}
}

groupshared float3 MaxBuf[NUM_THREAS_PER_GROUP];
groupshared float3 MinBuf[NUM_THREAS_PER_GROUP];
groupshared IDPrimitive PrimBuf[NUM_THREAS_PER_GROUP];
groupshared uint scanWarpArray[NUM_THREAS_PER_GROUP];
groupshared uint falseTotal;
groupshared uint axis;
groupshared float middle;

//refer to Figure 1.6 in Efficient ParallelScan algorithms Book page 14
int ScanWarp(const uint groupIndex : SV_GroupIndex, const int scanType)
{
	//index of thread in warp (0.. 31)
	const uint lane = groupIndex& (WARP_SIZE - 1);

	if (lane >= 1)
		scanWarpArray[groupIndex] += scanWarpArray[groupIndex - 1];
	
	if (lane >= 2)
		scanWarpArray[groupIndex] += scanWarpArray[groupIndex - 2];
	
	if (lane >= 4)
		scanWarpArray[groupIndex] += scanWarpArray[groupIndex - 4];
	
	if (lane >= 8)
		scanWarpArray[groupIndex] += scanWarpArray[groupIndex - 8];
	
	if (lane >= 16)
		scanWarpArray[groupIndex] += scanWarpArray[groupIndex - 16];

	if (scanType == INCLUSIVE)
		return scanWarpArray[groupIndex];
	else
		return (lane >0) ? scanWarpArray[groupIndex - 1] : IDENTITY;
}

// Efficient parallel scan algorithms for many core GPU's diagram figure 1.6 
int ScanBlock(volatile int bit, const uint groupIndex : SV_GroupIndex, const int scanType)
{
	int fValue;
	const uint lane = groupIndex & (WARP_SIZE - 1);
	const uint id = groupIndex >> 5;
	scanWarpArray[groupIndex] = bit;
	GroupMemoryBarrierWithGroupSync();
	//step 1 Intra-warp scan in each warp
	fValue = ScanWarp(groupIndex, scanType);
	GroupMemoryBarrierWithGroupSync();

	//step 2 collect per-warp partial results
	if (lane == (WARP_SIZE - 1))
		scanWarpArray[id] = scanWarpArray[groupIndex];

	GroupMemoryBarrierWithGroupSync();

	//step 3 use 1st warp to scan per-warp results from step 2
	if (id == 0)
		ScanWarp(groupIndex, INCLUSIVE);
	GroupMemoryBarrierWithGroupSync();

	//step 4 accumulate results from steps 1 and 3
	if (id>0)
		fValue += scanWarpArray[id - 1];

	return fValue;
}

uint Split(
	bool pred,
	uint  groupIndex      : SV_GroupIndex)			//Thread Index/Id withing a Group
{
	int true_before;

	//Split Figure 2.of desgning efficient sorting algorithms for Manycore GPUS
	//(1) Count 'True' pedictates held by lower - numered threads
	true_before = ScanBlock(pred != 0, groupIndex, EXCLUSIVE);

	//(2)last thread calculates the total number of 'False' predicates
	if (groupIndex == NUM_THREAS_PER_GROUP - 1)
		falseTotal = NUM_THREAS_PER_GROUP - (true_before + (pred ? 1 : 0));
	GroupMemoryBarrierWithGroupSync();

	//(3) Compute and return the 'rank' for this thread
	if (pred)
		return true_before + falseTotal;
	else
		return groupIndex - true_before;
}


[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void Build(uint3 id:SV_DispatchThreadID, 
		   uint3 lid : SV_GroupThreadID,
		   uint3 gid : SV_GroupID,
		   uint  groupIndex : SV_GroupIndex)
{
	/* 1. Obtener el eje mas grande */
	/* 1.1 Cargar datos a la memoria de grupo*/
	/* Conseguir la cantidad de primitivas q tiene la caja actual */
	uint num_box = (1 << level) + gid.x + suma;
	uint offet_prim = g_BVH[num_box].offset;
	uint num_primitives = g_BVH[num_box].numPrimitives;

	if (lid.x < num_primitives)
	{
		PrimBuf[lid.x].id = lid.x;
		PrimBuf[lid.x].position = g_Primitives[lid.x + offet_prim].position;

		/* Cambiar esto por el max y min de la primitiva en el futuro cercano*/
		MinBuf[lid.x] = MaxBuf[lid.x] = PrimBuf[lid.x].position.xyz;
	}
	else
	{
		PrimBuf[lid.x].id = lid.x;
		MinBuf[lid.x] = SET_MAX_VEC4D;
		MaxBuf[lid.x] = SET_MIN_VEC4D;
	}
	
	GroupMemoryBarrierWithGroupSync();

	for (uint i = NUM_THREAS_PER_GROUP >> 1; i >= 1; i >>= 1)
	{
		float3 maxLocal = SET_MIN_VEC4D;
		float3 minLocal = SET_MAX_VEC4D;

		if (lid.x < i)
		{
			uint index1 = (lid.x << 1);
			uint index2 = (lid.x << 1) + 1;
			maxLocal = max(MaxBuf[index1], MaxBuf[index2]);
			minLocal = min(MinBuf[index1], MinBuf[index2]);
		}

		GroupMemoryBarrierWithGroupSync();
		MaxBuf[lid.x] = maxLocal;
		MinBuf[lid.x] = minLocal;
	}

	/* 2. Check what axis is longer */
	if (lid.x == 0)
	{
		float size_x = MaxBuf[0].x - MinBuf[0].x;
		float size_y = MaxBuf[0].y - MinBuf[0].y;
		float size_z = MaxBuf[0].z - MinBuf[0].z;

		axis = (size_x > size_y) ? ((size_x > size_z) ? 0 : 2) : ((size_y > size_z) ? 1 : 2);

		g_BVH[num_box].axis = axis;
		middle = (MaxBuf[0][axis] + MinBuf[0][axis]) / 2;
	}
	GroupMemoryBarrierWithGroupSync();
	/* Split box */

	IDPrimitive p = PrimBuf[groupIndex];
	bool pred = (lid.x < num_primitives)? ((p.position[axis] > middle) ? 1 : 0): 1;
	int destination = Split(pred, groupIndex);
	PrimBuf[destination] = p;

	GroupMemoryBarrierWithGroupSync();
	Primitive rp;
	if (lid.x < num_primitives)
		rp = g_Primitives[PrimBuf[lid.x].id + offet_prim];
	
	GroupMemoryBarrierWithGroupSync();
	if (lid.x < num_primitives)
		g_Primitives[lid.x + offet_prim] = rp;

	/* Set primitives for the two children*/
	if (lid.x == 0)
	{
		if (falseTotal == 1)
		{
			g_BVH[num_box << 1].isLeaf = true;
		}
			
		g_BVH[num_box << 1].numPrimitives = falseTotal;
		g_BVH[num_box << 1].offset = offet_prim;

		if ((num_primitives - falseTotal) == 1)
		{
			g_BVH[(num_box << 1) + 1].isLeaf = true;
		}
			
		g_BVH[(num_box << 1) + 1].numPrimitives = num_primitives - falseTotal;
		g_BVH[(num_box << 1) + 1].offset = offet_prim + falseTotal;
	}
}


[numthreads(NUM_THREAS_PER_GROUP, 1, 1)]
void Postbuild(uint3 id:SV_DispatchThreadID)
{
	int index = (1 << level) + id.x;
	/* if it is invalid, skip */
	if (g_BVH[index].numPrimitives <= 0)
		return;

	if (id.x < (1 << level))
	{
		if (g_BVH[index].isLeaf)
		{
			g_BVH[index].idPrimitive = g_Primitives[g_BVH[index].offset].id;
			g_BVH[index].max = g_Primitives[g_BVH[index].offset].max;
			g_BVH[index].min = g_Primitives[g_BVH[index].offset].min;
		}
		else
		{
			g_BVH[index].max = max(g_BVH[index << 1].max, g_BVH[(index << 1) + 1].max);
			g_BVH[index].min = min(g_BVH[index << 1].min, g_BVH[(index << 1) + 1].min);
		}
	}
	
}