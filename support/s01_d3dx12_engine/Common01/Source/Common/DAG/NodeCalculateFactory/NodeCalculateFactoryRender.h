#pragma once

#include "json/json.hpp"

class DrawSystem;
class iDagNode;

namespace NodeCalculateFactoryRender
{
	typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeCalculateFactory;

	void Append(std::map<std::string, NodeCalculateFactory>& mapCalculate, const std::filesystem::path& rootPath, DrawSystem* pDrawSystem);
};
