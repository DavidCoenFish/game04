#pragma once

class iDagNode;

class DagCollection
{
public:
   DagCollection();
   ~DagCollection();

   static const std::string MakeUniqueName();

   void AddDagNode(const std::string& name, const std::shared_ptr< iDagNode >& in_dagNode);
   void RemoveDagNode(const std::string& name);

   iDagNode* const GetDagNode(const std::string& name) const;

   void LinkPush( iDagNode* const pInput, iDagNode* const pOutput );
   void LinkRemove( iDagNode* const pInput, iDagNode* const pOutput );

   void LinkIndex( const int inputIndex, iDagNode* const pInputOrNullptr, iDagNode* const pOutput );

private:
   std::map< std::string, std::shared_ptr< iDagNode > > m_dagNodeMap;

};
