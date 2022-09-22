#include "CommonPCH.h"
#include "Common\DAG\dagcollection.h"
#include "Common\dag\idagnode.h"

DagCollection::DagCollection()
{
   return;
}

DagCollection::~DagCollection()
{
   //unlink
   for (auto iter : m_dagNodeMap)
   {
      if (nullptr != iter.second)
      {
         iter.second->RemoveAllInput();
      }
   }
   m_dagNodeMap.clear();
}

const std::string DagCollection::MakeUniqueName()
{
   static int s_count = 0;
   const std::string result = std::string("__defaultName") + std::to_string(s_count++);
   return result;
}

void DagCollection::AddDagNode(const std::string& name, const std::shared_ptr< iDagNode >& in_dagNode)
{
   auto found = m_dagNodeMap.find( name );
   if (found != m_dagNodeMap.end())
   {
      assert(nullptr == found->second);
   }
   m_dagNodeMap[ name ] = in_dagNode;
}

void DagCollection::RemoveDagNode(const std::string& name)
{
   auto found = m_dagNodeMap.find( name );
   if (found != m_dagNodeMap.end())
   {
      m_dagNodeMap.erase( found );
   }
   return;
}

iDagNode* const DagCollection::GetDagNode(const std::string& name) const
{
   auto found = m_dagNodeMap.find( name );
   if (found != m_dagNodeMap.end())
   {
      return found->second.get();
   }
   return nullptr;
}

void DagCollection::LinkPush( iDagNode* const pInput, iDagNode* const pOutput )
{
   if (nullptr != pOutput)
   {
      pOutput->StackInputPush( pInput );
      pOutput->MarkDirty();
   }
   return;
}

void DagCollection::LinkRemove( iDagNode* const pInput, iDagNode* const pOutput )
{
   if (nullptr != pOutput)
   {
      pOutput->StackInputRemove(pInput);
      pOutput->MarkDirty();
   }
   return;
}

void DagCollection::LinkIndex( const int inputIndex, iDagNode* const pInputOrNullptr, iDagNode* const pOutput )
{
   if (nullptr != pOutput)
   {
      pOutput->OrderedInputSet(inputIndex, pInputOrNullptr);
      pOutput->MarkDirty();
   }
   return;
}
