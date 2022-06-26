#pragma once

#include "Common/Log/LogEnum.h"

class ILogConsumer
{
public:
   virtual ~ILogConsumer();

   virtual void AddMessage(const LogTopic topic, const std::string& message ) = 0;

   virtual const bool AcceptsTopic(const LogTopic topic) = 0;

   // don't support run time change of supported topics, reasons. performance and thread safty
   //virtual void RegisterAcceptsTopicChangeCallback(const std::function<void(void)>& callback) = 0;
};
