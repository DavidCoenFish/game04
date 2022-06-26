#pragma once

#include "json/json.hpp"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/JSON/JSONDrawSystemEnum.h"
#include "Common/JSON/JSONConstantBufferInfo.h"

//typedef struct DXGI_SAMPLE_DESC
//{
//    UINT Count;
//    UINT Quality;
//} DXGI_SAMPLE_DESC;
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   DXGI_SAMPLE_DESC,
   Count,
   Quality
   );

//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
//   D3D12_RESOURCE_DESC,
//   Dimension,
//   Alignment,
//   Width,
//   Height,
//   DepthOrArraySize,
//   MipLevels,
//   Format,
//   SampleDesc,
//   Layout,
//   Flags
//   );
void to_json(nlohmann::json& j, const D3D12_RESOURCE_DESC& p);
void from_json(const nlohmann::json& j, D3D12_RESOURCE_DESC& p);

//DXGI_FORMAT         Format;
//D3D12_SRV_DIMENSION ViewDimension;
//UINT                Shader4ComponentMapping;
// union D3D12_TEX2D_SRV Texture2D
//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
//   D3D12_SHADER_RESOURCE_VIEW_DESC,
//   Format,
//   ViewDimension,
//   Shader4ComponentMapping
//);
//void from_json(const nlohmann::json& j, D3D12_SHADER_RESOURCE_VIEW_DESC& p);
//struct JSONShaderResourceViewDesc
//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
//   D3D12_SHADER_RESOURCE_VIEW_DESC,
//   Format,
//   ViewDimension,
//   Shader4ComponentMapping,
//   //Buffer,
//   //Texture1D,
//   //Texture1DArray,
//   Texture2D
//   //Texture2DArray,
//   //Texture2DMS,
//   //Texture2DMSArray,
//   //Texture3D,
//   //TextureCube,
//   //TextureCubeArray,
//   //RaytracingAccelerationStructure
//   );

void to_json(nlohmann::json& j, const D3D12_SHADER_RESOURCE_VIEW_DESC& p);
void from_json(const nlohmann::json& j, D3D12_SHADER_RESOURCE_VIEW_DESC& p);

struct JSONShaderResource
{
   D3D12_RESOURCE_DESC desc;
   D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
   std::vector<uint8_t> data;
};
//NLOHMANN_DEFINE_TYPE_INTRUSIVE
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONShaderResource,
   desc,
   shaderResourceViewDesc,
   data
);

