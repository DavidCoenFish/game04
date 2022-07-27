#pragma once

#include "Common/Application/IApplication.h"
#include "Common/DrawSystem/Geometry/Geometry.h"
class ApplicationHolder;
class DrawSystem;
class Shader;
class ShaderResource;
class GeometryGeneric;

class ApplicationText : public IApplication
{
   typedef IApplication BaseType;
public:
   static IApplication* const Factory(const HWND hWnd, const IApplicationParam& applicationParam);

   ApplicationText(const HWND hWnd, const IApplicationParam& applicationParam);
   virtual ~ApplicationText();

private:
   virtual void Update() override;
   virtual void OnWindowSizeChanged(const int width, const int height) override;

private:
    std::unique_ptr< DrawSystem > m_pDrawSystem;
    std::shared_ptr< Shader > m_pShader;
    std::shared_ptr< ShaderResource > m_pTexture;
    struct Vertex
   {
       DirectX::XMFLOAT2 m_positionBottomLeft;
       DirectX::XMFLOAT2 m_positionTopRight;
       DirectX::XMFLOAT2 m_uvBottomLeft;
       DirectX::XMFLOAT2 m_uvTopRight;
       DirectX::XMFLOAT4 m_textureMask;
   };
   std::shared_ptr< Geometry< Vertex > > m_pGeometry;

};