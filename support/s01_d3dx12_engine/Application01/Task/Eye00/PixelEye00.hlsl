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

static const float PI = 3.14159265f;

float CalculateIrisDepth(float t, float d, float scale)
{
    return 0.5;
}

float CalculateDepth(float2 uv, float r0, float r1)
{
    float2 coord = (uv * 2.0) - float2(1.0, 1.0);
    float t = ((atan2(coord.y, coord.x)) / (2.0 * PI)) + 0.5;
    float d = length(coord); 
    if (d < r0)
    {
        return 0.0;
    }
    if (r1 < d)
    {
        return 1.0;
    }

    float dn = (d - r0) / (r1 - r0);

    float irisDepth = CalculateIrisDepth(t, dn, r1 - r0);
    return irisDepth;
}

Pixel main( Interpolant input )
{
    float value = CalculateDepth(input.uv, 0.35, 0.9);

    Pixel Out;
    //Out.color = float4(input.uv.x, input.uv.y, 0.0, 1.0);
    Out.color = float4(value, value, value, 1.0);

    return Out;
}