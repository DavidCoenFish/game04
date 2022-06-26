#include "CommonPCH.h"

#include "Common/FileSystem/ProviderDisk.h"

#if 0
OverlayDisk::OverlayDisk(const int filter, const std::filesystem::path& basePath)
   : m_filter(filter)
   , m_basePath(basePath)
   , m_componentFileMap()
{
   //nop
}

OverlayDisk::~OverlayDisk()
{
   //nop
}

std::shared_ptr< std::vector<uint8_t> > OverlayDisk::ReadFileLoadData(const std::filesystem::path& path)
{
   const auto localPath = m_basePath / path;
   std::ifstream stream(localPath.c_str(), std::ios::in | std::ios::binary);
   if (false == stream.is_open())
   {
      return nullptr;
   }
   auto pResult = std::make_shared<std::vector<uint8_t>>((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
   return pResult;
}
#endif// 0
