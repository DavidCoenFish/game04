#include "CommonPCH.h"
#include "Common/Application/ApplicationComputeShader.h"
#include "Common/Log/Log.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/Shader/Shader.h"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/DrawSystem/Shader/ConstantBufferInfo.h"
#include "Common/DrawSystem/Shader/ShaderResourceInfo.h"
#include "Common/DrawSystem/Shader/UnorderedAccessInfo.h"
#include "Common/DrawSystem/Shader/UnorderedAccess.h"
#include "Common/DrawSystem/Shader/ShaderResource.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetTexture.h"
#include "Common/FileSystem/FileSystem.h"

IApplication* const ApplicationComputeShader::Factory(const HWND hWnd, const IApplicationParam& applicationParam)
{
   return new ApplicationComputeShader(hWnd, applicationParam);
}

ApplicationComputeShader::ApplicationComputeShader(const HWND hWnd, const IApplicationParam& applicationParam)
   : IApplication(hWnd, applicationParam)
   , m_timePointValid(false)
   , m_timePoint()
   , m_timeAccumulate(0.0f)
   , m_computeDataIndex(0)
{
   LOG_MESSAGE("ApplicationComputeShader  ctor %p", this);
   m_pDrawSystem = std::make_unique< DrawSystem>(
      hWnd, 
      2, //const unsigned int backBufferCount = 2,
      D3D_FEATURE_LEVEL_11_0, //const D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0,
      0, //const unsigned int options = 0,
      RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM, true, VectorFloat4(0.0f,0.0f,0.0f,1.0f)), //const RenderTargetFormatData& targetFormatData = RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM),
      RenderTargetDepthData() //const RenderTargetDepthData& targetDepthData = RenderTargetDepthData()
      );

   auto pCommandList = m_pDrawSystem->CreateCustomCommandList();

   //render target
   {
      std::vector< RenderTargetFormatData > targetFormatDataArray({
         RenderTargetFormatData(DXGI_FORMAT_B8G8R8A8_UNORM, true, VectorFloat4(0.5f,0.5f,0.5f,1.0f))
         });
      RenderTargetDepthData targetDepthData;
      m_pRenderTargetTexture = m_pDrawSystem->MakeRenderTargetTexture(
         pCommandList->GetCommandList(), 
         targetFormatDataArray,
         targetDepthData,
         512,
         512
         );
   }

   //make a moving triangle so we have something fun
   {
      std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDescArray;
      inputElementDescArray.push_back(D3D12_INPUT_ELEMENT_DESC{
         "POSITION",
         0,
         DXGI_FORMAT_R32G32_FLOAT,
         0,
         D3D12_APPEND_ALIGNED_ELEMENT,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         0 // UINT InstanceDataStepRate;
         });
      inputElementDescArray.push_back(D3D12_INPUT_ELEMENT_DESC{
         "COLOR",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         0,
         D3D12_APPEND_ALIGNED_ELEMENT,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         0 // UINT InstanceDataStepRate;
         });

      {
         auto pVertexShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / "VertexShaderTriangleMove.cso");
         auto pPixelShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / "PixelShaderTriangleMove.cso");
         std::vector<DXGI_FORMAT> renderTargetFormat;
         renderTargetFormat.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);

         ShaderPipelineStateData shaderPipelineStateData(
            inputElementDescArray,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            DXGI_FORMAT_UNKNOWN, // DXGI_FORMAT_D32_FLOAT,
            renderTargetFormat,
            CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            CD3DX12_DEPTH_STENCIL_DESC() //CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
            );

         std::vector< std::shared_ptr< ConstantBufferInfo > > arrayShaderConstantsInfo;
         arrayShaderConstantsInfo.push_back(std::make_shared<ConstantBufferInfo>(std::vector< uint8_t >({0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}), D3D12_SHADER_VISIBILITY_VERTEX));
         m_pShaderTriangleMove = m_pDrawSystem->MakeShader(
            pCommandList->GetCommandList(),
            shaderPipelineStateData,
            pVertexShaderData,
            nullptr,
            pPixelShaderData,
            std::vector< std::shared_ptr< ShaderResourceInfo > >(),
            arrayShaderConstantsInfo
            );
      }

      {
         std::vector< VertexTriangleMove > vertexData({
            {{0.0f,0.75f}, {1.0f, 1.0f, 1.0f, 1.0f}}, 
            {{0.5f,-0.5}, {0.0f, 0.0f, 0.0f, 1.0f}},
            {{-0.5f,-0.5}, {0.0f, 0.0f, 0.0f, 1.0f}},
            });
         m_pGeometryTriangleMove = m_pDrawSystem->MakeGeometry(
            pCommandList->GetCommandList(),
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
            inputElementDescArray,
            vertexData
            );
      }
   }

   //make a screen quad
   {
      std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDescArray;
      inputElementDescArray.push_back(D3D12_INPUT_ELEMENT_DESC{
         "POSITION",
         0,
         DXGI_FORMAT_R32G32_FLOAT,
         0,
         D3D12_APPEND_ALIGNED_ELEMENT,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         0 // UINT InstanceDataStepRate;
         });
      inputElementDescArray.push_back(D3D12_INPUT_ELEMENT_DESC{
         "TEXCOORD",
         0,
         DXGI_FORMAT_R32G32_FLOAT,
         0,
         D3D12_APPEND_ALIGNED_ELEMENT,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         0 // UINT InstanceDataStepRate;
         });

      std::vector< VertexScreenQuad > vertexData({
         {{-0.5f,-1.0f}, {1.0f, 1.0f}}, 
         {{-1.0f,-1.0f}, {0.0f, 1.0f}},
         {{-0.5f,-0.5f}, {1.0f, 0.0f}},
         {{-1.0f,-0.5f}, {0.0f, 0.0f}},
         });
      m_pGeometryScreenQuad = m_pDrawSystem->MakeGeometry(
         pCommandList->GetCommandList(),
         D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
         inputElementDescArray,
         vertexData
         );

      auto pVertexShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / "VertexShaderScreenQuad.cso");
      auto pPixelShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / "PixelShaderScreenQuad.cso");
      std::vector<DXGI_FORMAT> renderTargetFormat;
      renderTargetFormat.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);

      std::vector< std::shared_ptr< ShaderResourceInfo > > arrayTexture;
      arrayTexture.push_back( ShaderResourceInfo::FactorySampler( m_pRenderTargetTexture->GetShaderResourceHeapWrapperItem(), D3D12_SHADER_VISIBILITY_PIXEL ) );

      ShaderPipelineStateData shaderPipelineStateData(
         inputElementDescArray,
         D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
         DXGI_FORMAT_UNKNOWN, // DXGI_FORMAT_D32_FLOAT,
         renderTargetFormat,
         CD3DX12_BLEND_DESC(D3D12_DEFAULT),
         CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
         CD3DX12_DEPTH_STENCIL_DESC() //CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
         );

      m_pShaderScreenQuad = m_pDrawSystem->MakeShader(
         pCommandList->GetCommandList(),
         shaderPipelineStateData,
         pVertexShaderData,
         nullptr,
         pPixelShaderData,
         arrayTexture
         );
   }

   //make a particle shader and geometry
   {
      std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDescArray;
      inputElementDescArray.push_back(D3D12_INPUT_ELEMENT_DESC{
         "POSITION",
         0,
         DXGI_FORMAT_R32G32_FLOAT,
         0,
         D3D12_APPEND_ALIGNED_ELEMENT,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         0 // UINT InstanceDataStepRate;
         });

      std::vector< VertexParticle > vertexData;
      for (int y = 0; y < 600; ++y)
      {
         for (int x = 0; x < 800; ++x)
         {
            vertexData.push_back(VertexParticle({{((float)x + 0.5f) / 800.0f, ((float)y + 0.5f) / 600.0f}}));
         }
      }
      m_pGeometryParticle = m_pDrawSystem->MakeGeometry(
         pCommandList->GetCommandList(),
         D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
         inputElementDescArray,
         vertexData
         );

      auto pVertexShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / "VertexShaderParticle.cso");
      auto pPixelShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / "PixelShaderParticle.cso");
      std::vector<DXGI_FORMAT> renderTargetFormat;
      renderTargetFormat.push_back(DXGI_FORMAT_B8G8R8A8_UNORM);

      std::vector< std::shared_ptr< ShaderResourceInfo > > arrayTexture;
      arrayTexture.push_back( ShaderResourceInfo::FactoryDataSampler( nullptr, D3D12_SHADER_VISIBILITY_VERTEX ) );

      ShaderPipelineStateData shaderPipelineStateData(
         inputElementDescArray,
         D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
         DXGI_FORMAT_UNKNOWN, // DXGI_FORMAT_D32_FLOAT,
         renderTargetFormat,
         CD3DX12_BLEND_DESC(D3D12_DEFAULT),
         CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
         CD3DX12_DEPTH_STENCIL_DESC() //CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT)
         );

      m_pShaderParticle = m_pDrawSystem->MakeShader(
         pCommandList->GetCommandList(),
         shaderPipelineStateData,
         pVertexShaderData,
         nullptr,
         pPixelShaderData,
         arrayTexture
         );
   }
   //compute textures
   {
      D3D12_RESOURCE_DESC desc = {
            D3D12_RESOURCE_DIMENSION_TEXTURE2D, //D3D12_RESOURCE_DIMENSION Dimension;
            D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, //UINT64 Alignment;
            800, //UINT64 Width;
            600, //UINT Height;
            1, //UINT16 DepthOrArraySize;
            1, //UINT16 MipLevels;
            DXGI_FORMAT_R32_FLOAT, //DXGI_FORMAT_R32G32B32A32_FLOAT, //DXGI_FORMAT_R8G8B8A8_UNORM, //DXGI_FORMAT Format;
            DXGI_SAMPLE_DESC{ 1, 0 }, //DXGI_SAMPLE_DESC SampleDesc;
            D3D12_TEXTURE_LAYOUT_UNKNOWN, //D3D12_TEXTURE_LAYOUT Layout;
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS //D3D12_RESOURCE_FLAGS Flags;
      };
      // Describe and create a SRV for the texture.
      D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
      uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
      uavDesc.Format = desc.Format;
      uavDesc.Texture2D.MipSlice = 0;
      uavDesc.Texture2D.PlaneSlice = 0;

      for (int index = 0; index < COMPUTE_DATA_COUNT; ++index)
      {
         m_pComputeData[index] = m_pDrawSystem->MakeUnorderedAccess(
            pCommandList->GetCommandList(),
            m_pDrawSystem->MakeHeapWrapperCbvSrvUav(),
            desc,
            uavDesc
            );
      }
   }

   //compute shader
   {
      auto pComputeShaderData = FileSystem::SyncReadFile(applicationParam.m_rootPath / "ComputeShader.cso");

      std::vector< std::shared_ptr< ShaderResourceInfo > > arrayTexture;
      arrayTexture.push_back( ShaderResourceInfo::FactoryDataSampler( m_pRenderTargetTexture->GetShaderResourceHeapWrapperItem(), D3D12_SHADER_VISIBILITY_ALL ) );

      std::vector< std::shared_ptr< ConstantBufferInfo > > arrayShaderConstantsInfo;
      arrayShaderConstantsInfo.push_back(std::make_shared<ConstantBufferInfo>(std::vector< uint8_t >({0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}), D3D12_SHADER_VISIBILITY_ALL));

      std::vector< std::shared_ptr< UnorderedAccessInfo > > arrayUnorderedAccessInfo;
      arrayUnorderedAccessInfo.push_back(std::make_shared<UnorderedAccessInfo>(nullptr, D3D12_SHADER_VISIBILITY_ALL));
      arrayUnorderedAccessInfo.push_back(std::make_shared<UnorderedAccessInfo>(nullptr, D3D12_SHADER_VISIBILITY_ALL));
      arrayUnorderedAccessInfo.push_back(std::make_shared<UnorderedAccessInfo>(nullptr, D3D12_SHADER_VISIBILITY_ALL));

      m_pShaderCompute = m_pDrawSystem->MakeShader(
         pCommandList->GetCommandList(),
         ShaderPipelineStateData::FactoryComputeShader(),
         nullptr,
         nullptr,
         nullptr,
         arrayTexture,
         arrayShaderConstantsInfo,
         pComputeShaderData,
         arrayUnorderedAccessInfo
         );
      m_pShaderCompute->SetDebugName("ShaderCompute");
   }

}

ApplicationComputeShader ::~ApplicationComputeShader ()
{
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WaitForGpu();
   }
   m_pShaderTriangleMove.reset();
   m_pGeometryTriangleMove.reset();
   m_pShaderScreenQuad.reset();
   m_pGeometryScreenQuad.reset();
   m_pRenderTargetTexture.reset();
   m_pShaderParticle.reset();
   m_pGeometryParticle.reset();
   for (int index = 0; index < COMPUTE_DATA_COUNT; ++index)
   {
      m_pComputeData[index].reset();
   }
   m_pShaderCompute.reset();

   m_pDrawSystem.reset();

   LOG_MESSAGE("ApplicationComputeShader dtor %p", this);
}

struct CBuffer
{
   float time;
   float _pad0;
   float _pad1;
   float _pad2;
};

void ApplicationComputeShader ::Update()
{
   BaseType::Update();

   const auto timePointNow = std::chrono::system_clock::now();
   float timeDeltaSeconds = 0.0f;
   if (true == m_timePointValid)
   {
      timeDeltaSeconds = (1.0f / 1000.0f) * (float)(std::chrono::duration_cast<std::chrono::milliseconds>(timePointNow - m_timePoint).count());
   } 
   else
   {
      m_timePointValid = true;
   }
   m_timePoint = timePointNow;
   m_timeAccumulate += timeDeltaSeconds;

   if (m_pDrawSystem)
   {
      m_pShaderTriangleMove->GetConstant<CBuffer>(0).time = m_timeAccumulate * 0.5f;

      auto pFrame = m_pDrawSystem->CreateNewFrame();

      pFrame->SetRenderTarget(m_pRenderTargetTexture.get());
      pFrame->SetShader(m_pShaderTriangleMove.get());
      pFrame->Draw(m_pGeometryTriangleMove.get());

      pFrame->SetRenderTarget(m_pDrawSystem->GetRenderTargetBackBuffer());

      m_pShaderCompute->SetUnorderedAccessViewHandle(0, m_pComputeData[m_computeDataIndex]->GetHeapWrapperItem());
      m_pShaderCompute->SetUnorderedAccessViewHandle(1, m_pComputeData[(m_computeDataIndex + 1) % 3]->GetHeapWrapperItem());
      m_pShaderCompute->SetUnorderedAccessViewHandle(2, m_pComputeData[(m_computeDataIndex + 2) % 3]->GetHeapWrapperItem());

      pFrame->SetShader(m_pShaderCompute.get());
      pFrame->Dispatch( 800 / 8, 600/ 8 );


      m_pShaderParticle->SetShaderResourceViewHandle(0, m_pComputeData[m_computeDataIndex]->GetHeapWrapperItem());
      pFrame->SetShader(m_pShaderParticle.get());
      pFrame->Draw(m_pGeometryParticle.get());

      pFrame->SetShader(m_pShaderScreenQuad.get());
      pFrame->Draw(m_pGeometryScreenQuad.get());

      m_computeDataIndex = (m_computeDataIndex + 1) % 3;
   }
}

void ApplicationComputeShader ::OnWindowSizeChanged(const int width, const int height)
{
   BaseType::OnWindowSizeChanged(width, height);
   if (m_pDrawSystem)
   {
      m_pDrawSystem->OnResize();
   }

   return;
}
