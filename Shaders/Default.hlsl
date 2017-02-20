

// Define el numero y estructura del grupo
// Grupo, es un equipo de hilos que comparten variables globales
struct VECTOR
{
	float x, y, z, w;
};

StructuredBuffer<VECTOR> Input;
RWStructuredBuffer<VECTOR> Output;

int x;
[numthreads(1,1,1)]
void main(uint3 id:SV_DispatchThreadID)
{

}