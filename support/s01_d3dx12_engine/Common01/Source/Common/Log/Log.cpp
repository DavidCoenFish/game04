#include "CommonPCH.h"

#include "Common/Log/Log.h"
#include "Common/Log/ILogConsumer.h"
#include "Common/Worker/WorkerTask.h"
#include "Common/Macro.h"
#include "Common/Util/Utf8.h"

static std::atomic<Log*> s_singleton = nullptr;

class LogImplimentation
{
public:
   LogImplimentation(const std::vector< std::shared_ptr< ILogConsumer > >& logConsumers);
   ~LogImplimentation();
   void DrainLogMessages();
   const bool AcceptsTopic(const LogTopic topic);
   void AddMessage(const LogTopic topic, const std::string& message);

private:
   void DoWork();

private:
   std::shared_ptr<WorkerTask> m_workerThread;

   std::list< std::pair< LogTopic, std::string > > m_listMessages;
   std::mutex m_listMessagesMutex;

   //ownership
   std::vector< std::shared_ptr< ILogConsumer > > m_logConsumers;

   //cache the consumers for each topic so we don't have to keep on filtering
   std::vector< ILogConsumer*> m_topicLogConsumers[(unsigned int)LogTopic::Count];
};

class CheckAllLogMessagesConsumed
{
public:
   ~CheckAllLogMessagesConsumed()
   {
      //so, this raises an issue. if we have logged messages before the log is constructed to consume all messages
      // as the application shuts down before messages can ever be consumed, have we done our job, or does log scope need to be eariler, like static startup?
      // or should we never log messages in static startup? at least before log scope...?
      std::lock_guard lock(m_listMessagesMutex);
      if (0 != m_listMessages.size())
      {
         Log::AddConsole("Warning: dtor of log with %d log messages not consumed", (int)m_listMessages.size());
      }
      assert(0 == m_listMessages.size());
   }
   void AddMessage(const LogTopic topic, const std::string& message)
   {
      std::lock_guard lock(m_listMessagesMutex);
      m_listMessages.push_back(std::pair< LogTopic, std::string >(topic, message));
   }
   void ConsumeAllMessages(std::list< std::pair< LogTopic, std::string > >& listMessages)
   {
      std::lock_guard lock(m_listMessagesMutex);
      for (const auto& iter: m_listMessages)
      {
         listMessages.push_back(iter);
      }
      m_listMessages.clear();
   }
   //the unit test may cause logs to emit, but never care about consuming, and no appropriate central place to consume logs for unit test? (todo)
   void RunningUintTest()
   {
      std::lock_guard lock(m_listMessagesMutex);
      m_listMessages.clear();
   }
public:
   std::list< std::pair< LogTopic, std::string > > m_listMessages;
   std::mutex m_listMessagesMutex;
};
static CheckAllLogMessagesConsumed s_CheckAllLogMessagesConsumed;

LogImplimentation::LogImplimentation(const std::vector< std::shared_ptr< ILogConsumer > >& logConsumers)
   : m_logConsumers(logConsumers)
{
   //DSC_ASSERT(nullptr == s_singleton);
   //s_singleton = this;
   m_workerThread = WorkerTask::Factory([=](){
      DoWork();
   });
   //UpdateTopicLogConsumers();
   for (int index = 0; index < (int)LogTopic::Count; ++index)
   {
      for (auto iter : logConsumers)
      {
         if (true == iter->AcceptsTopic((LogTopic)index))
         {
            m_topicLogConsumers[index].push_back(iter.get());
         }
      }
   }
}

LogImplimentation::~LogImplimentation()
{
   m_workerThread = nullptr;
}

void LogImplimentation::DrainLogMessages()
{
   std::lock_guard< std::mutex > lock(m_listMessagesMutex);
   s_CheckAllLogMessagesConsumed.ConsumeAllMessages(m_listMessages);
   //normally null check things, ie. m_workerThread
   // in this case, that would allow worker thread to not be created, and silently do a fake consume of log messages
   m_workerThread->SignalWorkToDo();
}

const bool LogImplimentation::AcceptsTopic(const LogTopic topic)
{
   return (0 != m_topicLogConsumers[(unsigned int)topic].size());
}

void LogImplimentation::AddMessage(const LogTopic topic, const std::string& message)
{
   {
      std::lock_guard< std::mutex > lock(m_listMessagesMutex);
      m_listMessages.push_back(std::pair< LogTopic, std::string >(topic, message));
   }

   if (nullptr != m_workerThread)
   {
      m_workerThread->SignalWorkToDo();
   }
   return;
}

void LogImplimentation::DoWork()
{
   while (true)
   {
      std::pair< LogTopic, std::string > messagePair;
      {
         std::lock_guard< std::mutex > lock(m_listMessagesMutex);
         if (true == m_listMessages.empty())
         {
            return;
         }
         messagePair = m_listMessages.front();
         m_listMessages.pop_front();
      }

      for (auto pIter : m_topicLogConsumers[(unsigned int)messagePair.first])
      {
         pIter->AddMessage(messagePair.first, messagePair.second);
      }
   }

   return;
}

static const std::string FormatString(const char* const pFormat, va_list vaArgs)
{
   // reliably acquire the size
   // from a copy of the variable argument array
   // and a functionally reliable call to mock the formatting
   va_list vaArgsCopy;
   va_copy(vaArgsCopy, vaArgs);
   const int iLen = std::vsnprintf(NULL, 0, pFormat, vaArgsCopy);
   va_end(vaArgsCopy);

   // return a formatted string without risking memory mismanagement
   // and without assuming any compiler or platform specific behavior
   std::vector<char> zc(iLen + 1);
   std::vsnprintf(zc.data(), zc.size(), pFormat, vaArgs);

   std::string message(zc.data(), iLen); 

   return message;
}

void Log::AddConsole(const char* const pFormat, ... )
{
   va_list vaArgs;
   va_start(vaArgs, pFormat);
   std::string message = FormatString(pFormat, vaArgs);
   va_end(vaArgs);

   message += "\n";
   OutputDebugStringW(Utf8::Utf8ToUtf16(message).c_str());
   //OutputDebugStringW(L"\n");
}

std::shared_ptr< Log > Log::Factory(const LogContainer& arrayConsumer, const bool bOwnerSingelton)
{
   auto pResult = std::make_shared< Log >(arrayConsumer, bOwnerSingelton);
   return pResult;
}

Log::Log(const std::vector< std::shared_ptr< ILogConsumer >>& arrayConsumer, const bool bOwnerSingelton)
   : m_bOwnerSingelton(bOwnerSingelton)
{
   m_pImplimentation = std::make_unique< LogImplimentation >(arrayConsumer);
   if (true == m_bOwnerSingelton)
   {
      auto pOldSingelton = s_singleton.exchange(this);
      DSC_ASSERT(nullptr == pOldSingelton);
      DrainLogMessages();
   }
}

Log::~Log()
{
   if (true == m_bOwnerSingelton)
   {
      auto pOldSingelton = s_singleton.exchange(nullptr);
      DSC_ASSERT(nullptr != pOldSingelton);
   }
   m_pImplimentation = nullptr;
}

//https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/8098080
void Log::AddMessage(const LogTopic topic, const char* const pFormat, ... )
{
   auto pLog = s_singleton.load();
   // you know what, if the log doesn't exist, that may be intentional
   //DSC_ASSERT(pImple);
   bool bDumpToMemory = false;
   if (nullptr == pLog)
   {
      bDumpToMemory = true;
   }

   if ((nullptr != pLog) && (false == pLog->AcceptsTopic(topic)))
   {
      return;
   }

   va_list vaArgs;
   va_start(vaArgs, pFormat);
   const std::string message = FormatString(pFormat, vaArgs);
   va_end(vaArgs);

   if (false == bDumpToMemory)
   {
      pLog->AddMessageInternal(topic, message);
   }
   else
   {
      s_CheckAllLogMessagesConsumed.AddMessage(topic, message);
      AddConsole("Warning: Message before log is ready: topic:[%d] message:[%s]", (int)topic, message.c_str());
      // we could log the fact that we are logging outside log scope? seems redundant though
      //if (topic != LogTopic::Log)
      //{
      //   AddMessage(LogTopic::Log, "Warning: topic:[%d] is trying to add messages to log outside log scope");
      //}
   }
}

void Log::RunningUintTest()
{
   s_CheckAllLogMessagesConsumed.RunningUintTest();
}

const bool Log::AcceptsTopic(const LogTopic topic)
{
   return m_pImplimentation->AcceptsTopic(topic);
}

void Log::MemberAddMessage(const LogTopic topic, const char* const pFormat, ... )
{
   if (false == m_pImplimentation->AcceptsTopic(topic))
   {
      return;
   }

   va_list vaArgs;
   va_start(vaArgs, pFormat);
   const std::string message = FormatString(pFormat, vaArgs);
   va_end(vaArgs);

   m_pImplimentation->AddMessage(topic, message);
}

void Log::DrainLogMessages()
{
   m_pImplimentation->DrainLogMessages();
}

void Log::AddMessageInternal(const LogTopic topic, const std::string& message)
{
   m_pImplimentation->AddMessage(topic, message);
}
