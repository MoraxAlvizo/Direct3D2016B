

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
    float4 Color : COLOR;
};

VERTEX_OUTPUT VSMain(VERTEX_INPUT Input)
{
    VERTEX_OUTPUT Output;

    Output.Position = Input.Position;
    Output.Color = Input.Color;
    Output.TexCoord = Input.TexCoord;

    return Output;
}

cbuffer PARAMS
{
    float4 Delta;
    // Add here more params as requiered
};

Texture2D Frame : register(t0);
SamplerState Sampler : register(s0);
float4 PSEdgeDetect(VERTEX_OUTPUT Input) : SV_Target
{
    float4 left, right, up, down;
    left    = Frame.Sample(Sampler, Input.TexCoord.xy + float2(-Delta.x, 0));
    right   = Frame.Sample(Sampler, Input.TexCoord.xy + float2(Delta.x, 0));
    up      = Frame.Sample(Sampler, Input.TexCoord.xy + float2(0, -Delta.y));
    down    = Frame.Sample(Sampler, Input.TexCoord.xy + float2(0, Delta.y));

    return abs(right - left) + abs(up - down);

    //return float4(1, 0, 0, 0);
    //return Frame.Sample(Sampler, Input.TexCoord.xy) + float4(.25,0,0,0);
}