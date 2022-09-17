#pragma once

#include "Common/Application/IApplication.h"
class DrawSystem;
class DagCollection;
class iDagNode;

class ApplicationDisplayList : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(const HWND hWnd, const IApplicationParam& applicationParam);

   ApplicationDisplayList(const HWND hWnd, const IApplicationParam& applicationParam);
   virtual ~ApplicationDisplayList();

private:
   virtual void Update() override;
   virtual void OnWindowSizeChanged(const int width, const int height) override;

private:
   std::unique_ptr< DrawSystem > m_pDrawSystem;
   std::shared_ptr< DagCollection > m_pDagCollection;
   std::shared_ptr< iDagNode > m_pDagTimeAccumulate;
   std::shared_ptr< iDagNode > m_pDagFrameCount;
   std::shared_ptr< iDagNode > m_pDagBackBuffer;
   std::shared_ptr< iDagNode > m_pDagDrawSystemFrame;
   std::shared_ptr< iDagNode > m_pDagRender;

   bool m_timePointValid;
   std::chrono::system_clock::time_point m_timePoint;

};