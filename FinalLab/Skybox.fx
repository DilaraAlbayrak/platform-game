//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

TextureCube txSkyColor : register(t1);
SamplerState txSkySampler : register(s1);

cbuffer ConstantBufferSkybox : register(b2)
{
    matrix View;
    matrix Projection;
    float4 eyePos;
    matrix cubeMapWorld;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Pos : POSITION;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 viewDir : TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    float4 inPos = float4(input.Pos, 1.0f);
    float4 worldPos = mul(float4(input.Pos, 1.0f), cubeMapWorld);
    output.viewDir = worldPos.xyz - eyePos.xyz;
    inPos.xyz += eyePos;
    inPos = mul(inPos, View);
    inPos = mul(inPos, Projection);
    output.Pos = inPos;
    
    return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
    return txSkyColor.Sample(txSkySampler, input.viewDir);
}