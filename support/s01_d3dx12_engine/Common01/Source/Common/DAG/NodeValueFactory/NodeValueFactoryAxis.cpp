#include "CommonPCH.h"
#include "Common/DAG/NodeValueFactory/NodeValueFactoryAxis.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeAxis.h"
#include "Common/DAG/iDagNode.h"
#include "Common/Interface/IUpdate.h"

class JSONAxisKey
{
public:
	int virtualKey;
	float value;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
	JSONAxisKey,
	virtualKey,
	value
);

class NodeValueFactoryAxisKey : public IUpdate
{
public:
	explicit NodeValueFactoryAxisKey(
		const int virtualKey = 0,
		const float value = 0,
		const std::shared_ptr< iDagNode >& pDagNode = nullptr,
		const std::shared_ptr< DagValue<float> >& pDagValue = nullptr
		) 
		: m_virtualKey(virtualKey)
		, m_value(value)
		, m_pDagNode(pDagNode)
		, m_pDagValue(pDagValue)
	{
		return;
	}

private:
	virtual void Update(const float in_timeDeltaSeconds) override
	{
		const SHORT state = GetKeyState(m_virtualKey);
		const bool bDown = (0 != (state & 0x8000));
		//const bool bToggle = (0 != (state & 0x0001));
		const int value = bDown ? m_value : 0.0f;
		if (nullptr != m_pDagValue)
		{
			m_pDagValue->Set(value);
		}
		if (nullptr != m_pDagNode)
		{
			m_pDagNode->MarkDirty();
		}
	}
private:
	int m_virtualKey;
	float m_value;
	std::shared_ptr< iDagNode > m_pDagNode;
	std::shared_ptr< DagValue<float> > m_pDagValue;
};

void NodeValueFactoryAxis::Append(
	std::map<std::string, NodeValueFactory>& mapValue,
	std::vector< std::shared_ptr< IUpdate > >* pUpdatableObjects
	)
{
	mapValue["AxisKey"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
		JSONAxisKey axisKey;
		data.get_to(axisKey);
		auto pValue = DagValue<float>::Factory(0.0f);
		auto pResult = DagNodeAxis::Factory(pValue);
		pUpdatableObjects->push_back(
			std::make_shared<NodeValueFactoryAxisKey>(
				axisKey.virtualKey,
				axisKey.value,
				pResult,
				pValue
			));
		return pResult;
	};

	return;
}
