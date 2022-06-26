#pragma once

#include "Common/Application/IApplication.h"
class ApplicationHolder;
class DrawSystem;
class Shader;
class GeometryGeneric;

class ApplicationTriangleJson : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(const HWND hWnd, const IApplicationParam& applicationParam);

   ApplicationTriangleJson(const HWND hWnd, const IApplicationParam& applicationParam);
   virtual ~ApplicationTriangleJson();

private:
   virtual void Update() override;
   virtual void OnWindowSizeChanged(const int width, const int height) override;

private:
   std::unique_ptr< DrawSystem > m_pDrawSystem;
   std::shared_ptr< Shader > m_pShader;
   std::shared_ptr< GeometryGeneric > m_pGeometry;

};