#pragma once

#include "json/json.hpp"
#include "Common/DAG/iDagNode.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeCalculate.h"

class iDagNode;

namespace NodeCalculateFactoryDefault
{
	typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeCalculateFactory;

	void Append(std::map<std::string, NodeCalculateFactory>& mapCalculate);

	template <typename TYPE>
	NodeCalculateFactory MakeStackFactory()
	{
		return [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
			data;
			auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue > {
				stackInput; indexInput; pValue;
				auto pLocalValue = std::dynamic_pointer_cast<DagValue< std::vector< TYPE > >>(pValue);
				if (nullptr == pLocalValue)
				{
					pLocalValue = DagValue< std::vector< TYPE > >::Factory(std::vector< TYPE >());
				}
				std::vector< TYPE > resultData;
				for (const auto& item : stackInput)
				{
					auto pTemp = item ? std::dynamic_pointer_cast<DagValue< TYPE >>(item->GetValue()) : nullptr;
					if (nullptr == pTemp)
					{
						continue;
					}
					resultData.push_back(pTemp->GetRef());
				}
				pLocalValue->SetRef(resultData);
				return pLocalValue;
				});
			return pResult;
		};
	}

};
