//--------------------------------------------------------------------------------------
// VertexShader.hlsl
//
// Simple vertex shader for rendering a triangle
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
//#pragma target 5.0

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
//SamplerState samLinear : register(s0);

VertexOutput main( Vertex input )
{
   //int2 pos_xy = { input.position.x, input.position.y };
   float source = 0.0; //txData[pos_xy];

   VertexOutput output;
   output.position = float4((input.position.x * 2.0) - 1.0, (input.position.y * 2.0) - 1.0, 0.0, 1.0f);
   output.life = source; //source.w;
   return output;
}