#pragma once

#include "Common/Application/IApplication.h"
class DrawSystem;
class Shader;
class UnorderedAccess;
template<typename VERTEX_TYPE>
class Geometry;
class RenderTargetTexture;
class ShaderResource;

class ApplicationComputeShader : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(const HWND hWnd, const IApplicationParam& applicationParam);

   ApplicationComputeShader(const HWND hWnd, const IApplicationParam& applicationParam);
   virtual ~ApplicationComputeShader();

private:
   virtual void Update() override;
   virtual void OnWindowSizeChanged(const int width, const int height) override;

private:
   std::unique_ptr< DrawSystem > m_pDrawSystem;

   bool m_timePointValid;
   std::chrono::system_clock::time_point m_timePoint;
   float m_timeAccumulate;

   std::shared_ptr< RenderTargetTexture > m_pRenderTargetTexture;

   std::shared_ptr< Shader > m_pShaderTriangleMove;
   struct VertexTriangleMove
   {
      DirectX::XMFLOAT2 m_position;
      DirectX::XMFLOAT4 m_color;
   };
   std::shared_ptr< Geometry< VertexTriangleMove > > m_pGeometryTriangleMove;

   std::shared_ptr< Shader > m_pShaderScreenQuad;
   struct VertexScreenQuad
   {
      DirectX::XMFLOAT2 m_position;
      DirectX::XMFLOAT2 m_uv;
   };
   std::shared_ptr< Geometry< VertexScreenQuad > > m_pGeometryScreenQuad;


   int m_computeDataIndex;
   // Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
#define COMPUTE_DATA_COUNT 8
   std::shared_ptr< UnorderedAccess > m_pComputeData[COMPUTE_DATA_COUNT];
   std::shared_ptr< Shader > m_pShaderCompute;

   //SetCompute32BitConstants
   //Dispatch( Math::DivideByMultiple(dstWidth, 8), Math::DivideByMultiple(dstHeight
   // 
   std::shared_ptr< Shader > m_pShaderParticle;
   struct VertexParticle
   {
      DirectX::XMFLOAT2 m_uv;
   };
   std::shared_ptr< Geometry< VertexParticle > > m_pGeometryParticle;

};