#pragma once

#include "json/json.hpp"
class iDagNode;
class iDagValue;
class DagCollection;

struct JSONDagValue
{
   std::string name;
   std::string type;
   nlohmann::json data;

};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONDagValue, 
   name,
   type,
   data
   );

struct JSONDagCalculate
{
   std::string name;
   std::string type;
   //std::string function;
   std::vector< std::string > stackInput;
   std::vector< std::string > orderedInput;
   
   //std::vector< JSONDagOrderedInput > orderedInputArray;
   nlohmann::json data;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONDagCalculate, 
   name,
   type,
   stackInput,
   orderedInput,
   data
   );

struct JSONDagFile
{
   std::string file;
   nlohmann::json searchReplace;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONDagFile, 
   file,
   searchReplace
   );

struct JSONDagCollection
{
   typedef std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)> ValueFactory;
   typedef std::function< std::shared_ptr< iDagValue >(const std::vector< iDagNode* >&, const std::vector< iDagNode* >&, const std::shared_ptr< iDagValue >&) > CalculateFunction;

   std::vector<JSONDagValue> valueArray;
   std::vector<JSONDagFile> fileArray;
   std::vector<JSONDagCalculate> calculateArray;
   //std::vector<std::string> fileArray;

   static std::shared_ptr< DagCollection > Factory(
      const std::string& fileName,
      const std::function<const std::string(const std::string& fileName)>& dealFileCallback,
      const std::vector< std::pair< std::string, std::shared_ptr< iDagNode > > >& inbuiltDagValues,
      const std::map<std::string, ValueFactory>& valueFactoryMap,
      const std::map<std::string, ValueFactory>& calculateFactoryMap
      );
   static void AppendCollection(
      const std::shared_ptr< DagCollection >& pDagCollection,
      const std::string& fileName,
      const std::function<const std::string(const std::string& fileName)>& dealFileCallback,
      const std::map<std::string, ValueFactory>& valueFactoryMap,
      const std::map<std::string, ValueFactory>& calculateFactoryMap
      );

   //static void RegisterValue(const std::string& type, const std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>& factory);
   //static void RegisterCalculate(const std::string& function, const std::function<std::shared_ptr< iDagNode >(const nlohmann::json& data)>& factory);
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
   JSONDagCollection, 
   valueArray,
   fileArray,
   calculateArray
   );
