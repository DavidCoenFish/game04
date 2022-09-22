#include "CommonPCH.h"
#include "Common\dag\dagnoderef.h"

std::shared_ptr< iDagNode > DagNodeRef::Factory( const std::shared_ptr< iDagValue >& pValue )
{
   auto pResult = std::make_shared< DagNodeRef >( pValue );
   return pResult;
}


DagNodeRef::DagNodeRef( const std::shared_ptr< iDagValue >& pValue ) 
   : m_pValue( pValue )
{
   return;
}

DagNodeRef::~DagNodeRef()
{
   assert( 0 == m_arrayOutput.size() ); //enforce clean up of links before destruction?
   return;
}

void DagNodeRef::SetValue( const std::shared_ptr< iDagValue >& pValue )
{
   m_pValue = pValue; 
   return; 
}

std::shared_ptr< iDagValue >& DagNodeRef::GetValue( void )
{
   return m_pValue;
}

void DagNodeRef::MarkDirty()
{
	return;
}

void DagNodeRef::StackInputPush(iDagNode* const)
{
   assert( false ); //not applicable to value nodes
}

void DagNodeRef::OrderedInputSet(const int, iDagNode* const)
{
   assert( false ); //not applicable to value nodes
}

void DagNodeRef::StackInputRemove(iDagNode* const)
{
   assert( false ); //not applicable to value nodes
}

void DagNodeRef::RemoveAllInput(void)
{
   //assert( false ); //not applicable to value nodes
}

void DagNodeRef::SetOutput(iDagNode* const pNode)
{
   if (nullptr != pNode)
   {
      m_arrayOutput.push_back( pNode );
      //pNode->OnMarkDirty();
   }
}

void DagNodeRef::RemoveOutput(iDagNode* const pNode)
{
   m_arrayOutput.erase(std::remove(m_arrayOutput.begin(), m_arrayOutput.end(), pNode), m_arrayOutput.end()); 
   //if (nullptr != pNode)
   //{
   //   pNode->OnMarkDirty();
   //}
}
