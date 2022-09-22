#include "CommonPCH.h"
#include "Common\dag\DagNodeAxis.h"
#include "Common\dag\iDagValue.h"

std::shared_ptr< iDagNode > DagNodeAxis::Factory(const std::shared_ptr< iDagValue >& pValue)
{
	auto pResult = std::make_shared< DagNodeAxis >(pValue);
	return pResult;
}


DagNodeAxis::DagNodeAxis(const std::shared_ptr< iDagValue >& pValue)
	: m_pValue(pValue)
{
	return;
}

DagNodeAxis::~DagNodeAxis()
{
	assert(0 == m_arrayOutput.size()); //enforce clean up of links before destruction?
	return;
}

void DagNodeAxis::SetValue(const std::shared_ptr< iDagValue >& pValue)
{
	m_pValue = pValue;
	MarkDirty();
	return;
}

std::shared_ptr< iDagValue >& DagNodeAxis::GetValue(void)
{
	return m_pValue;
}

void DagNodeAxis::MarkDirty()
{
	if ((nullptr != m_pValue) && (false == m_pValue->IsZero()))
	{
		for (auto pIter : m_arrayOutput)
		{
			pIter->MarkDirty();
		}
	}
}

void DagNodeAxis::StackInputPush(iDagNode* const)
{
	assert(false); //not applicable to value nodes
}

void DagNodeAxis::OrderedInputSet(const int, iDagNode* const)
{
	assert(false); //not applicable to value nodes
}

void DagNodeAxis::StackInputRemove(iDagNode* const)
{
	assert(false); //not applicable to value nodes
}

void DagNodeAxis::RemoveAllInput(void)
{
	//assert( false ); //not applicable to value nodes
}

void DagNodeAxis::SetOutput(iDagNode* const pNode)
{
	if (nullptr != pNode)
	{
		m_arrayOutput.push_back(pNode);
		//pNode->OnMarkDirty();
	}
}

void DagNodeAxis::RemoveOutput(iDagNode* const pNode)
{
	m_arrayOutput.erase(std::remove(m_arrayOutput.begin(), m_arrayOutput.end(), pNode), m_arrayOutput.end());
	//if (nullptr != pNode)
	//{
	//   pNode->OnMarkDirty();
	//}
}
