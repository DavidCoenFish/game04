#include "CommonPCH.h"
#include "Common\dag\dagnodecalculate.h"
#include "Common\dag\idagvalue.h"

std::shared_ptr< DagNodeCalculate > DagNodeCalculate::Factory(const CalculateFunction& pCalculateCallback)
{
	std::shared_ptr< DagNodeCalculate > pResult = std::make_shared< DagNodeCalculate >(pCalculateCallback);
	return pResult;
}

DagNodeCalculate::DagNodeCalculate(const CalculateFunction& pCalculateCallback)
	: m_pCalculateCallback(pCalculateCallback)
	, m_dirty(true)
{
	return;
}

DagNodeCalculate::~DagNodeCalculate()
{
	return;
}

void DagNodeCalculate::SetValue(const std::shared_ptr< iDagValue >&)
{
	assert(false); //not applicable to calculate nodes
}

std::shared_ptr< iDagValue >& DagNodeCalculate::GetValue(void)
{
	if ((true == m_dirty) && (nullptr != m_pCalculateCallback))
	{
		m_dirty = false;
		m_pValue = m_pCalculateCallback(m_arrayInputStack, m_arrayInputIndex, m_pValue);
	}
	return m_pValue;
}

void DagNodeCalculate::MarkDirty()
{
	if (true != m_dirty)
	{
		m_dirty = true;
		for (auto pIter : m_arrayOutput)
		{
			pIter->MarkDirty();
		}
	}
}

void DagNodeCalculate::StackInputPush(iDagNode* const pNode)
{
	if (nullptr != pNode)
	{
		m_arrayInputStack.push_back(pNode);
		pNode->SetOutput(this);
	}
	return;
}

void DagNodeCalculate::OrderedInputSet(const int index, iDagNode* const pNodeOrNullptr)
{
	assert(0 <= index);
	if (m_arrayInputIndex.size() <= index)
	{
		m_arrayInputIndex.resize((size_t)index + 1);
	}
	{
		iDagNode* const pBefore = m_arrayInputIndex[index];
		if (nullptr != pBefore)
		{
			pBefore->RemoveOutput(this);
		}
	}
	m_arrayInputIndex[index] = pNodeOrNullptr;
	if (nullptr != pNodeOrNullptr)
	{
		pNodeOrNullptr->SetOutput(this);
	}
	//else
	//{
	//   // if pNodeOrNullptr, we could have changed our input. pNodeOrNullptr->SetOutput also calls MarkDirty
	//   OnMarkDirty();
	//}

	return;
}

void DagNodeCalculate::StackInputRemove(iDagNode* const pNode)
{
	if (nullptr == pNode)
	{
		return;
	}
	pNode->RemoveOutput(this);

	m_arrayInputStack.erase(std::remove(m_arrayInputStack.begin(), m_arrayInputStack.end(), pNode), m_arrayInputStack.end());

	//we have OrderedInputSet(index, nullptr) if we want to clear indexed input...
#if 0
   //std::remove(m_arrayInputIndex.begin(), m_arrayInputIndex.end(), pNode); 
	for (int index = 0; index < (int)m_arrayInputIndex.size(); ++index)
	{
		if (pNode == m_arrayInputIndex[index])
		{
			m_arrayInputIndex[index] = nullptr;
		}
	}
#endif
	return;
}

void DagNodeCalculate::RemoveAllInput(void)
{
	for (auto pIter : m_arrayInputStack)
	{
		pIter->RemoveOutput(this);
	}
	for (auto pIter : m_arrayInputIndex)
	{
		if (nullptr != pIter)
		{
			pIter->RemoveOutput(this);
		}
	}

	m_arrayInputStack.clear();
	m_arrayInputIndex.clear();

	return;
}

void DagNodeCalculate::SetOutput(iDagNode* const pNode)
{
	if (nullptr != pNode)
	{
		m_arrayOutput.push_back(pNode);
		//pNode->OnMarkDirty();
	}
}

void DagNodeCalculate::RemoveOutput(iDagNode* const pNode)
{
	m_arrayOutput.erase(std::remove(m_arrayOutput.begin(), m_arrayOutput.end(), pNode), m_arrayOutput.end());
	//if (nullptr != pNode)
	//{
	//   pNode->OnMarkDirty();
	//}
}
