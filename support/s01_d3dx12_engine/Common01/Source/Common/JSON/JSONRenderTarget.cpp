#include "CommonPCH.h"
#include "Common/JSON/JSONRenderTarget.h"

#include "Common/DrawSystem/RenderTarget/RenderTargetFormatData.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetDepthData.h"
#include "Common/JSON/JSONDrawSystemEnum.h"

//#define NLOHMANN_JSON_TO(v1) nlohmann_json_j[#v1] = nlohmann_json_t.v1;
//#define NLOHMANN_JSON_FROM(v1) if(nlohmann_json_j.contains(#v1)){ nlohmann_json_j.at(#v1).get_to(nlohmann_json_t.v1); }

void to_json(nlohmann::json& j, const RenderTargetFormatData& p)
{
   j["format"] = p.format;
   j["clearColor"] = p.clearColor;
   j["clearDepthOnSet"] = p.clearOnSet;
}
void from_json(const nlohmann::json& j, RenderTargetFormatData& p)
{
   if(j.contains("format"))
   { 
      j.at("format").get_to(p.format); 
   }
   if(j.contains("clearColor"))
   { 
      j.at("clearColor").get_to(p.clearColor); 
      if(j.contains("clearOnSet"))
      { 
         j.at("clearOnSet").get_to(p.clearOnSet); 
      }
      else
      {
         p.clearOnSet = true;
      }
   }
}

void to_json(nlohmann::json& j, const RenderTargetDepthData& p)
{
   j["format"] = p.format;
   j["clearDepthOnSet"] = p.clearDepthOnSet;
   j["clearDepth"] = p.clearDepth;
   j["clearStencilOnSet"] = p.clearStencilOnSet;
   j["clearStencil"] = p.clearStencil;
   j["shaderResource"] = p.shaderResource;
}

void from_json(const nlohmann::json& j, RenderTargetDepthData& p)
{
   if(j.contains("format"))
   { 
      j.at("format").get_to(p.format); 
   }
   if(j.contains("clearDepth"))
   { 
      j.at("clearDepth").get_to(p.clearDepth); 
      if(j.contains("clearDepthOnSet"))
      { 
         j.at("clearDepthOnSet").get_to(p.clearDepthOnSet); 
      }
      else
      {
         p.clearDepthOnSet = true;
      }
   }
   if(j.contains("clearStencil"))
   { 
      j.at("clearStencil").get_to(p.clearStencil); 
      if(j.contains("clearStencilOnSet"))
      { 
         j.at("clearStencilOnSet").get_to(p.clearStencilOnSet); 
      }
      else
      {
         p.clearStencilOnSet = true;
      }
   }
   if(j.contains("shaderResource"))
   { 
      j.at("shaderResource").get_to(p.shaderResource); 
   }

   return;
}

