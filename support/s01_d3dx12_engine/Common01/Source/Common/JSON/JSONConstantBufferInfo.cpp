#include "CommonPCH.h"
#include "Common/JSON/JSONConstantBufferInfo.h"
#include "Common/JSON/JSONDrawSystemEnum.h"
#include "Common/DrawSystem/Shader/ConstantBufferInfo.h"

const std::vector< std::shared_ptr< ConstantBufferInfo > > TransformConstantBufferInfo(const std::vector< JSONConstantBufferInfo >& input)
{
   std::vector< std::shared_ptr< ConstantBufferInfo > > output;
   for (const auto& item : input)
   {
      std::vector< uint8_t > data;
      data.resize(sizeof(float) * item.floatCount);
      output.push_back(std::make_shared< ConstantBufferInfo >(data, item.visiblity));
   }
   return output;
}

