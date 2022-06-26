#pragma once

class VectorFloat4
{
public:
   explicit VectorFloat4(const float x = 0, const float y = 0, const float z = 0, const float w = 0);

   float& operator[](const int index);
   const float operator[](const int index)const;

private:
   float m_x;
   float m_y;
   float m_z;
   float m_w;
};

const bool operator==(const VectorFloat4& lhs, const VectorFloat4& rhs);

