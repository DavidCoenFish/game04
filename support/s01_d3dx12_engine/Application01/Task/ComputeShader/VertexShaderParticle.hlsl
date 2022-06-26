//--------------------------------------------------------------------------------------
// VertexShader.hlsl
//
// Simple vertex shader for rendering a triangle
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

struct Vertex
{
    float2 position     : Position;
};

struct VertexOutput
{
    float4 position     : SV_Position;
    float life        : COLOR0;
};

Texture2D<float> txData : register( t0 );
SamplerState samLinear : register(s0);

VertexOutput main( Vertex input )
{
   float source = txData.SampleLevel( samLinear, input.position, 0 );

   VertexOutput output;
   output.position = float4((input.position.x * 2.0) - 1.0, (input.position.y * 2.0) - 1.0, 0.0, 1.0f);
   output.life = source; //source.w;
   return output;
}