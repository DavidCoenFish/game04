//https://www.3dgep.com/learning-directx-12-4/

#define BLOCK_SIZE 8

Texture2D txInput : register( t0 );
SamplerState samLinear : register(s0);

cbuffer ComputeConstantBuffer : register( b0 )
{
    float2 texelSize; // 1.0 / txOutput0.Dimensions
    float timeDelta;
    float timeAccumulate;
};

RWTexture2D<float> txData0 : register( u0 ); //output particle state, x,y,z,lifespan
RWTexture2D<float> txData1 : register( u1 ); //previous state 0
RWTexture2D<float> txData2 : register( u2 ); //previous state 1

// Gold Noise (c)2015 dcerisano@standard3d.com
// - based on the Golden Ratio
// - uniform normalized distribution
// - fastest static noise generator function (also runs at low precision)
// - use with indicated seeding method. 
float PHI = 1.61803398874989484820459;  // phi = Golden Ratio   
float gold_noise(float2 xy, float seed){
   return frac(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

[numthreads( BLOCK_SIZE, BLOCK_SIZE, 1 )]
void main( int3 id : SV_DispatchThreadID )
{
    //float2 uv = texelSize * ( id.xy + 0.5 );
    //float4 source = txInput.SampleLevel( samLinear, uv, 0 );
    //float sourceAverage = (source.r + source.g + source.b) / 3.0;
    //float rand = gold_noise(uv, frac(timeAccumulate));
    //float4 prevData0 = txData1[id.xy];
    //float4 prevData1 = txData2[id.xy];
    //float4 newData = (float4)0;

    //if ((rand < sourceAverage) && (prevData0.w <= 0.0))
    //{
    //   //emit new
    //   newData = float4(uv.x, uv.y, 0.0, 1.0);
    //}
    //else if(0.0 < prevData0.w) //still alive
    //{
    //   newData.w = prevData0.w - (timeDelta * 0.25);

    //   if (prevData0.w < prevData1.w) //we have continuity with last frame
    //   {
    //      newData.x = prevData0.x + (prevData0.x - prevData1.x);
    //      newData.y = prevData0.y + (prevData0.y - prevData1.y) + (timeDelta * 0.1);
    //      newData.z = prevData0.z + (prevData0.z - prevData1.z);
    //   }
    //   else //no continuity, make up values for velocity
    //   {
    //      float rand1 = gold_noise(uv, rand);
    //      float rand2 = gold_noise(uv, rand1);

    //      newData.x = (rand1 - 0.5) * 0.1;
    //      newData.y = (rand2 - 0.5) * 0.1;
    //      newData.z = rand * timeDelta;
    //   }
    //}

    //txData0[id.xy] = newData.x;
   txData0[id.xy] = 0.5f;
}

