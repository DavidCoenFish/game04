#include "CommonPCH.h"
#include "Common/JSON/JSONGeometry.h"
#include "Common/JSON/JSONDrawSystemEnum.h"
#include "Common/JSON/JSONInputElementDesc.h"

JSONGeometry::JSONGeometry()
   : topology(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED)
   , floatPerVertex(0)
{
   //nop
}

void to_json(nlohmann::json& j, const JSONGeometry& p)
{
   //j["inputElementDesc"] = p.inputElementDesc;
   j["topology"] = p.topology;
   j["vertexData"] = p.vertexData;
   j["floatPerVertex"] = p.floatPerVertex;
}

void from_json(const nlohmann::json& j, JSONGeometry& p)
{
   if(j.contains("inputElementDesc"))
   { 
      from_json(j.at("inputElementDesc"), p.inputElementDesc); 
   }
   if(j.contains("topology"))
   { 
      j.at("topology").get_to(p.topology); 
   }
   if(j.contains("vertexData"))
   { 
      j.at("vertexData").get_to(p.vertexData); 
   }
   if(j.contains("floatPerVertex"))
   { 
      j.at("floatPerVertex").get_to(p.floatPerVertex); 
   }
}
