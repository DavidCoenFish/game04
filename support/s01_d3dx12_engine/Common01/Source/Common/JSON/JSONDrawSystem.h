#pragma once

#include "json/json.hpp"
#include "Common/JSON/JSONRenderTarget.h"

class DrawSystem;
class CustomCommandList;
class Shader;
class FileCache;

class JSONDrawSystem
{
public:
   JSONDrawSystem()
      : backBufferCount(2)
      , d3dFeatureLevel(D3D_FEATURE_LEVEL_11_0)
      , options(0)
   {
      //nop
   }
   unsigned int backBufferCount;
   D3D_FEATURE_LEVEL d3dFeatureLevel;
   unsigned int options;
   RenderTargetFormatData targetFormatData;
   RenderTargetDepthData targetDepthData;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONDrawSystem,
   backBufferCount,
   d3dFeatureLevel,
   options,
   targetFormatData,
   targetDepthData
   );
