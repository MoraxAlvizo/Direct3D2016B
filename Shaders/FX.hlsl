

struct VERTEX_INPUT
{
    float4 Position : POSITION;
    float4 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

struct VERTEX_OUTPUT
{
    float4 Position : SV_Position;
    float4 TexCoord : TEXCOORD;
    float4 PositionNonProjected : POSITION;
    float4 Color : COLOR;
};



cbuffer PARAMS
{
    float4 Delta;
    float4 RadialBlur;      // x:fuerza
    float4 DirectionalBlur; // xy direccion, z: fuerza
    float4 Umbral;
    matrix WVP;
	float4 Brightness; //Pixel Shader Brightness control
	uint4  flags;
    // Add here more params as requiered
};

VERTEX_OUTPUT VSMain(VERTEX_INPUT Input)
{
	VERTEX_OUTPUT Output;
	float4 Position = mul(Input.Position, WVP);

	Output.Position = Position;
	Output.Color = Input.Color;
	Output.TexCoord = Input.TexCoord;
	Output.PositionNonProjected = Input.Position;


	return Output;
}

Texture2D Frame : register(t0);
Texture2D BrightPassM : register(t1);
Texture2D BrightPassC : register(t2);
Texture2D BrightPassO : register(t3);
TextureCube CubeMap : register(t4);

SamplerState Sampler : register(s0);
SamplerState TriLinearSam : register(s1)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


float4 PSEdgeDetect(VERTEX_OUTPUT Input) : SV_Target
{
    float4 left, right, up, down;
    left    = Frame.Sample(Sampler, Input.TexCoord.xy + float2(-Delta.x, 0));
    right   = Frame.Sample(Sampler, Input.TexCoord.xy + float2(Delta.x, 0));
    up      = Frame.Sample(Sampler, Input.TexCoord.xy + float2(0, -Delta.y));
    down    = Frame.Sample(Sampler, Input.TexCoord.xy + float2(0, Delta.y));

    return (abs(right - left) + abs(up - down));

    //return float4(1, 0, 0, 0);
    //return Frame.Sample(Sampler, Input.TexCoord.xy); //+ float4(.25,0,0,0);
}

float4 PSNone(VERTEX_OUTPUT Input) : SV_Target
{
	return Frame.Sample(Sampler, Input.TexCoord.xy)+ Brightness;
}

float4 PSRadianBlur(VERTEX_OUTPUT Input) : SV_Target
{
    float r = sqrt(dot(Input.PositionNonProjected.xy, Input.PositionNonProjected.xy));
    float2 Dir = normalize(Input.PositionNonProjected.xy);
    float4 Color = 0;
    for (int i = 0; i < 8; i++)
        Color += Frame.Sample(Sampler, Input.TexCoord.xy + RadialBlur.x*r * Dir *i);
    
    Color = Color / 8;
    return Color;

}

float4 PSDirectionalBlur(VERTEX_OUTPUT Input): SV_Target
{
    float4 Color = 0;

    for (int i = 0; i < 8; i++)
        Color += Frame.Sample(Sampler, Input.TexCoord.xy + i * DirectionalBlur.xy*DirectionalBlur.z);

    return Color / 8;

}


float4 PSGaussHorizontalBlur(VERTEX_OUTPUT Input) : SV_Target
{
    const float Gauss[7] = { 0.006, 0.061, 0.242, 0.383, .242, 0.061, 0.006 };
    float4 Color = 0;

    for (int i = -3; i <= 3; i++)
    {
        Color += Frame.Sample(Sampler, Input.TexCoord.xy + Delta.xy * float2(4*i, 0)) * Gauss[i + 3];
    }

    return Color;
}


float4 PSGaussVerticalBlur(VERTEX_OUTPUT Input) : SV_Target
{
    const float Gauss[7] = { 0.006, 0.061, 0.242, 0.383, .242, 0.061, 0.006 };
    float4 Color = 0;

    for (int i = -3; i <= 3; i++)
    {
        Color += Frame.Sample(Sampler, Input.TexCoord.xy + Delta.xy * float2(0, 4*i)) * Gauss[i + 3];
    }

    return Color;
}

float4 PSBrightPass(VERTEX_OUTPUT Input): SV_Target
{
    float4 Color = Frame.Sample(Sampler, Input.TexCoord.xy);
    float L = dot( float4(0.3, 0.5, 0.2, 0),Color);

    return L > Umbral.x ? Color : float4(0,0,0,0);
}

float4 PSMerged(VERTEX_OUTPUT Input) : SV_Target
{
    float4 ColorM = BrightPassM.Sample(Sampler, Input.TexCoord.xy);
    float4 ColorC = BrightPassC.Sample(Sampler, Input.TexCoord.xy);
    float4 ColorO = BrightPassO.Sample(Sampler, Input.TexCoord.xy);
    float4 ColorA = Frame.Sample(Sampler, Input.TexCoord.xy);

    ColorC = (ColorC + ColorO) - (ColorC * ColorO);
    ColorM = (ColorC + ColorM) - (ColorC * ColorM);

    return (ColorA + ColorM) - (ColorA * ColorM);
}


struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 PosL : POSITION;
};

VertexOut VSSky(VERTEX_INPUT vin)
{
    VertexOut vout;
    // Set z = w so that z/w = 1 (i.e., skydome always on far plane).
    vout.PosH = mul(vin.Position, WVP).xyzw;
    // Use local vertex position as cubemap lookup vector.
    vout.PosL = vin.Position;
    return vout;
}

float4 PSSky(VertexOut pin) : SV_Target
{
    return CubeMap.Sample(TriLinearSam, pin.PosL.xzy);
}
