#pragma once

class JobFinished;

class JobFinishedToken
{
public:
   JobFinishedToken(const std::shared_ptr< JobFinished >& pJobFinished);
   ~JobFinishedToken();
   void Clear();
private:
   std::shared_ptr< JobFinished > m_pJobFinished;
};

