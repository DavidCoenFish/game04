#pragma once

class iDagValue;

class iDagNode
{
public:
   virtual void SetValue( const std::shared_ptr< iDagValue >& pValue ) = 0;
   virtual std::shared_ptr< iDagValue >& GetValue( void ) = 0;

//protected:
   virtual void OnMarkDirty() = 0;
   virtual void StackInputPush(iDagNode* const pNode) = 0;
   virtual void StackInputRemove(iDagNode* const pNode) = 0;
   virtual void OrderedInputSet(const int index, iDagNode* const pNodeOrNullptr) = 0;
   virtual void RemoveAllInput(void) = 0;
   virtual void SetOutput(iDagNode* const pNode) = 0;
   virtual void RemoveOutput(iDagNode* const pNode) = 0;

};
