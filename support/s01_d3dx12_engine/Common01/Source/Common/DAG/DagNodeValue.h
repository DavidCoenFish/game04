#pragma once

#include "common/dag/idagnode.h"

/*
combine DagValue and DagNodeValue, or would that result in too much in the resultant class
DagNodeValue holds the DagValue and deals with the links/ dirty

DagNodeValue holds a value, is set dirty on value change, sets outputs dirty
*/

class iDagValue;

class DagNodeValue : public iDagNode
{
public:
	/*
	bMarkDirtyOnSet==false is used for passing in a ref to DrawSystemFrame* (interface to set render target, draw things for the current frame)
	*/
	static std::shared_ptr< iDagNode > Factory(const std::shared_ptr< iDagValue >& pValue);

	DagNodeValue(const std::shared_ptr< iDagValue >& pValue);
	~DagNodeValue();

private:
	virtual void SetValue(const std::shared_ptr< iDagValue >& pValue) override;
	virtual std::shared_ptr< iDagValue >& GetValue(void) override;
	virtual void MarkDirty() override;
	virtual void StackInputPush(iDagNode* const pNode)  override;
	virtual void OrderedInputSet(const int index, iDagNode* const pNodeOrNullptr)  override;
	virtual void StackInputRemove(iDagNode* const pNode)  override;
	virtual void RemoveAllInput(void) override;
	virtual void SetOutput(iDagNode* const pNode) override;
	virtual void RemoveOutput(iDagNode* const pNode) override;

private:
	std::vector< iDagNode* > m_arrayOutput;
	std::shared_ptr< iDagValue > m_pValue;
};

