#include "CommonPCH.h"
#include "Common/JSON/JSONShader.h"
#include "Common/JSON/JSONDrawSystemEnum.h"
#include "Common/JSON/JSONInputElementDesc.h"


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_RENDER_TARGET_BLEND_DESC,
    BlendEnable,
    LogicOpEnable,
    SrcBlend,
    DestBlend,
    BlendOp,
    SrcBlendAlpha,
    DestBlendAlpha,
    BlendOpAlpha,
    LogicOp,
    RenderTargetWriteMask
    );
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_BLEND_DESC,
   AlphaToCoverageEnable,
   IndependentBlendEnable,
   RenderTarget
   );
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_RASTERIZER_DESC,
   FillMode,
   CullMode,
   FrontCounterClockwise,
   DepthBias,
   DepthBiasClamp,
   SlopeScaledDepthBias,
   DepthClipEnable,
   MultisampleEnable,
   AntialiasedLineEnable,
   ForcedSampleCount,
   ConservativeRaster
   );

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_DEPTH_STENCILOP_DESC,
   StencilFailOp,
   StencilDepthFailOp,
   StencilPassOp,
   StencilFunc
   );

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   D3D12_DEPTH_STENCIL_DESC,
   DepthEnable,
   DepthWriteMask,
   DepthFunc,
   StencilEnable,
   StencilReadMask,
   StencilWriteMask,
   FrontFace,
   BackFace
   );

//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
//   D3D12_STATIC_SAMPLER_DESC,
//   Filter,
//   AddressU,
//   AddressV,
//   AddressW,
//   MipLODBias,
//   MaxAnisotropy,
//   ComparisonFunc,
//   BorderColor,
//   MinLOD,
//   MaxLOD,
//   ShaderRegister,
//   RegisterSpace,
//   ShaderVisibility
//   );

void to_json(nlohmann::json& j, const ShaderPipelineStateData& p)
{
   j; p;
}

void from_json(const nlohmann::json& j, ShaderPipelineStateData& p)
{
   if(j.contains("inputElementDesc"))
   { 
      from_json(j.at("inputElementDesc"), p.m_inputElementDescArray); 
   }
   if(j.contains("topologyType"))
   { 
      j.at("topologyType").get_to(p.m_primitiveTopologyType); 
   }
   if(j.contains("depthStencilViewFormat"))
   { 
      j.at("depthStencilViewFormat").get_to(p.m_depthStencilViewFormat); 
   }
   if(j.contains("renderTargetFormat"))
   { 
      j.at("renderTargetFormat").get_to(p.m_renderTargetFormat); 
   }
   if(j.contains("blendState"))
   { 
      j.at("blendState").get_to(p.m_blendState); 
   }
   if(j.contains("rasterizerState"))
   { 
      j.at("rasterizerState").get_to(p.m_rasterizerState); 
   }
   if(j.contains("depthStencilState"))
   { 
      j.at("depthStencilState").get_to(p.m_depthStencilState); 
   }
   return;
}

