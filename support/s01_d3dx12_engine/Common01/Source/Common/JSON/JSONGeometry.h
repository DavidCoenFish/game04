#pragma once

#include "json/json.hpp"

struct JSONGeometry
{
   JSONGeometry();
   std::vector< D3D12_INPUT_ELEMENT_DESC > inputElementDesc;
   D3D_PRIMITIVE_TOPOLOGY topology;
   std::vector< float > vertexData;
   int floatPerVertex;
};

void to_json(nlohmann::json& j, const JSONGeometry& p);
void from_json(const nlohmann::json& j, JSONGeometry& p);

