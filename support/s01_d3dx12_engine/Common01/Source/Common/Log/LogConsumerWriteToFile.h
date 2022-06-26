#pragma once
#include "Common/Log/ILogConsumer.h"

//writes to disk, no dependency on FileSystem class
class LogConsumerWriteToFile: public ILogConsumer
{
public:
   LogConsumerWriteToFile(
      const std::string& logFilePath = GetDefaultPath(),
      const std::vector<bool>& topicFilterArrayOrEmpty = std::vector<bool>()
   );
   ~LogConsumerWriteToFile();
private:
   static const std::string GetDefaultPath();
   virtual void AddMessage(const LogTopic topic, const std::string& message ) override;
   virtual const bool AcceptsTopic(const LogTopic topic) override;

private:
   std::vector<bool> m_topicFilterArrayOrEmpty;

   std::mutex m_fileLock;
   std::ofstream m_file;
};
