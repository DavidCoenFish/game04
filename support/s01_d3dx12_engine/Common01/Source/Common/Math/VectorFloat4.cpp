#include "CommonPCH.h"
#include "Common/Math/VectorFloat4.h"

VectorFloat4::VectorFloat4(const float x, const float y, const float z, const float w)
   : m_x(x)
   , m_y(y)
   , m_z(z)
   , m_w(w)
{
   return;
}

float& VectorFloat4::operator[](const int index)
{
   switch(index)
   {
   case 0:
      return m_x;
   case 1:
      return m_y;
   case 2:
      return m_z;
   case 3:
      return m_w;
   default:
      break;
   }
   static float s_dummy = 0;
   return s_dummy;
}

const float VectorFloat4::operator[](const int index)const
{
   switch(index)
   {
   case 0:
      return m_x;
   case 1:
      return m_y;
   case 2:
      return m_z;
   case 3:
      return m_w;
   default:
      break;
   }
   return 0;
}

const bool operator==(const VectorFloat4& lhs, const VectorFloat4& rhs)
{
   if (lhs[0] != rhs[0])
   {
      return false;
   }
   if (lhs[1] != rhs[1])
   {
      return false;
   }
   if (lhs[2] != rhs[2])
   {
      return false;
   }
   if (lhs[3] != rhs[3])
   {
      return false;
   }
   return true;
}

