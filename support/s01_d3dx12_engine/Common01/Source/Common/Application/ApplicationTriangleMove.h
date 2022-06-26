#pragma once

#include "Common/Application/IApplication.h"
class ApplicationHolder;
class DrawSystem;
class Shader;
class GeometryGeneric;

class ApplicationTriangleMove : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(const HWND hWnd, const IApplicationParam& applicationParam);

   ApplicationTriangleMove(const HWND hWnd, const IApplicationParam& applicationParam);
   virtual ~ApplicationTriangleMove();

private:
   virtual void Update() override;
   virtual void OnWindowSizeChanged(const int width, const int height) override;

private:
   std::unique_ptr< DrawSystem > m_pDrawSystem;
   std::shared_ptr< Shader > m_pShader;
   std::shared_ptr< GeometryGeneric > m_pGeometry;

   bool m_timePointValid;
   std::chrono::system_clock::time_point m_timePoint;
   float m_timeAccumulation;

};