#include "CommonPCH.h"
#include "Common/JSON/JSONVector.h"
#include "Common/Math/VectorFloat4.h"

//#define NLOHMANN_JSON_TO(v1) nlohmann_json_j[#v1] = nlohmann_json_t.v1;
//#define NLOHMANN_JSON_FROM(v1) if(nlohmann_json_j.contains(#v1)){ nlohmann_json_j.at(#v1).get_to(nlohmann_json_t.v1); }

void to_json(nlohmann::json& j, const VectorFloat4& p)
{
   j = nlohmann::json::array({p[0], p[1], p[2], p[3]});
   return;
}

void from_json(const nlohmann::json& j, VectorFloat4& p)
{
   if (j.is_array())
   {
      p[0] = j[0];
      p[1] = j[1];
      p[2] = j[2];
      p[3] = j[3];
   }
   return;
}
