#include "CommonPCH.h"

#include "Common/Worker/JobFinished.h"
#include "Common/Worker/JobFinishedToken.h"

std::shared_ptr<JobFinished> JobFinished::Factory(const std::function<void(void)>& callback)
{
   auto pResult = std::make_shared<JobFinished>(callback);
   pResult->SetSelfWeakRef(pResult);
   return pResult;
}

JobFinished::JobFinished(const std::function<void(void)>& callback)
   : m_callback(callback)
{
   return;
}

JobFinished::~JobFinished()
{
   if (nullptr != m_callback)
   {
      m_callback();
   }
}

std::shared_ptr< JobFinishedToken > JobFinished::MakeToken()
{
   return std::make_shared< JobFinishedToken >(m_weakThis.lock());
}

void JobFinished::SetSelfWeakRef(const std::weak_ptr< JobFinished >& weakThis)
{
   m_weakThis = weakThis;
}
