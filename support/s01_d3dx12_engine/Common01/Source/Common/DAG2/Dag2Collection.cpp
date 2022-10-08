#include "CommonPCH.h"

#include "Common/DAG2/Dag2Collection.h"
#include "Common/DAG2/iDag2Node.h"

Dag2Collection::Dag2Collection()
{
	return;
}

Dag2Collection::~Dag2Collection()
{
	return;
}

const std::string Dag2Collection::MakeUniqueName()
{
	static int s_count = 0;
	const std::string result = std::string("__defaultName") + std::to_string(s_count++);
	return result;
}

void Dag2Collection::AddNode(const std::string& name, const std::shared_ptr< iDag2Node >& in_dagNode)
{
	auto found = m_dagNodeMap.find(name);
	if (found != m_dagNodeMap.end())
	{
		assert(nullptr == found->second);
	}
	m_dagNodeMap[name] = in_dagNode;
}

void Dag2Collection::RemoveNode(const std::string& name)
{
	auto found = m_dagNodeMap.find(name);
	if (found != m_dagNodeMap.end())
	{
		m_dagNodeMap.erase(found);
	}
	return;
}

iDag2Node* const Dag2Collection::GetNode(const std::string& name) const
{
	auto found = m_dagNodeMap.find(name);
	if (found != m_dagNodeMap.end())
	{
		return found->second.get();
	}
	return nullptr;
}

void Dag2Collection::AddLinkStack(iDag2Node* const pInput, iDag2Node* const pOutput)
{
	if (nullptr != pOutput)
	{
		pOutput->AddInputStack(pInput);
		pOutput->MarkDirty();
	}
	return;
}

void Dag2Collection::RemoveLinkStack(iDag2Node* const pInput, iDag2Node* const pOutput)
{
	if (nullptr != pOutput)
	{
		pOutput->RemoveInputStack(pInput);
		pOutput->MarkDirty();
	}
	return;
}

void Dag2Collection::SetLinkIndex(iDag2Node* const pInputOrNullptr, const int inputIndex, iDag2Node* const pOutput)
{
	if (nullptr != pOutput)
	{
		pOutput->SetInputIndex(pInputOrNullptr, inputIndex);
		pOutput->MarkDirty();
	}
	return;
}
