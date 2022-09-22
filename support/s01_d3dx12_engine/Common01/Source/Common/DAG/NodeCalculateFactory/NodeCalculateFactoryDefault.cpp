#include "CommonPCH.h"
#include "Common/DAG/NodeCalculateFactory/NodeCalculateFactoryDefault.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/DAG/DagNodeCalculate.h"
#include "Common/DAG/iDagNode.h"


void NodeCalculateFactoryDefault::Append(std::map<std::string, NodeCalculateFactory>& mapCalculate)
{
	//stackInput [float]
	//output std::vector< float >
	mapCalculate["StackFloat"] = MakeStackFactory<float>();

	//stackInput [std::vector< float >]
	//output std::vector< std::vector< float > >
	mapCalculate["StackVectorFloat"] = MakeStackFactory<std::vector< float >>();

	//stackInput [std::vector< float >]
	//output float
	mapCalculate["IndexFloat"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
		int index = 0;
		data.get_to(index);
		auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue > {
			stackInput; indexInput; pValue;
			auto pLocalValue = std::dynamic_pointer_cast<DagValue< float >>(pValue);
			if (nullptr == pLocalValue)
			{
				pLocalValue = DagValue< float >::Factory(0);
			}

			const auto pDag = 0 < indexInput.size() ? std::dynamic_pointer_cast<DagValue< std::vector< float > >>(indexInput[0]->GetValue()) : nullptr;
			if ((nullptr != pDag) && (0 <= index) && (index < (int)pDag->GetRef().size()))
			{
				pLocalValue->Set( pDag->GetRef()[index] );
			}
			return pLocalValue;
		});
		return pResult;
	};

	return;
}
