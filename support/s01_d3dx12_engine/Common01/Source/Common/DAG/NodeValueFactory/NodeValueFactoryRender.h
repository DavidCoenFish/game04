#pragma once

#include "json/json.hpp"

class DrawSystem;
class iDagNode;

namespace NodeValueFactoryRender
{
	typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeValueFactory;

	void Append(std::map<std::string, NodeValueFactory>& mapValue, DrawSystem* pDrawSystem);
};
