#include "Polar.hlsli"

cbuffer VertexConstants : register(b0)
{
    float4 m_cameraXYZNear;
    float4 m_cameraAtFar;
    float4 m_cameraUpLineWidth;
    float4 m_viewportWidthHeightHorizontalFOVDegreeVerticalFOVDegree;
};

struct Interpolant
{
    float4 position     : SV_Position;
    float2 uv           : TEXCOORD0;
};

struct Pixel
{
    float4 color    : SV_TARGET0;
};

float GetPixelSizeAtDepth(float depth)
{
   float fovhRadians = radians(m_viewportWidthHeightHorizontalFOVDegreeVerticalFOVDegree.z);
   float pixelSize = m_cameraUpLineWidth.w * fovhRadians * depth / (m_viewportWidthHeightHorizontalFOVDegreeVerticalFOVDegree.x);
   return pixelSize;
}

//http://geomalgorithms.com/a07 0 distance.html
float2 FindRayRayClosest(float3 u, float3 posU, float3 v, float3 posV, float maxDistance)
{
   float2 result = float2(maxDistance, 0.0f);
   float3 w = posU - posV;
   float a = dot(u, u);
   float b = dot(u, v);
   float c = dot(v, v);
   float ACminBB = (a * c) - (b * b);
   if (0.0001 <= abs(ACminBB))
   {
      float d = dot(u, w);
      float e = dot(v, w);
      result.x = ((b * e) - (c * d))/ ACminBB;
      result.y = ((a * e) - (b * d))/ ACminBB;
   }
   return result;
}

float AlphaFromAxisLine(float3 axis, float3 ray, float3 pos, float near, float far)
{
   float result = 0.0f;
   float2 scTc = FindRayRayClosest(ray, pos, axis, float3(0.0f, 0.0f, 0.0f), far);
   if ((near < scTc.x) && (scTc.x < far))
   {
      float pixelSize = GetPixelSizeAtDepth(scTc.x);
      float pixelSizeSquared = pixelSize * pixelSize;
      float3 offset = (pos + (ray * scTc.x)) - (axis * scTc.y);
      float distanceSquared = dot(offset, offset);
      if (distanceSquared <= pixelSizeSquared)
      {
         float alpha = 1.0f - (distanceSquared / pixelSizeSquared);
         //float distanceFromFar = far - scTc.x;
         result = max(0.0, alpha);
      }
   }
   return result;
}

float AlphaFromPlane(float4 plane, float3 crossA, float3 crossB, float3 ray, float3 pos, float near, float far)
{
   float result = 0.0f;
   float ln = dot(ray, plane.xyz);
   if (0.0001 < abs(ln))
   {
      float t = (plane.w - dot(pos, plane.xyz)) / ln;
      if ((near < t) && (t < (far / 3.0f)))
      {
         float pixelSize = GetPixelSizeAtDepth(t) * 3.0;
         float pixelSizeSquared = pixelSize * pixelSize;
         float3 testPos = fmod(pos + (ray * t), 2.0f);
         float a = abs(dot(crossA, testPos));
         float b = abs(dot(crossB, testPos));
         float c = min(a, b);
         float distanceSquared = c * c;
         if (distanceSquared < pixelSizeSquared)
         {
            float alpha = (1.0f - (distanceSquared / pixelSizeSquared)) * (1.0f - (t * 3.0 / far)) * 0.5;
            //float distanceFromFar = far - t;
            result = max(0.0, alpha);
         }
      }
   }
   return result;
}

    //float4 m_cameraXYZNear;
    //float4 m_cameraAtFar;
    //float4 m_cameraUp;
    //float4 m_viewportWidthHeightHorizontalFOVDegreeVerticalFOVDegree;

float3 MakeWorldRay(float3 screenEyeRay, float3 cameraRight, float3 cameraUp, float3 cameraAt)
{
   return (screenEyeRay.x * cameraRight) + (screenEyeRay.y * cameraUp) + (screenEyeRay.z * cameraAt);
}

Pixel main( Interpolant input )
{
   float2 screenRadians = (input.uv.xy - 0.5f) * radians(m_viewportWidthHeightHorizontalFOVDegreeVerticalFOVDegree.zw);
   float3 screenEyeRay = PolarToVector(screenRadians);
   float3 cameraRight = cross(m_cameraUpLineWidth.xyz, m_cameraAtFar.xyz);
   float3 worldRay = MakeWorldRay(screenEyeRay, cameraRight, m_cameraUpLineWidth.xyz, m_cameraAtFar.xyz);

   float alpha = 0.0;
   alpha += AlphaFromAxisLine(float3(1.0f, 0.0f, 0.0f), worldRay, m_cameraXYZNear.xyz, m_cameraXYZNear.w, m_cameraAtFar.w);
   alpha += AlphaFromAxisLine(float3(0.0f, 1.0f, 0.0f), worldRay, m_cameraXYZNear.xyz, m_cameraXYZNear.w, m_cameraAtFar.w);
   alpha += AlphaFromAxisLine(float3(0.0f, 0.0f, 1.0f), worldRay, m_cameraXYZNear.xyz, m_cameraXYZNear.w, m_cameraAtFar.w);
   //alpha += AlphaFromPlane(float4(0.0f, 1.0f, 0.0f, 0.0f), float3(1.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f), worldRay, m_cameraXYZNear.xyz, m_cameraXYZNear.w, m_cameraAtFar.w);
   alpha = min(1.0, alpha);

   //if (alpha <= 0.0f)
   //{
   //   discard;
   //}

   Pixel output = (Pixel)0;
   output.color = float4(alpha, alpha, alpha, alpha);
  // output.color.xyz = worldRay;
   return output;
}