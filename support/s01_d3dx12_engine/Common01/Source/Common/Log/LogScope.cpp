#include "CommonPCH.h"

#include "Common/Log/LogScope.h"
#include "Common/Log/Log.h"

LogScope::LogScope(const char* const pName)
   : m_pName(pName)
{
   LOG_MESSAGE_SCOPE("Scope start %s\n", m_pName);
}

LogScope::~LogScope()
{
   LOG_MESSAGE_SCOPE("Scope end %s\n", m_pName);
}


