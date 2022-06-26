#pragma once

#include "Common/dag/idagnode.h"

class iDagValue;

class DagNodeCalculate : public iDagNode
{
public:
   typedef std::function< std::shared_ptr< iDagValue >(const std::vector< iDagNode* >&, const std::vector< iDagNode* >&, const std::shared_ptr< iDagValue >&) > CalculateFunction;

   static std::shared_ptr< DagNodeCalculate > Factory(const CalculateFunction& pCalculateCallback);

   //array stack, array indexed, value
   DagNodeCalculate( const CalculateFunction& pCalculateCallback );
   ~DagNodeCalculate();

private:
   virtual void SetValue( const std::shared_ptr< iDagValue >& pValue ) override;
   virtual std::shared_ptr< iDagValue >& GetValue( void ) override;
   virtual void OnMarkDirty() override;
   virtual void StackInputPush(iDagNode* const pNode)  override;
   virtual void OrderedInputSet(const int index, iDagNode* const pNodeOrNullptr)  override;
   virtual void StackInputRemove(iDagNode* const pNode)  override;
   virtual void RemoveAllInput(void) override;
   virtual void SetOutput(iDagNode* const pNode) override;
   virtual void RemoveOutput(iDagNode* const pNode) override;

private:
   std::vector< iDagNode* > m_arrayInputStack;
   std::vector< iDagNode* > m_arrayInputIndex;
   bool m_dirty;
   CalculateFunction m_pCalculateCallback;
   std::shared_ptr< iDagValue > m_pValue;

   std::vector< iDagNode* > m_arrayOutput;
};

