#pragma once

#include "json/json.hpp"

class iDagNode;

namespace NodeValueFactoryDefault
{
	typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeValueFactory;

	void Append(std::map<std::string, NodeValueFactory>& mapValue);
};
