//--------------------------------------------------------------------------------------
// not being used as no building is rendered
//--------------------------------------------------------------------------------------

Texture2D txColormap : register(t5);
Texture2D txNormalmap : register(t6);
Texture2D txHeightmap : register(t7);
SamplerState txSampler : register(s3);

cbuffer ConstantBufferCamera : register(b0)
{
    matrix View;
    matrix Projection;
    float4 eyePos;
}

cbuffer ConstantBuffer : register(b4)
{
    matrix World;
}

struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD;
    float3 Norm : NORMAL;
    float3 Tang : TANGENT;
    float3 Binorm : BINORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float3 viewDirInTang : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(float4(input.Pos, 1.0f), World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;
    

    return output;
}
float4 PS(PS_INPUT input) : SV_Target
{
    return txColormap.Sample(txSampler, input.Tex);
}