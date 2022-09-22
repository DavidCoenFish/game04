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
		std::shared_ptr< DagValue > pResult = std::make_shared< DagValue >(constructionParam);
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

	static TYPE& GetRefForWrite(const std::shared_ptr< iDagNode >& pDagNode, const TYPE value)
	{
		static TYPE s_dummy;
		if (nullptr == pDagNode)
		{
			return s_dummy;
		}
		auto pValue = std::dynamic_pointer_cast<DagValue<TYPE>>(pDagNode->GetValue());
		if (nullptr == pValue)
		{
			return s_dummy;
		}
		pDagNode->MarkDirty();
		return pValue->m_value;
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

	void Set(const TYPE value)
	{
		m_value = value;
		return;
	}

	void SetRef(const TYPE& value)
	{
		m_value = value;
		return;
	}

private:
	virtual const bool IsZero() const override
	{
		return (0 == m_value);
	}


private:
	TYPE m_value;

};

//template <typename TYPE>
//const bool DagValue<std::vector<TYPE>>::IsZero() const
//{
//	return (0 == m_value.size());
//}

template <>
const bool DagValue<std::vector<float>>::IsZero() const;
template <>
const bool DagValue<std::vector<std::vector<float>>>::IsZero() const;
template <>
const bool DagValue<std::vector<int>>::IsZero() const;

class Shader;
class GeometryGeneric;
typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > > RenderPairData;
template <>
const bool DagValue<std::vector<RenderPairData>>::IsZero() const;
template <>
const bool DagValue<RenderPairData>::IsZero() const;

template <>
const bool DagValue<std::vector<std::shared_ptr<GeometryGeneric>>>::IsZero() const;

class HeapWrapperItem; //std::shared_ptr< HeapWrapperItem >
template <>
const bool DagValue<std::vector<std::shared_ptr< HeapWrapperItem >>>::IsZero() const;
