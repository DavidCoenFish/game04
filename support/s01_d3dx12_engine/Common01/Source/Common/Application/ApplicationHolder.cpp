#include "CommonPCH.h"
#include "Common/Application/ApplicationHolder.h"
#include "Common/Application/IApplication.h"

ApplicationHolder::ApplicationHolder()
{
   //nop
}

void ApplicationHolder::AddApplication(IApplication* pApplication)
{
   std::lock_guard< std::mutex >lock(m_windowsMutex);
   m_windows.push_back(pApplication);
}

void ApplicationHolder::RemoveApplication(IApplication* pApplication)
{
   std::lock_guard< std::mutex >lock(m_windowsMutex);
   m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), pApplication), m_windows.end());
}

const bool ApplicationHolder::HasApplication() const
{
   std::lock_guard< std::mutex >lock(m_windowsMutex);
   return (0 < m_windows.size());
}

//is this too big a scope for the lock, taking the count outside the lock, but then array bounds checing inside a lock
void ApplicationHolder::Update()
{
#if 1
   int count = (int)m_windows.size();
   for (int index = 0; index < count; ++index)
   {
      std::lock_guard< std::mutex >lock(m_windowsMutex);
      if (index < (int)m_windows.size())
      {
         m_windows[index]->Update();
      }
   }
#else
   std::lock_guard< std::mutex >lock(m_windowsMutex);
   for(auto pItem : m_windows)
   {
      pItem->Update();
   }
   return;
#endif
}


