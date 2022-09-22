#pragma once

#include "common/dag/idagnode.h"

/*
holds a value, does not set dependants dirty on set
*/

class iDagValue;

class DagNodeRef : public iDagNode
{
public:
   /*
   */
   static std::shared_ptr< iDagNode > Factory( const std::shared_ptr< iDagValue >& pValue);

   DagNodeRef( const std::shared_ptr< iDagValue >& pValue);
   ~DagNodeRef();

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

