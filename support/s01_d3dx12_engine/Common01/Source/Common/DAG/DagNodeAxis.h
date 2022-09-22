#pragma once

#include "common/dag/idagnode.h"

/*
holds a value, sets dependants dirty on set to non zero value
*/

class iDagValue;

class DagNodeAxis : public iDagNode
{
public:
   /*
   */
   static std::shared_ptr< iDagNode > Factory( const std::shared_ptr< iDagValue >& pValue);

   DagNodeAxis( const std::shared_ptr< iDagValue >& pValue);
   ~DagNodeAxis();

private:
   virtual void SetValue( const std::shared_ptr< iDagValue >& pValue ) override;
   virtual std::shared_ptr< iDagValue >& GetValue( void ) override;
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

