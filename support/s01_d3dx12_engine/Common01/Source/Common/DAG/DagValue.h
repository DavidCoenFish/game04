#pragma once

#include "Common\DAG\iDagValue.h"
#include "Common\DAG\iDagNode.h"
#include "Common\DAG\DagCollection.h"

template <typename TYPE>
class DagValue : public iDagValue
{
public:
   static std::shared_ptr< DagValue > Factory(
      const TYPE constructionParam
      )
   {
      std::shared_ptr< DagValue > pResult = std::make_shared< DagValue >( constructionParam );
      return pResult;
   }

   static void UpdateNodeRef(const std::shared_ptr< iDagNode >& pDagNode, const TYPE& value)
   {
      if (nullptr == pDagNode)
      {
         return;
      }
      auto pValue = std::dynamic_pointer_cast<DagValue<TYPE>>(pDagNode->GetValue());
      if (pValue)
      {
         if (pValue->GetRef() == value)
         {
            return;
         }
         pValue->SetRef(value);
         pDagNode->SetValue(pValue);
      }
   }

   static void UpdateNode(const std::shared_ptr< iDagNode >& pDagNode, const TYPE value)
   {
      if (nullptr == pDagNode)
      {
         return;
      }
      auto pValue = std::dynamic_pointer_cast<DagValue<TYPE>>(pDagNode->GetValue());
      if (pValue)
      {
         if (pValue->Get() == value)
         {
            return;
         }
         pValue->Set(value);
         pDagNode->SetValue(pValue);
      }
   }

   static void IncrementNode(const std::shared_ptr< iDagNode >& pDagNode, const TYPE delta)
   {
      if ((nullptr == pDagNode) || (0 == delta))
      {
         return;
      }
      auto pValue = std::dynamic_pointer_cast<DagValue<TYPE>>(pDagNode->GetValue());
      if (pValue)
      {
         const TYPE newValue = pValue->Get() + delta;
         pValue->Set(newValue);
         pDagNode->SetValue(pValue);
      }
   }

   DagValue(const TYPE in_default) : m_value(in_default) 
   {
      return;
   }

   const TYPE Get() const 
   {
      return m_value;
   }

   const TYPE& GetRef() const 
   {
      return m_value;
   }

   void Set( const TYPE value )
   {
      m_value = value;
      return;
   }

   void SetRef( const TYPE& value )
   {
      m_value = value;
      return;
   }

private:
   TYPE m_value;

};
