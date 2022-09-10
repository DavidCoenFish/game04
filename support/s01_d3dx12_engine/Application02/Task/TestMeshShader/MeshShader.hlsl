struct GeometryInput
{
    float2 positionLeftBottom     : Position;
    float2 positionRightTop       : Position;
    float2 uvLeftBottom           : TEXCOORD0;
    float2 uvRightTop             : TEXCOORD1;
    float4 textureMask            : COLOR0;
};

struct GeometryOutput
{
    float4 position     : Position;
    float4 textureMask  : COLOR0;
    float2 uv           : TEXCOORD0;
};


//void GS_Billboard(triangle GS_INPUT input[3], inout TriangleStream<GS_OUTPUT> OutputStream)
/*

      +1
       .
       .
-1...........+1
       .
       .
      -1

*/
[maxvertexcount(4)]
void GS(point GeometryInput input[1], inout TriangleStream<GeometryOutput> triangleStream)
{
    GeometryOutput vertex;

    //create sprite quad
    //--------------------------------------------

    //bottom left
    vertex.position = float4(input[0].positionLeftBottom.x, input[0].positionLeftBottom.y, 0.0f, 1.0f);
    vertex.textureMask = input[0].textureMask;
    vertex.uv = float2(input[0].uvLeftBottom.x, input[0].uvLeftBottom.y);
    triangleStream.Append(vertex);

    //top left
    vertex.position = float4(input[0].positionLeftBottom.x, input[0].positionRightTop.y, 0.0f, 1.0f);
    vertex.textureMask = input[0].textureMask;
    vertex.uv = float2(input[0].uvLeftBottom.x, input[0].uvRightTop.y);
    triangleStream.Append(vertex);

    //bottom right
    vertex.position = float4(input[0].positionRightTop.x, input[0].positionLeftBottom.y, 0.0f, 1.0f);
    vertex.textureMask = input[0].textureMask;
    vertex.uv = float2(input[0].uvRightTop.x, input[0].uvLeftBottom.y);
    triangleStream.Append(vertex);

    //top right
    vertex.position = float4(input[0].positionRightTop.x, input[0].positionRightTop.y, 0.0f, 1.0f);
    vertex.textureMask = input[0].textureMask;
    vertex.uv = float2(input[0].uvRightTop.x, input[0].uvRightTop.y);
    triangleStream.Append(vertex);
}