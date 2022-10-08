#pragma once

#include "Common/DAG2/iDag2Node.h"

class iDag2Value;

class Dag2Calculate : public iDag2Node
{
public:
	//array stack, array indexed, value
	typedef std::function< void (
		std::shared_ptr< iDag2Value >&,
		const std::vector< iDag2Node* >&, 
		const std::vector< iDag2Node* >&
		) > CalculateFunction;

	static std::shared_ptr< Dag2Calculate > Factory(
		const CalculateFunction& pCalculateCallback
		)
	{
		auto pResult = std::make_shared< Dag2Calculate >(
			pCalculateCallback
			);

		return pResult;
	}

	Dag2Calculate(const CalculateFunction& pCalculateCallback);
	~Dag2Calculate();

private:
	virtual void SetOutput(iDag2Node* const pNode) override;
	virtual void RemoveOutput(iDag2Node* const pNode) override;

	virtual void AddInputStack(iDag2Node* const pNode) override;
	virtual void RemoveInputStack(iDag2Node* const pNode) override;
	virtual void SetInputIndex(iDag2Node* const pNodeOrNullptr, const int index) override;

	virtual iDag2Value* GetValue() override;

	virtual void MarkDirty() override;

private:
	CalculateFunction m_pCalculateCallback;
	std::vector< iDag2Node* > m_arrayInputStack;
	std::vector< iDag2Node* > m_arrayInputIndex;
	std::vector< iDag2Node* > m_arrayOutput;
	std::shared_ptr< iDag2Value > m_pValue;
	bool m_dirty;

};
