#pragma once

#include "json/json.hpp"

//void to_json(nlohmann::json& j, const std::vector<D3D12_INPUT_ELEMENT_DESC>& p);
void from_json(const nlohmann::json& j, std::vector<D3D12_INPUT_ELEMENT_DESC>& p);

//std::vector<D3D12_INPUT_ELEMENT_DESC> JSONInputElementDesc_from_json(
//   const nlohmann::json& json
//   );
