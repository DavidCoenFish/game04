#include "CommonPCH.h"

#include "Common/Worker/JobFinishedToken.h"
#include "Common/Worker/JobFinished.h"


JobFinishedToken::JobFinishedToken(const std::shared_ptr< JobFinished >& pJobFinished)
   : m_pJobFinished(pJobFinished)
{
   return;
};
JobFinishedToken::~JobFinishedToken()
{
   return;
}
void JobFinishedToken::Clear()
{
   m_pJobFinished = nullptr;
}

