#include "CommonPCH.h"

#include "Common/Math/VectorMath.h"


void VectorMath::CrossProduct(float output[3], const float lhs[3], const float rhs[3])
{
	output[0] = (lhs[1] * rhs[2]) - (lhs[2] * rhs[1]);
	output[1] = (lhs[2] * rhs[0]) - (lhs[0] * rhs[2]);
	output[2] = (lhs[0] * rhs[1]) - (lhs[1] * rhs[0]);
	return;
}
