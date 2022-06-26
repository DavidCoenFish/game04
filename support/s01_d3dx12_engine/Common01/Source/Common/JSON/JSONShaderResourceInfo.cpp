#include "CommonPCH.h"
#include "Common/JSON/JSONShaderResourceInfo.h"
#include "Common/JSON/JSONDrawSystemEnum.h"
#include "Common/DrawSystem/Shader/ShaderResourceInfo.h"

//typedef struct D3D12_STATIC_SAMPLER_DESC
//    {
//    D3D12_FILTER Filter;
//    D3D12_TEXTURE_ADDRESS_MODE AddressU;
//    D3D12_TEXTURE_ADDRESS_MODE AddressV;
//    D3D12_TEXTURE_ADDRESS_MODE AddressW;
//    FLOAT MipLODBias;
//    UINT MaxAnisotropy;
//    D3D12_COMPARISON_FUNC ComparisonFunc;
//    D3D12_STATIC_BORDER_COLOR BorderColor;
//    FLOAT MinLOD;
//    FLOAT MaxLOD;
//    UINT ShaderRegister;
//    UINT RegisterSpace;
//    D3D12_SHADER_VISIBILITY ShaderVisibility;
//    } 	D3D12_STATIC_SAMPLER_DESC;

void to_json(nlohmann::json& j, const D3D12_STATIC_SAMPLER_DESC& p)
{
   j["Filter"] = p.Filter;
   j["AddressU"] = p.AddressU;
   j["AddressV"] = p.AddressV;
   j["AddressW"] = p.AddressW;
   j["MipLODBias"] = p.MipLODBias;
   j["MaxAnisotropy"] = p.MaxAnisotropy;
   j["ComparisonFunc"] = p.ComparisonFunc;
   j["BorderColor"] = p.BorderColor;
   j["MinLOD"] = p.MinLOD;
   j["MaxLOD"] = p.MaxLOD;
   j["ShaderRegister"] = p.ShaderRegister;
   j["RegisterSpace"] = p.RegisterSpace;
   j["ShaderVisibility"] = p.ShaderVisibility;
}

void from_json(const nlohmann::json& j, D3D12_STATIC_SAMPLER_DESC& p)
{
   p = D3D12_STATIC_SAMPLER_DESC{
      D3D12_FILTER_MIN_MAG_MIP_LINEAR, //D3D12_FILTER Filter;
      D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressU;
      D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressV;
      D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressW;
      0, //FLOAT MipLODBias;
      16, //UINT MaxAnisotropy;
      D3D12_COMPARISON_FUNC_LESS_EQUAL, //D3D12_COMPARISON_FUNC ComparisonFunc;
      D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, //D3D12_STATIC_BORDER_COLOR BorderColor;
      0, //FLOAT MinLOD;
      0, //FLOAT MaxLOD;
      0, //UINT ShaderRegister;
      0, //UINT RegisterSpace;
      D3D12_SHADER_VISIBILITY_ALL      //D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
      };

   if (j.contains("Filter"))
   {
      j.at("Filter").get_to(p.Filter); 
   }
   if (j.contains("AddressU"))
   {
      j.at("AddressU").get_to(p.AddressU); 
   }
   if (j.contains("AddressV"))
   {
      j.at("AddressV").get_to(p.AddressV); 
   }
   if (j.contains("AddressW"))
   {
      j.at("AddressW").get_to(p.AddressW); 
   }
   if (j.contains("MipLODBias"))
   {
      j.at("MipLODBias").get_to(p.MipLODBias); 
   }
   if (j.contains("MaxAnisotropy"))
   {
      j.at("MaxAnisotropy").get_to(p.MaxAnisotropy); 
   }
   if (j.contains("ComparisonFunc"))
   {
      j.at("ComparisonFunc").get_to(p.ComparisonFunc); 
   }
   if (j.contains("BorderColor"))
   {
      j.at("BorderColor").get_to(p.BorderColor); 
   }
   if (j.contains("MinLOD"))
   {
      j.at("MinLOD").get_to(p.MinLOD); 
   }
   if (j.contains("MaxLOD"))
   {
      j.at("MaxLOD").get_to(p.MaxLOD); 
   }
   if (j.contains("ShaderRegister"))
   {
      j.at("ShaderRegister").get_to(p.ShaderRegister); 
   }
   if (j.contains("RegisterSpace"))
   {
      j.at("RegisterSpace").get_to(p.RegisterSpace); 
   }
   if (j.contains("ShaderVisibility"))
   {
      j.at("ShaderVisibility").get_to(p.ShaderVisibility); 
   }
}

JSONShaderResourceInfo::JSONShaderResourceInfo()
   : useSampler(false)
   , staticSamplerDesc()
{
}

void to_json(nlohmann::json& j, const JSONShaderResourceInfo& p)
{
   j["useSampler"] = p.useSampler;
   if (p.useSampler)
   {
      j["staticSamplerDesc"] = p.staticSamplerDesc;
   }
}

void from_json(const nlohmann::json& j, JSONShaderResourceInfo& p)
{
   if(j.contains("dataSamplerDesc"))
   {
      p.useSampler = true;
      D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
      if (j.contains("shaderVisibility"))
      {
         j.at("shaderVisibility").get_to(ShaderVisibility); 
      }
      p.staticSamplerDesc =  D3D12_STATIC_SAMPLER_DESC{
         D3D12_FILTER_MIN_MAG_MIP_POINT, //D3D12_FILTER Filter;
         D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressU;
         D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressV;
         D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressW;
         0, //FLOAT MipLODBias;
         16, //UINT MaxAnisotropy;
         D3D12_COMPARISON_FUNC_LESS_EQUAL, //D3D12_COMPARISON_FUNC ComparisonFunc;
         D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, //D3D12_STATIC_BORDER_COLOR BorderColor;
         0, //FLOAT MinLOD;
         0, //FLOAT MaxLOD;
         0, //UINT ShaderRegister;
         0, //UINT RegisterSpace;
         ShaderVisibility      //D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
      };
   }
   else if(j.contains("simpleSamplerDesc"))
   {
      p.useSampler = true;
      D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
      if (j.contains("shaderVisibility"))
      {
         j.at("shaderVisibility").get_to(ShaderVisibility); 
      }
      p.staticSamplerDesc =  D3D12_STATIC_SAMPLER_DESC{
         D3D12_FILTER_MIN_MAG_MIP_LINEAR, //D3D12_FILTER Filter;
         D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressU;
         D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressV;
         D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressW;
         0, //FLOAT MipLODBias;
         16, //UINT MaxAnisotropy;
         D3D12_COMPARISON_FUNC_LESS_EQUAL, //D3D12_COMPARISON_FUNC ComparisonFunc;
         D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, //D3D12_STATIC_BORDER_COLOR BorderColor;
         0, //FLOAT MinLOD;
         0, //FLOAT MaxLOD;
         0, //UINT ShaderRegister;
         0, //UINT RegisterSpace;
         ShaderVisibility      //D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
      };
   }
   else if(j.contains("mipMapSamplerDesc"))
   {
      p.useSampler = true;
      D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
      if (j.contains("shaderVisibility"))
      {
         j.at("shaderVisibility").get_to(ShaderVisibility); 
      }
      p.staticSamplerDesc =  D3D12_STATIC_SAMPLER_DESC{
         D3D12_FILTER_ANISOTROPIC,
         D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressU;
         D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressV;
         D3D12_TEXTURE_ADDRESS_MODE_WRAP, //D3D12_TEXTURE_ADDRESS_MODE AddressW;
         0, //FLOAT MipLODBias;
         16, //UINT MaxAnisotropy;
         D3D12_COMPARISON_FUNC_LESS_EQUAL, //D3D12_COMPARISON_FUNC ComparisonFunc;
         D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, //D3D12_STATIC_BORDER_COLOR BorderColor;
         0, //FLOAT MinLOD;
         D3D12_FLOAT32_MAX, //FLOAT MaxLOD;
         0, //UINT ShaderRegister;
         0, //UINT RegisterSpace;
         ShaderVisibility      //D3D12_SHADER_VISIBILITY_PIXEL //D3D12_SHADER_VISIBILITY ShaderVisibility;
      };
   }
   else if(j.contains("staticSamplerDesc"))
   { 
      p.useSampler = true;
      j.at("staticSamplerDesc").get_to(p.staticSamplerDesc); 
   }
   else
   {
      p.useSampler = false;
   }
}

const std::vector< std::shared_ptr< ShaderResourceInfo > > TransformShaderResourceInfo(const std::vector< JSONShaderResourceInfo >& input)
{
   std::vector< std::shared_ptr< ShaderResourceInfo > > output;
   for (const auto& item : input)
   {
      output.push_back(std::make_shared< ShaderResourceInfo >(
         nullptr,
         item.staticSamplerDesc,
         item.useSampler));
   }
   return output;
}
