#include "CommonPCH.h"
#include "Common/Application/ApplicationDisplayList.h"
#include "Common/Log/Log.h"
#include "Common/DrawSystem/DrawSystem.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/FileSystem/FileSystem.h"
#include "Common/DrawSystem/CustomCommandList.h"
#include "Common/DrawSystem/Shader/ShaderPipelineStateData.h"
#include "Common/DrawSystem/Geometry/GeometryGeneric.h"
#include "Common/DrawSystem/Shader/Shader.h"
#include "Common/DrawSystem/Shader/ShaderResource.h"
#include "Common/DrawSystem/RenderTarget/RenderTargetTexture.h"
#include "Common/DrawSystem/DrawSystemFrame.h"
#include "Common/JSON/JSONDrawSystem.h"
#include "Common/JSON/JSONDagCollection.h"
#include "Common/JSON/JSONShader.h"
#include "Common/JSON/JSONShaderResource.h"
#include "Common/JSON/JSONGeometry.h"
#include "Common/JSON/JSONRenderTarget.h"
#include "Common/DAG/DagCollection.h"
#include "Common/DAG/DagValue.h"
#include "Common/DAG/DagNodeValue.h"
#include "Common/DAG/DagNodeCalculate.h"
#include "Common/DAG/iDagNode.h"
#include "json/json.hpp"

class JSONApplicationDisplayListSourceMove
{
public:
   std::string name;
   std::vector< int > atOffset;
   std::vector< int > upOffset;
   std::vector< float > value;
   std::vector< std::string > keypad;
   float velocity;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONApplicationDisplayListSourceMove,
   name,
   atOffset,
   upOffset,
   value,
   keypad,
   velocity
   );

class JSONApplicationDisplayListSourceRotate
{
public:
   std::vector< std::string > keypad;
   float velocity;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONApplicationDisplayListSourceRotate,
   keypad,
   velocity
   );

class JSONApplicationDisplayListTarget
{
public:
   std::string name;
   std::vector< int > offset;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONApplicationDisplayListTarget,
   name,
   offset
   );


enum class JSONApplicationDisplayListUpdateType
{
SetScreenWidthHeightToDagFloatArray,
InputRotateToDagFloatArray,
InputMoveToDagFloatArray
};

NLOHMANN_JSON_SERIALIZE_ENUM( JSONApplicationDisplayListUpdateType, {
   {ENUM_TOKEN_PAIR(JSONApplicationDisplayListUpdateType, SetScreenWidthHeightToDagFloatArray )},
   {ENUM_TOKEN_PAIR(JSONApplicationDisplayListUpdateType, InputRotateToDagFloatArray          )},
   {ENUM_TOKEN_PAIR(JSONApplicationDisplayListUpdateType, InputMoveToDagFloatArray            )},
   });

class JSONApplicationDisplayListUpdate
{
public:
   JSONApplicationDisplayListUpdateType type;
   nlohmann::json source;
   JSONApplicationDisplayListTarget target;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONApplicationDisplayListUpdate,
   type,
   source,
   target
   );

class JSONHeapWrapperFromRenderTargetData
{
public:
   bool useDepth;
   bool useShaderResource;
   int shaderResourceIndex;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONHeapWrapperFromRenderTargetData,
   useDepth,
   useShaderResource,
   shaderResourceIndex
   );

class JSONApplicationDisplayList
{
public:
   std::string file;
   JSONDrawSystem drawSystem;
   std::vector< JSONApplicationDisplayListUpdate > update;
   //std::vector< nlohmann::json > update;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONApplicationDisplayList,
   file,
   drawSystem,
   update
   );

std::shared_ptr< std::vector<uint8_t> > LoadFile(const std::filesystem::path& rootPath, const std::string& fileName)
{
   if (true == fileName.empty())
   {
      return nullptr;
   }
   auto newPath = rootPath / fileName;
   auto pResult = FileSystem::SyncReadFile(newPath);
   return pResult;
}

typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeValueFactory;
typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> NodeCalculateFactory;

IApplication* const ApplicationDisplayList::Factory(const HWND hWnd, const IApplicationParam& applicationParam)
{
   return new ApplicationDisplayList(hWnd, applicationParam);
}

template <typename TYPE>
NodeCalculateFactory MakeStackFactory()
{
   return [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      data;
      auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
         stackInput; indexInput; pValue;
         auto pLocalValue = std::dynamic_pointer_cast<DagValue< std::vector< TYPE > >>(pValue);
         if (nullptr == pLocalValue)
         {
            pLocalValue = DagValue< std::vector< TYPE > >::Factory(std::vector< TYPE >());
         }
         std::vector< TYPE > resultData;
         for (const auto& item : stackInput )
         {
            auto pTemp = item ? std::dynamic_pointer_cast< DagValue< TYPE > >(item->GetValue()) : nullptr;
            if (nullptr == pTemp)
            {
               continue;
            }
            resultData.push_back(pTemp->GetRef());
         }
         pLocalValue->SetRef(resultData);
         return pLocalValue;
      });
      return pResult;
   };
}

ApplicationDisplayList::ApplicationDisplayList(const HWND hWnd, const IApplicationParam& applicationParam)
   : IApplication(hWnd, applicationParam)
   , m_timePointValid(false)
   , m_timePoint()
{
   LOG_MESSAGE("ApplicationDisplayList ctor %p", this);

   JSONApplicationDisplayList jsonData;
   nlohmann::from_json(applicationParam.m_json, jsonData);

   m_pDrawSystem = DrawSystem::Factory(hWnd, jsonData.drawSystem);

   std::map<std::string, NodeValueFactory> mapValue;
   mapValue["Geometry"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      JSONGeometry jsonGeometry;
      data.get_to(jsonGeometry);
      auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
      auto pGeometry = m_pDrawSystem->MakeGeometryGeneric(
         pCommandList->GetCommandList(),
         jsonGeometry.topology,
         jsonGeometry.inputElementDesc,
         jsonGeometry.vertexData,
         jsonGeometry.floatPerVertex
         );
      auto pValue = DagValue<std::shared_ptr< GeometryGeneric >>::Factory(pGeometry);
      auto pResult = DagNodeValue::Factory( pValue );
      return pResult;
   };
   mapValue["RenderTarget"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      JSONRenderTarget jsonRenderTarget;
      data.get_to(jsonRenderTarget);
      auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
      auto pRenderTargetTexture = m_pDrawSystem->MakeRenderTargetTexture(
         pCommandList->GetCommandList(),
         jsonRenderTarget.targetFormatDataArray,
         jsonRenderTarget.targetDepthData,
         jsonRenderTarget.width,
         jsonRenderTarget.height,
         jsonRenderTarget.resizeWithScreen
         );
      auto pValue = DagValue<std::shared_ptr< RenderTargetTexture >>::Factory(pRenderTargetTexture);
      auto pResult = DagNodeValue::Factory( pValue );
      return pResult;
   };
   mapValue["Texture"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      JSONShaderResource jsonShaderResource;
      data.get_to(jsonShaderResource);
      auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
      auto pShaderResource = m_pDrawSystem->MakeShaderResource(
         pCommandList->GetCommandList(),
         m_pDrawSystem->MakeHeapWrapperCbvSrvUav(),
         jsonShaderResource.desc,
         jsonShaderResource.shaderResourceViewDesc,
         jsonShaderResource.data
         );
      auto pValue = DagValue<std::shared_ptr< ShaderResource >>::Factory(pShaderResource);
      auto pResult = DagNodeValue::Factory( pValue );
      return pResult;
   };
   mapValue["Float"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      float value = 0.0f;
      data.get_to(value);
      auto pValue = DagValue<float>::Factory(value);
      auto pResult = DagNodeValue::Factory( pValue );
      return pResult;
   };
   mapValue["FloatArray"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      std::vector<float> value;
      data.get_to(value);
      auto pValue = DagValue<std::vector<float>>::Factory(value);
      auto pResult = DagNodeValue::Factory( pValue );
      return pResult;
   };

   std::map<std::string, NodeCalculateFactory> mapCalculate;

   //indexInput
   //  0.std::shared_ptr< ShaderResource >
   //output std::shared_ptr< HeapWrapperItem >
   mapCalculate["HeapWrapperFromTexture"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      data;
      auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
         stackInput; indexInput; pValue;
         auto pLocalValue = std::dynamic_pointer_cast<DagValue< std::shared_ptr< HeapWrapperItem > > >(pValue);
         if (nullptr == pLocalValue)
         {
            pLocalValue = DagValue< std::shared_ptr< HeapWrapperItem > >::Factory(nullptr);
         }

         const auto pDagHeapWrapperItem = 0 < indexInput.size() ? std::dynamic_pointer_cast< DagValue< std::shared_ptr< ShaderResource > > >(indexInput[0]->GetValue()) : nullptr;
         const auto pShaderResource = pDagHeapWrapperItem ? pDagHeapWrapperItem->GetRef() : nullptr;
         std::shared_ptr< HeapWrapperItem > pHeapWrapperItem;
         if ( nullptr != pShaderResource)
         {
            pHeapWrapperItem = pShaderResource->GetHeapWrapperItem();
         }

         pLocalValue->SetRef(pHeapWrapperItem);

         return pLocalValue;
      });
      return pResult;
   };

   //indexInput
   //  //0.std::shared_ptr< RenderTargetTexture >, or IRenderTarget?
   //  0.IRenderTarget
   //output std::shared_ptr< HeapWrapperItem >
   mapCalculate["HeapWrapperFromIRenderTarget"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      JSONHeapWrapperFromRenderTargetData jsonHeapWrapperFromRenderTargetData;
      data.get_to(jsonHeapWrapperFromRenderTargetData);
      auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
         stackInput; indexInput; pValue;
         auto pLocalValue = std::dynamic_pointer_cast<DagValue< std::shared_ptr< HeapWrapperItem > > >(pValue);
         if (nullptr == pLocalValue)
         {
            pLocalValue = DagValue< std::shared_ptr< HeapWrapperItem > >::Factory(nullptr);
         }

         const auto pDag = 0 < indexInput.size() ? std::dynamic_pointer_cast< DagValue< IRenderTarget* > >(indexInput[0]->GetValue()) : nullptr;
         const IRenderTarget* pIRenderTarget = pDag ? pDag->Get() : nullptr;
         const RenderTargetTexture* pRenderTargetTexture = dynamic_cast<const RenderTargetTexture*>(pIRenderTarget);
         std::shared_ptr< HeapWrapperItem > pHeapWrapperItem;
         if ( nullptr != pRenderTargetTexture)
         {
            if (true == jsonHeapWrapperFromRenderTargetData.useDepth)
            {
               pHeapWrapperItem = pRenderTargetTexture->GetDepthShaderResourceHeapWrapperItem();
            }
            else if (true == jsonHeapWrapperFromRenderTargetData.useShaderResource)
            {
               pHeapWrapperItem = pRenderTargetTexture->GetShaderResourceHeapWrapperItem(jsonHeapWrapperFromRenderTargetData.shaderResourceIndex);
            }
         }

         pLocalValue->SetRef(pHeapWrapperItem);

         return pLocalValue;
      });
      return pResult;
   };

   //indexInput
   //  0.std::shared_ptr< RenderTargetTexture >, or IRenderTarget?
   //output std::vector< float >
   mapCalculate["WidthHeightFromIRenderTarget"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      data;
      auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
         stackInput; indexInput; pValue;
         auto pLocalValue = std::dynamic_pointer_cast<DagValue<std::vector<float>>>(pValue);
         if (nullptr == pLocalValue)
         {
            pLocalValue = DagValue< std::vector< float > >::Factory(std::vector< float >());
         }

         std::vector< float > output({0.0f, 0.0f});
         const auto pDag = 0 < indexInput.size() ? std::dynamic_pointer_cast< DagValue< IRenderTarget* > >(indexInput[0]->GetValue()) : nullptr;
         const IRenderTarget* pIRenderTarget = pDag ? pDag->Get() : nullptr;
         const RenderTargetTexture* pRenderTargetTexture = dynamic_cast<const RenderTargetTexture*>(pIRenderTarget);
         if ( nullptr != pRenderTargetTexture)
         {
            output[0] = (float)(pRenderTargetTexture->GetWidth());
            output[1] = (float)(pRenderTargetTexture->GetHeight());
         }

         pLocalValue->SetRef(output);

         return pLocalValue;
      });
      return pResult;
   };

   //indexInput
   //  0.std::shared_ptr< RenderTargetTexture >
   //output IRenderTarget
   mapCalculate["IRenderTargetFromRenderTargetTexture"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      data;
      auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
         stackInput; indexInput; pValue;
         auto pLocalValue = std::dynamic_pointer_cast<DagValue<IRenderTarget*>>(pValue);
         if (nullptr == pLocalValue)
         {
            pLocalValue = DagValue<IRenderTarget*>::Factory(nullptr);
         }

         auto pDag = 0 < indexInput.size() ? std::dynamic_pointer_cast< DagValue< std::shared_ptr< RenderTargetTexture > > >(indexInput[0]->GetValue()) : nullptr;
         RenderTargetTexture* pRenderTargetTexture = pDag ? pDag->Get().get() : nullptr;
         pLocalValue->Set(pRenderTargetTexture);

         return pLocalValue;
      });
      return pResult;
   };

   //stackInput [std::shared_ptr< HeapWrapperItem >]
   //output std::vector< std::shared_ptr< HeapWrapperItem > >
   mapCalculate["StackHeapWrapper"] = MakeStackFactory<std::shared_ptr<HeapWrapperItem>>();

   //stackInput [float]
   //output std::vector< float >
   mapCalculate["StackFloat"] = MakeStackFactory<float>();

   //stackInput [std::vector< float >]
   //output std::vector< std::vector< float > >
   mapCalculate["StackVectorFloat"] = MakeStackFactory<std::vector< float >>();

   //indexInput
   //  0.std::vector< std::shared_ptr< HeapWrapperItem > >
   //  1.std::vector< std::vector< float > >
   //output std::shared_ptr< Shader >
   mapCalculate["Shader"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      JSONShader jsonShader;
      data.get_to(jsonShader);
      auto pCommandList = m_pDrawSystem->CreateCustomCommandList();
      auto pShader = m_pDrawSystem->MakeShader(
         pCommandList->GetCommandList(),
         jsonShader.pipelineState,
         LoadFile(applicationParam.m_rootPath, jsonShader.vertexShader),
         LoadFile(applicationParam.m_rootPath, jsonShader.geometryShader),
         LoadFile(applicationParam.m_rootPath, jsonShader.pixelShader),
         TransformShaderResourceInfo(jsonShader.resourceInfo),
         TransformConstantBufferInfo(jsonShader.constantInfo)
         );
      auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
         stackInput; indexInput; pValue;
         auto pLocalValue = std::dynamic_pointer_cast<DagValue< std::shared_ptr< Shader > > >(pValue);
         if (nullptr == pLocalValue)
         {
            pLocalValue = DagValue< std::shared_ptr< Shader > >::Factory(pShader);
         }

         const auto pArrayHeapWrapperItem = 0 < indexInput.size() && indexInput[0] ? std::dynamic_pointer_cast< DagValue< std::vector< std::shared_ptr< HeapWrapperItem > > > >(indexInput[0]->GetValue()) : nullptr;
         const auto pArrayConstantBuffer = 1 < indexInput.size() && indexInput[1] ? std::dynamic_pointer_cast< DagValue< std::vector< std::vector< float > > > >(indexInput[1]->GetValue()) : nullptr;

         if (nullptr != pArrayHeapWrapperItem)
         {
            const std::vector< std::shared_ptr< HeapWrapperItem > >& arrayHeapWrapperItem = pArrayHeapWrapperItem->GetRef();
            for (int index = 0; index < (int)arrayHeapWrapperItem.size(); ++index)
            {
               pShader->SetShaderResourceViewHandle( index, arrayHeapWrapperItem[index] );
            }
         }
         if (nullptr != pArrayConstantBuffer)
         {
            const std::vector< std::vector< float > >& arrayConstantBuffer = pArrayConstantBuffer->GetRef();
            for (int index = 0; index < (int)arrayConstantBuffer.size(); ++index)
            {
               pShader->SetConstantBufferData( index, arrayConstantBuffer[index] );
            }
         }

         return pLocalValue;
      });
      return pResult;
   };

   //stackInput 
   // std::shared_ptr< GeometryGeneric >
   //output 
   //  std::vector< std::shared_ptr< GeometryGeneric > >
   mapCalculate["StackGeometry"] = MakeStackFactory<std::shared_ptr< GeometryGeneric >>();

   //indexInput
   //  0.std::shared_ptr< Shader >
   //  1.std::vector< std::shared_ptr< GeometryGeneric > >
   //output 
   //  std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > >
   mapCalculate["Draw"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      data;
      auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
         stackInput; indexInput; pValue;
         typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > > PairData;
         auto pLocalValue = std::dynamic_pointer_cast<DagValue< PairData > >(pValue);

         const auto pShader = std::dynamic_pointer_cast< DagValue< std::shared_ptr< Shader > > >(indexInput[0]->GetValue());
         const auto pGeometryList = std::dynamic_pointer_cast< DagValue< std::vector< std::shared_ptr< GeometryGeneric > > > >(indexInput[1]->GetValue());
         PairData newValue( pShader->GetRef(), pGeometryList->GetRef() );
         if (nullptr == pLocalValue)
         {
            //pLocalValue = std::make_shared< DagValue< std::vector< std::shared_ptr< Geometry > > > >();
            pLocalValue = DagValue< PairData >::Factory(newValue);
         }
         else
         {
            pLocalValue->SetRef(newValue);
         }
         return pLocalValue;
      });
      return pResult;
   };

   //stackInput
   //  std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > >
   //output
   //  std::vector< std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > > >
   mapCalculate["StackDraw"] = MakeStackFactory<std::pair<std::shared_ptr<Shader>, std::vector<std::shared_ptr<GeometryGeneric>>>>();

   //indexInput
   //  0.DrawSystemFrame
   //  1.IRenderTarget
   //  2.std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > >
   //output 
   //  IRenderTarget* (same as input 1)
   mapCalculate["RenderList"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      data;
      auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
         stackInput; indexInput; pValue;
         typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > > PairData;
         typedef std::vector< PairData > PairArray;

         const auto pDAGDrawSystemFrame = std::dynamic_pointer_cast< DagValue< DrawSystemFrame* > >(indexInput[0]->GetValue());
         const auto pDAGRenderTarget = std::dynamic_pointer_cast< DagValue< IRenderTarget* > >(indexInput[1]->GetValue());
         //const auto pDAGRenderTargetTexture = std::dynamic_pointer_cast< DagValue< std::shared_ptr<RenderTargetTexture> > >(pValue);
         const auto pDAGDrawList = std::dynamic_pointer_cast< DagValue< PairArray > >(indexInput[2]->GetValue());

         DrawSystemFrame* pDrawSystemFrame = pDAGDrawSystemFrame ? pDAGDrawSystemFrame->Get() : nullptr;
         IRenderTarget* pRenderTarget = pDAGRenderTarget ? pDAGRenderTarget->Get() : nullptr;
         assert(nullptr != pDrawSystemFrame);
         assert(nullptr != pRenderTarget);
         assert(nullptr != pDAGDrawList);
         const auto& drawList = pDAGDrawList->GetRef();

         pDrawSystemFrame->SetRenderTarget( pRenderTarget );

         for (const auto& item : drawList)
         {
            pDrawSystemFrame->SetShader(item.first.get());
            for (const auto& subItem : item.second)
            {
               pDrawSystemFrame->Draw(subItem.get());
            }
         }

         auto pLocalValue = std::dynamic_pointer_cast<DagValue< IRenderTarget* >>(pValue);
         if (nullptr == pLocalValue)
         {
            pLocalValue = DagValue< IRenderTarget* >::Factory(nullptr);
         }
         pLocalValue->SetRef(pRenderTarget);

         return pLocalValue;
      });
      return pResult;
   };

   //value. FloatFromInput
   //position. Float3FromInput
   //rotation. Float4FromInput

   //jsonData
   //  keyPositive, keyNegative
   //  valueRateOfChange 
   //  useRangeMax, rangeMax, Min
   //indexInput
   //  0._InputState
   //output 
   //  float
   mapCalculate["FloatFromInput"] = [=](const nlohmann::json& data) -> std::shared_ptr< iDagNode > {
      data;
      auto pResult = DagNodeCalculate::Factory([=](const std::vector< iDagNode* >& stackInput, const std::vector< iDagNode* >& indexInput, const std::shared_ptr< iDagValue >& pValue) -> std::shared_ptr< iDagValue >{
         stackInput; indexInput; pValue;
         typedef std::pair< std::shared_ptr< Shader >, std::vector< std::shared_ptr< GeometryGeneric > > > PairData;
         typedef std::vector< PairData > PairArray;

         const auto pDAGDrawSystemFrame = std::dynamic_pointer_cast< DagValue< DrawSystemFrame* > >(indexInput[0]->GetValue());
         const auto pDAGRenderTarget = std::dynamic_pointer_cast< DagValue< IRenderTarget* > >(indexInput[1]->GetValue());
         //const auto pDAGRenderTargetTexture = std::dynamic_pointer_cast< DagValue< std::shared_ptr<RenderTargetTexture> > >(pValue);
         const auto pDAGDrawList = std::dynamic_pointer_cast< DagValue< PairArray > >(indexInput[2]->GetValue());

         DrawSystemFrame* pDrawSystemFrame = pDAGDrawSystemFrame ? pDAGDrawSystemFrame->Get() : nullptr;
         IRenderTarget* pRenderTarget = pDAGRenderTarget ? pDAGRenderTarget->Get() : nullptr;
         assert(nullptr != pDrawSystemFrame);
         assert(nullptr != pRenderTarget);
         assert(nullptr != pDAGDrawList);
         const auto& drawList = pDAGDrawList->GetRef();

         pDrawSystemFrame->SetRenderTarget( pRenderTarget );

         for (const auto& item : drawList)
         {
            pDrawSystemFrame->SetShader(item.first.get());
            for (const auto& subItem : item.second)
            {
               pDrawSystemFrame->Draw(subItem.get());
            }
         }

         auto pLocalValue = std::dynamic_pointer_cast<DagValue< IRenderTarget* >>(pValue);
         if (nullptr == pLocalValue)
         {
            pLocalValue = DagValue< IRenderTarget* >::Factory(nullptr);
         }
         pLocalValue->SetRef(pRenderTarget);

         return pLocalValue;
      });
      return pResult;
   };


   std::vector< std::pair< std::string, std::shared_ptr< iDagNode > > > inbuiltDagValues;
   {
      //_FrameCount
      m_pDagFrameCount = DagNodeValue::Factory( DagValue<int>::Factory(0));
      inbuiltDagValues.push_back( {"_FrameCount", m_pDagFrameCount });
      //_DrawSystemFrame
      m_pDagDrawSystemFrame = DagNodeValue::Factory( 
            DagValue<DrawSystemFrame*>::Factory(nullptr),
            false
            );
      inbuiltDagValues.push_back( {"_DrawSystemFrame", m_pDagDrawSystemFrame});
      //_BackBuffer
      m_pDagBackBuffer = DagNodeValue::Factory(DagValue<IRenderTarget*>::Factory(nullptr));
      inbuiltDagValues.push_back( {"_BackBuffer", m_pDagBackBuffer});
      //_TimeAccumulate
      m_pDagTimeAccumulate = DagNodeValue::Factory( DagValue<float>::Factory(0));
      inbuiltDagValues.push_back( {"_TimeAccumulate", m_pDagTimeAccumulate });
      //_InputState
   }

   m_pDagCollection = JSONDagCollection::Factory(
      jsonData.file,
      [=](const std::string& fileName){
         auto newPath = applicationParam.m_rootPath / fileName;
         auto pResult = FileSystem::SyncReadFile(newPath);
         auto result = FileSystem::DataToString(pResult);
         return result;
      },
      inbuiltDagValues, 
      mapValue, 
      mapCalculate
      );

   //hook up Update array
   for (auto& item : jsonData.update)
   {
       auto pTarget = m_pDagCollection->GetDagNode(item.target.name);
       if (nullptr == pTarget)
       {
           continue;
       }

       switch (item.type)
       {
       default:
           break;
       case JSONApplicationDisplayListUpdateType::InputMoveToDagFloatArray:
           {
                JSONApplicationDisplayListSourceMove jsonSourceMove;
                nlohmann::from_json(item.source, jsonSourceMove);
                auto pSource = m_pDagCollection->GetDagNode(jsonSourceMove.name);
           }
           break;
       case JSONApplicationDisplayListUpdateType::InputRotateToDagFloatArray:
           {
                JSONApplicationDisplayListSourceRotate jsonSourceRotate;
                nlohmann::from_json(item.source, jsonSourceRotate);
           }
           break;
       case JSONApplicationDisplayListUpdateType::SetScreenWidthHeightToDagFloatArray:

           break;
       }
   }
}

ApplicationDisplayList::~ApplicationDisplayList()
{
   if (m_pDrawSystem)
   {
      m_pDrawSystem->WaitForGpu();
   }
   m_pDagCollection.reset();
   m_pDrawSystem.reset();

   LOG_MESSAGE("ApplicationDisplayList dtor %p", this);
}

void ApplicationDisplayList::Update()
{
   BaseType::Update();

   const auto timePointNow = std::chrono::system_clock::now();
   float timeDeltaSeconds = 0.0f;
   if (true == m_timePointValid)
   {
      timeDeltaSeconds = (1.0f / 1000.0f) * (float)(std::chrono::duration_cast<std::chrono::milliseconds>(timePointNow - m_timePoint).count());
   } 
   else
   {
      m_timePointValid = true;
   }
   m_timePoint = timePointNow;
   if (m_pDagTimeAccumulate)
   {
      DagValue<float>::IncrementNode(m_pDagTimeAccumulate, timeDeltaSeconds);
   }

   if (m_pDrawSystem)
   {
      auto pFrame = m_pDrawSystem->CreateNewFrame();

      DagValue<int>::IncrementNode(m_pDagFrameCount, 1);
      DagValue<DrawSystemFrame*>::UpdateNode(m_pDagDrawSystemFrame, pFrame.get());
      DagValue<IRenderTarget*>::UpdateNode(m_pDagBackBuffer, m_pDrawSystem->GetRenderTargetBackBuffer());

      auto pRender = m_pDagCollection->GetDagNode("_Render");
      if (pRender)
      {
         pRender->GetValue();
      }
   }
}

void ApplicationDisplayList::OnWindowSizeChanged(const int width, const int height)
{
   BaseType::OnWindowSizeChanged(width, height);
   if (m_pDrawSystem)
   {
      m_pDrawSystem->OnResize();
   }

   return;
}

void ApplicationDisplayList::OnKey(const int vkCode, const int scanCode, const bool repeatFlag, const int repeatCount, bool upFlag)
{
    LOG_MESSAGE("OnKey %d %d %d %d %d", vkCode, scanCode, repeatFlag, repeatCount, upFlag);

    return;
}
