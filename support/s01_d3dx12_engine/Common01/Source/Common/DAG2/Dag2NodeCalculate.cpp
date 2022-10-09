#include "CommonPCH.h"

#include "Common/DAG2/Dag2NodeCalculate.h"
#include "Common/DAG2/iDag2Node.h"
#include "Common/DAG2/iDag2Value.h"

Dag2NodeCalculate::Dag2NodeCalculate(const CalculateFunction& pCalculateCallback)
	: m_pCalculateCallback(pCalculateCallback)
	, m_dirty(false)
{
	return;
}

Dag2NodeCalculate::~Dag2NodeCalculate()
{
	return;
}

void Dag2NodeCalculate::SetOutput(iDag2Node* const pNode)
{
	if (nullptr != pNode)
	{
		m_arrayOutput.push_back(pNode);
	}
}

void Dag2NodeCalculate::RemoveOutput(iDag2Node* const pNode)
{
	m_arrayOutput.erase(std::remove(m_arrayOutput.begin(), m_arrayOutput.end(), pNode), m_arrayOutput.end());
}

void Dag2NodeCalculate::AddInputStack(iDag2Node* const pNode)
{
	if (nullptr != pNode)
	{
		m_arrayInputStack.push_back(pNode);
		pNode->SetOutput(this);
	}
	return;
}

void Dag2NodeCalculate::RemoveInputStack(iDag2Node* const pNode)
{
	if (nullptr == pNode)
	{
		return;
	}
	pNode->RemoveOutput(this);

	m_arrayInputStack.erase(std::remove(m_arrayInputStack.begin(), m_arrayInputStack.end(), pNode), m_arrayInputStack.end());

	return;
}

void Dag2NodeCalculate::SetInputIndex(iDag2Node* const pNodeOrNullptr, const int index)
{
	assert(0 <= index);
	if (m_arrayInputIndex.size() <= index)
	{
		m_arrayInputIndex.resize((size_t)index + 1);
	}
	{
		iDag2Node* const pBefore = m_arrayInputIndex[index];
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

	return;
}

iDag2Value* Dag2NodeCalculate::GetValue()
{
	if ((true == m_dirty) && (nullptr != m_pCalculateCallback))
	{
		m_dirty = false;
#if 0
		m_pCalculateCallback(
			m_pValue, 
			m_arrayInputStack, 
			m_arrayInputIndex
			);
#else
		std::vector<iDag2Value*> arrayInputStack; 
		for(auto iter: m_arrayInputStack)
		{
			iDag2Value* pValue = iter ? iter->GetValue() : nullptr;
			arrayInputStack.push_back(pValue);
		}
		std::vector<iDag2Value*> arrayInputIndex; 
		for(auto iter: m_arrayInputIndex)
		{
			iDag2Value* pValue = iter ? iter->GetValue() : nullptr;
			arrayInputIndex.push_back(pValue);
		}

		m_pCalculateCallback(
			m_pValue, 
			arrayInputStack, 
			arrayInputIndex
			);

#endif
	}
	return m_pValue.get();
}

void Dag2NodeCalculate::MarkDirty()
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

