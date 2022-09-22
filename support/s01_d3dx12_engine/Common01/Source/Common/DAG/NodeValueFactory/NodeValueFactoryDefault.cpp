#include "CommonPCH.h"
#include "Common/DAG/NodeValueFactory/NodeValueFactoryDefault.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/DAG/DagNodeCalculate.h"
#include "Common/DAG/iDagNode.h"


void NodeValueFactoryDefault::Append(std::map<std::string, NodeValueFactory>& mapValue)
{
	mapValue["Float"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
		float value = 0.0f;
		data.get_to(value);
		auto pValue = DagValue<float>::Factory(value);
		auto pResult = DagNodeValue::Factory(pValue);
		return pResult;
	};

	mapValue["FloatArray"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
		std::vector<float> value;
		data.get_to(value);
		auto pValue = DagValue<std::vector<float>>::Factory(value);
		auto pResult = DagNodeValue::Factory(pValue);
		return pResult;
	};

	mapValue["Int"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
		int value = 0;
		data.get_to(value);
		auto pValue = DagValue<int>::Factory(value);
		auto pResult = DagNodeValue::Factory(pValue);
		return pResult;
	};

	mapValue["IntArray"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
		std::vector<int> value;
		data.get_to(value);
		auto pValue = DagValue<std::vector<int>>::Factory(value);
		auto pResult = DagNodeValue::Factory(pValue);
		return pResult;
	};



	return;
}
