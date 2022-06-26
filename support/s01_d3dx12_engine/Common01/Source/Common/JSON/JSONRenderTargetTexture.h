#pragma once

#include "json/json.hpp"
#include "Common/JSON/JSONRenderTarget.h"

struct JSONRenderTargetTexture
{
   std::vector< RenderTargetFormatData > targetFormatDataArray;
   RenderTargetDepthData targetDepthData;
   int width;
   int height;
   bool resizeWithScreen;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONRenderTargetTexture,
   targetFormatDataArray,
   targetDepthData,
   width,
   height,
   resizeWithScreen
   );
