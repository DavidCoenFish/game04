#pragma once
#include "Common/Log/ILogConsumer.h"

class LogConsumerConsole : public ILogConsumer
{
public:
   LogConsumerConsole(const std::vector<bool>& topicFilterArrayOrEmpty = std::vector<bool>());
   ~LogConsumerConsole();
private:
   virtual void AddMessage(const LogTopic topic, const std::string& message ) override;
   virtual const bool AcceptsTopic(const LogTopic topic) override;

private:
   std::vector<bool> m_topicFilterArrayOrEmpty;

};
