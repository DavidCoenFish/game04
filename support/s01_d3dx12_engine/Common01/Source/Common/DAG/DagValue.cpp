#include "CommonPCH.h"
#include "Common\dag\dagvalue.h"


//template <>
//void TClass<int>::doSomething(std::vector<int> * v) {
// // Do somtehing with a vector of int's
//}

template <>
const bool DagValue<std::vector<float>>::IsZero() const
{
	return (0 == m_value.size());
}

template <>
const bool DagValue<std::vector<std::vector<float>>>::IsZero() const
{
	return (0 == m_value.size());
}

template <>
const bool DagValue<std::vector<int>>::IsZero() const
{
	return (0 == m_value.size());
}

template <>
const bool DagValue<std::vector<RenderPairData>>::IsZero() const
{
	return (0 == m_value.size());
}

template <>
const bool DagValue<RenderPairData>::IsZero() const
{
	return false;
}

template <>
const bool DagValue<std::vector<std::shared_ptr<GeometryGeneric>>>::IsZero() const
{
	return (0 == m_value.size());
}

template <>
const bool DagValue<std::vector<std::shared_ptr<HeapWrapperItem>>>::IsZero() const
{
	return (0 == m_value.size());
}
