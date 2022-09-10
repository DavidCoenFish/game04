struct Interpolant
{
    float4 position     : Position;
    float4 textureMask  : COLOR0;
    float2 uv           : TEXCOORD0;
};

struct Pixel
{
    float4 color    : SV_TARGET0;
};

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

Pixel main( Interpolant In )
{
    float4 textureColor = texture0.Sample(sampler0, In.uv);
    float alpha = dot(textureColor, In.textureMask);

    Pixel Out;
    Out.color = float4(1.0f, 1.0f, 1.0f, alpha);
    return Out;
}