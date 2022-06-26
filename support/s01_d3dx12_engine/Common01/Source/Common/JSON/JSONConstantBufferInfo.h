#pragma once

#include "json/json.hpp"

struct ConstantBufferInfo;

//void to_json(nlohmann::json& j, const ConstantBufferInfo& p);
//void from_json(const nlohmann::json& j, ConstantBufferInfo& p);
//void from_json(const nlohmann::json& j, std::vector< std::shared_ptr< ConstantBufferInfo > >& p);

struct JSONConstantBufferInfo
{
   JSONConstantBufferInfo()
      : floatCount(0)
      , visiblity(D3D12_SHADER_VISIBILITY_ALL)
   {
      //nop
   }
   int floatCount;
   D3D12_SHADER_VISIBILITY visiblity;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONConstantBufferInfo,
   floatCount,
   visiblity
   );

const std::vector< std::shared_ptr< ConstantBufferInfo > > TransformConstantBufferInfo(const std::vector< JSONConstantBufferInfo >& input); 
