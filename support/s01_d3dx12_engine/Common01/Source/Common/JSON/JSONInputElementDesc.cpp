#include "CommonPCH.h"
#include "Common/JSON/JSONInputElementDesc.h"

enum class JSONSemantic
{
   Invalid = 0,
   BINORMAL,
   BLENDINDICES,
   BLENDWEIGHT,
   COLOR,
   NORMAL,
   POSITION,
   PSIZE,
   TANGENT,
   TEXCOORD
};
NLOHMANN_JSON_SERIALIZE_ENUM( JSONSemantic, {
   {JSONSemantic::Invalid, nullptr},
   {JSONSemantic::BINORMAL, "BINORMAL"},
   {JSONSemantic::BLENDINDICES, "BLENDINDICES"},
   {JSONSemantic::BLENDWEIGHT, "BLENDWEIGHT"},
   {JSONSemantic::COLOR, "COLOR"},
   {JSONSemantic::NORMAL, "NORMAL"},
   {JSONSemantic::POSITION, "POSITION"},
   {JSONSemantic::PSIZE, "PSIZE"},
   {JSONSemantic::TANGENT, "TANGENT"},
   {JSONSemantic::TEXCOORD, "TEXCOORD"}
   });

struct JSONInputElementDescData
{
   JSONSemantic semantic;
   DXGI_FORMAT format;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JSONInputElementDescData, semantic, format);

struct JSONInputElementDescWrapper
{
   std::vector< JSONInputElementDescData > inputElementDesc;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(JSONInputElementDescWrapper, inputElementDesc);

//std::vector<D3D12_INPUT_ELEMENT_DESC> JSONInputElementDesc_from_json(
//   const nlohmann::json& json
//   )
void from_json(const nlohmann::json& j, std::vector<D3D12_INPUT_ELEMENT_DESC>& p)
{
   p.clear();

   //auto wrapper = json.get<JSONInputElementDescWrapper>();
   std::vector< JSONInputElementDescData > inputElementDesc;
   j.get_to(inputElementDesc);

   std::map< JSONSemantic, int > mapSemanticUsage;
   for (const auto& item : inputElementDesc)
   {
      UINT semanticIndex = 0;
      auto found = mapSemanticUsage.find(item.semantic);
      if (found != mapSemanticUsage.end())
      {
         found->second += 1;
         semanticIndex = found->second;
      }
      else
      {
         mapSemanticUsage[item.semantic] = 0;
      }
      D3D12_INPUT_ELEMENT_DESC desc = {
         get_enum_char(item.semantic),
         semanticIndex,// UINT SemanticIndex;
         item.format,
         0, // UINT InputSlot;
         D3D12_APPEND_ALIGNED_ELEMENT,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         0 // UINT InstanceDataStepRate;
      };
      p.push_back(desc);
   }
   return;
}

