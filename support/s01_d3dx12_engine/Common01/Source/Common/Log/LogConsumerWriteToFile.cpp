#include "CommonPCH.h"

#include "Common/Log/LogConsumerWriteToFile.h"
#include "Common/Log/Log.h"

LogConsumerWriteToFile::LogConsumerWriteToFile(
   const std::string& logFilePath,
   const std::vector<bool>& topicFilterArrayOrEmpty
   )
   : m_topicFilterArrayOrEmpty(topicFilterArrayOrEmpty)
{
   std::lock_guard< std::mutex > lock(m_fileLock);
   m_file.open(logFilePath.c_str(), std::ofstream::out);
}

LogConsumerWriteToFile::~LogConsumerWriteToFile()
{
   std::lock_guard< std::mutex > lock(m_fileLock);
   if (true == m_file.is_open())
   {
      m_file.flush();
      m_file.close();
   }
}

const std::string LogConsumerWriteToFile::GetDefaultPath()
{
   return "g:\\log.txt";
}

void LogConsumerWriteToFile::AddMessage(const LogTopic topic, const std::string& message )
{
   std::lock_guard< std::mutex > lock(m_fileLock);
   if (true == m_file.is_open())
   {
      //warning, does "<< std::endl" flush, and is that ok?
      m_file << (int)topic << ":" << message.c_str() << std::endl;
   }
}

const bool LogConsumerWriteToFile::AcceptsTopic(const LogTopic topic)
{
   if ((0 <= (int)topic) && ((int)topic < m_topicFilterArrayOrEmpty.size()))
   {
      return m_topicFilterArrayOrEmpty[(int)topic];
   }

   return true;
}

