#pragma once

#include "Common\DAG\iDagValue.h"
#include "Common\DAG\DagCollection.h"

template <typename CONSTUCTION_PARAM, typename... ARGUMENTS>
class DagValueVariant : public iDagValue
{
public:
   static std::shared_ptr< iDagNode > AppendDagCollection(
      DagCollection& dagCollection,
      const std::string& nodeName,
      const CONSTUCTION_PARAM constructionParam
      )
   {
      std::shared_ptr< iDagNode > pResult = std::make_shared< DagValueVariant >( constructionParam );
      dagCollection.AddDagNode( nodeName, pResult );
      return pResult;
   }

   DagValueVariant(const CONSTUCTION_PARAM in_default) : m_variant(in_default) 
   {
      return;
   }

   template < typename TYPE >
   TYPE Get()
   {
      return std::get< TYPE >( m_variant );
   }

   template < typename TYPE >
   void Set( TYPE value )
   {
      m_variant = value;
      return;
   }

   template < typename TYPE >
   void SetRef( const TYPE& value )
   {
      m_variant = value;
      return;
   }

private:
   std::variant<CONSTUCTION_PARAM, ARGUMENTS...> m_variant;
};
