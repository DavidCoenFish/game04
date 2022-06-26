#include "CommonPCH.h"
#include "Common/Application/ApplicationBasic.h"
#include "Common/Log/Log.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"

ApplicationBasic::ApplicationBasic(const IApplicationParam& applicationParam)
   : IApplication(applicationParam)
{
   LOG_MESSAGE("ApplicationBasic ctor %p", this);
   m_pDrawSystem = std::make_unique< DrawSystem>(applicationParam.m_hWnd);
}

ApplicationBasic::~ApplicationBasic()
{
   LOG_MESSAGE("ApplicationBasic dtor %p", this);
}

void ApplicationBasic::Update()
{
   BaseType::Update();
   if (m_pDrawSystem)
   {
      auto pTemp = m_pDrawSystem->CreateNewFrame();
      pTemp;
   }
}

void ApplicationBasic::OnWindowSizeChanged(const int width, const int height)
{
   BaseType::OnWindowSizeChanged(width, height);
   if (m_pDrawSystem)
   {
      m_pDrawSystem->OnResize();
   }

   return;
}
