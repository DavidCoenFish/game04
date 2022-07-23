//--------------------------------------------------------------------------------------
// PixelShader.hlsl
//
// Simple shader to render a triangle
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

struct PixelInput
{
    float4 position     : SV_Position;
    float life        : COLOR0;
};

struct PixelOutput
{
    float4 color    : SV_TARGET0;
};

PixelOutput main( PixelInput input )
{
   //if (input.life <= 0.0f)
   //{
   //   discard;
   //   PixelOutput output;
   //   output.color = (float4)0;
   //   return output;
   //}
   PixelOutput output;
   output.color = float4(input.life, input.life, 1.0, 1.0f);
   return output;
}