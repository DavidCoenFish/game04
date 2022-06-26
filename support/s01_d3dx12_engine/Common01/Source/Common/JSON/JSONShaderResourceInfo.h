#pragma once

#include "json/json.hpp"

struct ShaderResourceInfo;

void to_json(nlohmann::json& j, const D3D12_STATIC_SAMPLER_DESC& p);
void from_json(const nlohmann::json& j, D3D12_STATIC_SAMPLER_DESC& p);
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

struct JSONShaderResourceInfo
{
   JSONShaderResourceInfo();
   D3D12_STATIC_SAMPLER_DESC staticSamplerDesc;
   bool useSampler;
};
void to_json(nlohmann::json& j, const JSONShaderResourceInfo& p);
void from_json(const nlohmann::json& j, JSONShaderResourceInfo& p);

//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
//   JSONShaderResourceInfo,
//   staticSamplerDesc
//   );

const std::vector< std::shared_ptr< ShaderResourceInfo > > TransformShaderResourceInfo(const std::vector< JSONShaderResourceInfo >& input); 
