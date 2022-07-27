struct Vertex
{
    float2 positionLeftBottom     : Position;
    float2 positionRightTop       : TEXCOORD0;
    float2 uvLeftBottom           : TEXCOORD1;
    float2 uvRightTop             : TEXCOORD2;
    float4 textureMask            : COLOR0;
};

struct VertexOutput
{
    float2 positionLeftBottom     : Position;
    float2 positionRightTop       : TEXCOORD0;
    float2 uvLeftBottom           : TEXCOORD1;
    float2 uvRightTop             : TEXCOORD2;
    float4 textureMask            : COLOR0;
};

VertexOutput main( Vertex input )
{
   VertexOutput output;
   output.positionLeftBottom = input.positionLeftBottom;
   output.positionRightTop = input.positionRightTop;
   output.textureMask = input.textureMask;
   output.uvLeftBottom = input.uvLeftBottom;
   output.uvRightTop = input.uvRightTop;
   return output;
}