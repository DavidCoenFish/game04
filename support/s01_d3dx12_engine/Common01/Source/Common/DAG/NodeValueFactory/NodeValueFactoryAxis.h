#pragma once

#include "json/json.hpp"

class DrawSystem;
class iDagNode;
class IUpdate;

namespace NodeValueFactoryAxis
{
	typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeValueFactory;

	void Append(
		std::map<std::string, NodeValueFactory>& mapValue,
		std::vector< std::shared_ptr< IUpdate > >* pUpdatableObjects
		);
};
