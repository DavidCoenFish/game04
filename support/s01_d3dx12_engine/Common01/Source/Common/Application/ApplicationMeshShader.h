#pragma once

#include "Common/Application/IApplication.h"
#include "Common/DrawSystem/Geometry/Geometry.h"
class ApplicationHolder;
class DrawSystem;
class Shader;
class GeometryGeneric;

class ApplicationMeshShader : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(const HWND hWnd, const IApplicationParam& applicationParam);

   ApplicationMeshShader (const HWND hWnd, const IApplicationParam& applicationParam);
   virtual ~ApplicationMeshShader ();

private:
   virtual void Update() override;
   virtual void OnWindowSizeChanged(const int width, const int height) override;

private:
   std::unique_ptr< DrawSystem > m_pDrawSystem;

};