//--------------------------------------------------------------------------------------
// PixelShader.hlsl
//
// Simple shader to render a triangle
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

struct Interpolant
{
    float4 position     : SV_Position;
    float2 uv           : TEXCOORD0;
};

struct Pixel
{
    float4 color    : SV_TARGET0;
};

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

Pixel main( Interpolant input )
{
    Pixel output;
    output.color = txDiffuse.Sample(samLinear, input.uv);
    //output.color = float4(input.uv.y, 0.0, 0.0, 1.0);
    return output;
}