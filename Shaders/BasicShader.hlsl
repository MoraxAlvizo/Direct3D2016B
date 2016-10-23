
// HLSL : High Level Shader Language

/*
	Tipos básicos
	half:16,float:32,int:32,uint:32,bool:32,double:64
	Tipos vectoriales
	half2,half3,half4, float2,float3,float4, ...
	Tipo matricial
	matrix
*/

// Firma de entrada: Establece el formato de vértice que ingresad
// y coincide con el InputLayout que definimos en C.

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
    float4 A:NORMAL1, B:NORMAL2, C:NORMAL3;
    float4 TexCoord : TEXCOORD;
    float4 LightPosition : POSITION1;
    float4 ReflectionVector : TEXCOORD1;
    float4 FogAmount : TEXCOORD2;
};

struct MATERIAL
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
    float4 Power;
};


// Flags
#define LIGHT_ON 0x01
// Types
#define LIGHT_DIRECTIONAL 0 
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

struct LIGHT
{
    uint4 FlagsAndType;
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Attenuation; // 1/(x+(y*d)+(z*(d^2)))
    float4 Position;
    float4 Direction;
    float4 Factors; // x: Power Spotlight

};

#define LIGHTING_AMBIENT            0x0001
#define LIGHTING_DIFFUSE            0x0002
#define LIGHTING_SPECULAR           0x0004
#define LIGHTING_EMISSIVE           0x0008
#define MAPPING_DIFFUSE             0x0010
#define MAPPING_NORMAL              0x0020
#define MAPPING_ENVIROMENTAL_FAST   0x0040
#define MAPPING_NORMAL_TRUE			0x0080
#define MAPPING_EMISSIVE			0x0100
#define MAPPING_SHADOW				0x0200
#define MAPPING_ENVIROMENTAL_SKY	0x0400
#define FOG_ENABLE              	0x0800
#define DRAW_JUST_WITH_COLOR		0x1000

cbuffer PARAMS:register(b0)
{
    uint4 Flags; // Banderas de dibujo e iluminacion
    matrix World; //Model to World Trnasform
    matrix View;
    matrix Projection;
    matrix LightView;
    matrix LightProjection;
    float4 Brightness; //Pixel Shader Brightness control
    float4 CameraPosition;
    MATERIAL Material;
    LIGHT lights[8];
};


VERTEX_OUTPUT VSMain(VERTEX_INPUT Input)
{
	VERTEX_OUTPUT Output;
    matrix WV = mul(World, View);
    matrix WVP = mul(WV, Projection);
    float4 Position = mul(Input.Position, WVP);

    Output.Position = Position;
    Output.PositionNonProjected = mul(Input.Position,WV);
    Output.Normal = mul(Input.Normal, mul(World, View));
	Output.Color = Input.Color;
    Output.TexCoord = Input.TexCoord;

    float4 T, B;
    // Transformar el espacio tangente al espacio de vista
    T = normalize(mul(Input.Tanget, WV));
    B = normalize(mul(Input.Binormal, WV));

    // Transponer la base orthonormal (T,B,N) en espacio de vista,
    // para ir de espacio de modelo a espacio de vista
    Output.A = float4(T.x, B.x, Output.Normal.x, 0);
    Output.B = float4(T.y, B.y, Output.Normal.y, 0);
    Output.C = float4(T.z, B.z, Output.Normal.z, 0);

    Output.LightPosition = mul(Input.Position, mul(mul(World, LightView), LightProjection)  );

    // Sky env 
    float4 worldPosition = mul(Input.Position, World);
    float4 incident = normalize(worldPosition - CameraPosition);
    float4 normal = normalize(mul(float4(Input.Normal), World));

    Output.ReflectionVector = reflect(incident, normal);

    if (Flags.x & FOG_ENABLE)
    {
        float4 viewDirection = CameraPosition - worldPosition;
        Output.FogAmount = saturate((length(viewDirection) - /*fogStart*/10.0f) / (20.0f /*fogRange*/));
    }
        
     
    return Output;

}

Texture2D Diffuse:register(t0);
Texture2D NormalMap : register(t1);
Texture2D EnviromentalMap : register(t2);
Texture2D NormalMapTrue : register(t3);
Texture2D EmissiveMap : register(t4);
Texture2D ShadowMap : register(t5);
TextureCube SkyEnviromentalMap : register(t6);


SamplerState Sampler : register(s0);
SamplerState TriLinearSam : register(s1)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PSMain(VERTEX_OUTPUT Input) :SV_Target
{
    float4 ColorDiffuse = 0;
    float4 Protuberancia = 0;
    float4 ColorEnviomental = 0;
    float4 ColorSpecular = 0;
    float4 ColorEmissive = 0;
    float4 ColorOutput = 0;
    float4 N = normalize(Input.Normal);

	if (Flags.x & DRAW_JUST_WITH_COLOR)
		return Input.Color;

    if (Flags.x & MAPPING_EMISSIVE)
    {
        ColorEmissive = EmissiveMap.Sample(Sampler, Input.TexCoord.xy);
    }
    if (Flags.x & MAPPING_NORMAL)
    {
        Protuberancia = 0.3 * NormalMap.Sample(Sampler, Input.TexCoord.xy);
        Protuberancia.w = 0;
        N = normalize(Input.Normal + Protuberancia);
    }
    if (Flags.x & MAPPING_NORMAL_TRUE)
    {
        //float2 TextCoord =float2(0,1) + Input.TexCoord.xy*float2(1,-1);
        float4 NormalSample = NormalMapTrue.Sample(Sampler, Input.TexCoord.xy) * float4(2, 2, 1, 0) - float4(1, 1, 0, 0);
        Protuberancia.x = dot(Input.A, NormalSample);
        Protuberancia.y = dot(Input.B, NormalSample);
        Protuberancia.z = dot(Input.C, NormalSample);
        Protuberancia.w = 0;
        N = normalize(Protuberancia);
    }

    if (Flags.x & MAPPING_ENVIROMENTAL_FAST)
    {
        ColorEnviomental = EnviromentalMap.Sample(Sampler, (N.xy * float2(0.5, -0.5) + 0.5)  );
    }
    if (Flags.x & MAPPING_ENVIROMENTAL_SKY)
    {
        ColorEnviomental = SkyEnviromentalMap.Sample(TriLinearSam, Input.ReflectionVector.xzy);
    }

    float Shadowed = 1;
    
    if(Flags.x & MAPPING_SHADOW)
    {
        // 1. Calcular la coordenada de textura a partir de la posicion 
        //    interpolada en espacio de luz
        float4 ShadowPos = Input.LightPosition / Input.LightPosition.w;
        ShadowPos.xy = (ShadowPos.xy * float2(0.5, -0.5)) + 0.5;

        if(saturate(ShadowPos.x) == ShadowPos.x && 
           saturate(ShadowPos.y) == ShadowPos.y)
        {   
            float depth = ShadowMap.Sample(Sampler, ShadowPos.xy).r;

            if((ShadowPos.z - depth) > 0.001)
            {
                Shadowed = 0;
            }
        }
    }
    
    for (int i = 0; i < 8; i++)
    {
        if (lights[i].FlagsAndType.x & LIGHT_ON)
        {
            switch(lights[i].FlagsAndType.y)
            {
                case LIGHT_DIRECTIONAL:
                    {
                        float ILambert = max(0, -dot(N, lights[i].Direction));
                        
                        ColorDiffuse += ILambert * lights[i].Diffuse * Shadowed;

                        // Tarea : Calcular la componente specular de la luz direccional
                        // Vector de vista V
                        float4 V = normalize(float4(0, 0, 0, 1) - Input.PositionNonProjected);
                        float4 H = normalize(V - lights[i].Direction);

                        float IPhong = pow(max(0, dot(H, N)), Material.Power.x);
                        ColorSpecular += IPhong * lights[i].Specular * Shadowed;
                        
                    }
                    break;

                // Hacer estos casos de tarea
                case LIGHT_POINT:
                    // L direccion de la luz
                    // N Normal del vertice 
                    // Q es la posicion del vertice
                    // P Posicion de la luz
                    {
                        float d = length(Input.PositionNonProjected - lights[i].Position);
                        float4 L = normalize(Input.PositionNonProjected - lights[i].Position);
                        float ILambert = max(0, -dot(N, L));
                        float AtenuacionPuntual = 1 / dot(lights[i].Attenuation, float4(1, d, d * d, 0));
                        ColorDiffuse += ILambert * AtenuacionPuntual * lights[i].Diffuse;

                        // Vector de vista V
                        float4 V = normalize(float4(0, 0, 0, 1) - Input.PositionNonProjected);
                        float4 H = normalize(V - L);

                        float IPhong = pow(max(0, dot(H, N)), Material.Power.x);
                        ColorSpecular += IPhong * AtenuacionPuntual * lights[i].Specular;
                    }
                    break;
                case LIGHT_SPOT:
                    // L direccion de la luz
                    // N Normal del vertice 
                    // Q es la posicion del vertice
                    // P Posicion de la luz
                    {
                        
                        float d = length(Input.PositionNonProjected - lights[i].Position);
                        float4 L = normalize(Input.PositionNonProjected - lights[i].Position);

                        float AtenuacionSpot = pow(max(0, dot(L, lights[i].Direction)), lights[i].Factors.x);

                        float ILambert = max(0, -dot(N, L));
                        float AtenuacionPuntual = 1 / dot(lights[i].Attenuation, float4(1, d, d * d, 0));
                        ColorDiffuse += ILambert * AtenuacionPuntual * AtenuacionSpot * lights[i].Diffuse;

                        // Vector de vista V
                        float4 V = normalize(float4(0, 0, 0, 1) - Input.PositionNonProjected);
                        float4 H = normalize(V - L);

                        float IPhong = pow(max(0, dot(H, N)), Material.Power.x);
                        ColorSpecular += IPhong * AtenuacionPuntual * AtenuacionSpot * lights[i].Specular;

                        break;
                    }
            }
        }
    }

    if (Flags.x & MAPPING_DIFFUSE)
        ColorDiffuse *= Diffuse.Sample(Sampler, Input.TexCoord.xy);
    
    ColorOutput  = Material.Emissive +
           ColorDiffuse  * Material.Diffuse  + 
           ColorSpecular * Material.Specular +
           ColorEnviomental * Material.Ambient+
           ColorEmissive + 
		   Input.Color +
           Brightness;

    if (Flags.x & FOG_ENABLE)
    {
        ColorOutput = lerp(ColorOutput, float4(0, 0, .1, 0), Input.FogAmount);
    }

    return ColorOutput;

}

struct SHADOW_OUTPUT
{
    float4 Position : SV_Position;
};

SHADOW_OUTPUT VSShadow(VERTEX_INPUT input)
{
    SHADOW_OUTPUT Output;
    Output.Position = mul(input.Position, mul(mul(World, LightView), LightProjection));

    return Output;
}

float PSShadow(SHADOW_OUTPUT Input) : SV_Target
{
    return Input.Position.z;
}