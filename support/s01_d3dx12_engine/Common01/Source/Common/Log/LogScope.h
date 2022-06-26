#pragma once

#if defined(_DEBUG)
   #define LOG_SCOPE_DEBUG(name) LogScope MACRO_JOIN(_logScope,__LINE__)(name)
#else
   #define LOG_SCOPE_DEBUG(name) (void)0
#endif

class LogScope
{
public:
   explicit LogScope(const char* const pName = nullptr);
   ~LogScope();
private:
   const char* const m_pName;
};
