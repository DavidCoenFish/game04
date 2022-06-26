#pragma once

#include "json/json.hpp"

class VectorFloat4;

//NLOHMANN_DEFINE_TYPE_INTRUSIVE(
//   VectorFloat4
//   );
void to_json(nlohmann::json& j, const VectorFloat4& p);
void from_json(const nlohmann::json& j, VectorFloat4& p);
