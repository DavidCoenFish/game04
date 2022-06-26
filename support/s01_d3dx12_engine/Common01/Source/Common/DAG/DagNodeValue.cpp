#include "CommonPCH.h"
#include "Common\dag\dagnodevalue.h"

std::shared_ptr< iDagNode > DagNodeValue::Factory( const std::shared_ptr< iDagValue >& pValue, const bool bMarkDirtyOnSet )
{
   auto pResult = std::make_shared< DagNodeValue >( pValue, bMarkDirtyOnSet );
   return pResult;
}


DagNodeValue::DagNodeValue( const std::shared_ptr< iDagValue >& pValue, const bool bMarkDirtyOnSet ) 
   : m_bMarkDirtyOnSet(bMarkDirtyOnSet)
   , m_pValue( pValue )
{
   return;
}

DagNodeValue::~DagNodeValue()
{
   assert( 0 == m_arrayOutput.size() ); //enforce clean up of links before destruction?
   return;
}

void DagNodeValue::SetValue( const std::shared_ptr< iDagValue >& pValue )
{
   m_pValue = pValue; 
   if (m_bMarkDirtyOnSet)
   {
      OnMarkDirty();
   }
   return; 
}

std::shared_ptr< iDagValue >& DagNodeValue::GetValue( void )
{
   return m_pValue;
}

void DagNodeValue::OnMarkDirty()
{
   for (auto pIter : m_arrayOutput )
   {
      pIter->OnMarkDirty();
   }
}

void DagNodeValue::StackInputPush(iDagNode* const)
{
   assert( false ); //not applicable to value nodes
}

void DagNodeValue::OrderedInputSet(const int, iDagNode* const)
{
   assert( false ); //not applicable to value nodes
}

void DagNodeValue::StackInputRemove(iDagNode* const)
{
   assert( false ); //not applicable to value nodes
}

void DagNodeValue::RemoveAllInput(void)
{
   //assert( false ); //not applicable to value nodes
}

void DagNodeValue::SetOutput(iDagNode* const pNode)
{
   if (nullptr != pNode)
   {
      m_arrayOutput.push_back( pNode );
      //pNode->OnMarkDirty();
   }
}

void DagNodeValue::RemoveOutput(iDagNode* const pNode)
{
   m_arrayOutput.erase(std::remove(m_arrayOutput.begin(), m_arrayOutput.end(), pNode), m_arrayOutput.end()); 
   //if (nullptr != pNode)
   //{
   //   pNode->OnMarkDirty();
   //}
}
