#pragma once

class IApplication;

class ApplicationHolder
{
public:
   ApplicationHolder();
   void AddApplication(IApplication* pApplication);
   void RemoveApplication(IApplication* pApplication);
   const bool HasApplication() const;
   void Update();

private:
   mutable std::mutex m_windowsMutex;
   std::vector<IApplication*> m_windows;

};