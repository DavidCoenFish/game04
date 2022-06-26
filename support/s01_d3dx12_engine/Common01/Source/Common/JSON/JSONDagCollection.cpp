#include "CommonPCH.h"
#include "Common/JSON/JSONDagCollection.h"
#include "Common/DAG/iDagNode.h"
#include "Common/DAG/DagCollection.h"
#include "Common/Log/Log.h"

typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> ValueFactory;

//https://stackoverflow.com/questions/7724011/in-c-whats-the-fastest-way-to-replace-all-occurrences-of-a-substring-within
static void replaceStringAll(std::string& str, const std::string& old, const std::string& new_s) {
   if(!old.empty())
   {
      size_t pos = str.find(old);
      while ((pos = str.find(old, pos)) != std::string::npos) 
      {
         str=str.replace(pos, old.length(), new_s);
         pos += new_s.length();
      }
   }
   return;
}

static void DealItem(JSONDagCollection& jsonChild, const JSONDagFile& item, const std::function<const std::string(const std::string& fileName)>& dealFileCallback)
{
   std::string data = dealFileCallback(item.file);
   if (item.searchReplace.is_object())
   {
      nlohmann::json::object_t object;
      item.searchReplace.get_to(object);
      for(const auto& iter : object)
      {
         if (false == iter.second.is_string())
         {
            continue;
         }
         std::string value;
         iter.second.get_to(value);
         replaceStringAll(data, iter.first, value);
      }
   }

   auto json = nlohmann::json::parse( data );
   json.get_to(jsonChild);
}

static void AppendCollectionInternal(
   const std::shared_ptr< DagCollection >& pDagCollection,
   const JSONDagCollection& jsonDagCollection,
   const std::function<const std::string(const std::string& fileName)>& dealFileCallback,
   const std::map<std::string, ValueFactory>& valueFactoryMap,
   const std::map<std::string, ValueFactory>& calculateFactoryMap
   )
{
   for (const auto& item : jsonDagCollection.valueArray)
   {
      auto found = valueFactoryMap.find(item.type);
      if (valueFactoryMap.end() == found)
      {
         LOG_MESSAGE_ERROR("Failed to find node value type:%s", item.type.c_str());
         continue;
      }
      auto pNode = found->second(item.data);
      pDagCollection->AddDagNode(item.name, pNode);
   }

   for (const auto& item : jsonDagCollection.fileArray)
   {
      JSONDagCollection jsonChild;
      DealItem(jsonChild, item, dealFileCallback);

      AppendCollectionInternal(
         pDagCollection, 
         jsonChild,
         dealFileCallback,
         valueFactoryMap,
         calculateFactoryMap
         );
   }

   for (const auto& item : jsonDagCollection.calculateArray)
   {
      auto found = calculateFactoryMap.find(item.type);
      if (calculateFactoryMap.end() == found)
      {
         LOG_MESSAGE_ERROR("Failed to find node calculate type:%s", item.type.c_str());
         continue;
      }
      auto pNode = found->second(item.data);
      pDagCollection->AddDagNode(item.name, pNode);

      for (const auto& link : item.stackInput)
      {
         auto* pLinkInput = pDagCollection->GetDagNode(link);
         if (nullptr == pLinkInput)
         {
            LOG_MESSAGE_ERROR("Failed to link nodes:%s %s", item.name.c_str(), link.c_str());
            continue;
         }
         pDagCollection->LinkPush(pLinkInput, pNode.get());
      }

      for (int index = 0; index < (int)item.orderedInput.size(); ++index)
      {
         const auto name = item.orderedInput[index];
         auto* pLinkInput = pDagCollection->GetDagNode(name);
         if (nullptr == pLinkInput)
         {
            if (false == name.empty())
            {
               LOG_MESSAGE_ERROR("Failed to link index nodes:%s %d %s", item.name.c_str(), index, name.c_str());
            }
            //null indexed input is ok....
            continue;
         }
         pDagCollection->LinkIndex(index, pLinkInput, pNode.get());
      }
   }
}

std::shared_ptr< DagCollection > JSONDagCollection::Factory(
   const std::string& fileName,
   const std::function<const std::string(const std::string& fileName)>& dealFileCallback,
   const std::vector< std::pair< std::string, std::shared_ptr< iDagNode > > >& inbuiltDagValues,
   const std::map<std::string, ValueFactory>& valueFactoryMap,
   const std::map<std::string, ValueFactory>& calculateFactoryMap
   )
{
   std::shared_ptr< DagCollection > pResult = std::make_shared< DagCollection >();
   for (const auto& item : inbuiltDagValues)
   {
      pResult->AddDagNode(item.first, item.second);
   }
   AppendCollection(pResult, fileName, dealFileCallback, valueFactoryMap, calculateFactoryMap);
   return pResult;
}

void JSONDagCollection::AppendCollection(
   const std::shared_ptr< DagCollection >& pDagCollection,
   const std::string& fileName,
   const std::function<const std::string(const std::string& fileName)>& dealFileCallback,
   const std::map<std::string, ValueFactory>& valueFactoryMap,
   const std::map<std::string, ValueFactory>& calculateFactoryMap
   )
{
   const auto data = dealFileCallback(fileName);
   auto json = nlohmann::json::parse( data );
   JSONDagCollection jsonDagCollection;
   json.get_to(jsonDagCollection);

   AppendCollectionInternal(
      pDagCollection,
      jsonDagCollection,
      dealFileCallback,
      valueFactoryMap,
      calculateFactoryMap
      );
}

