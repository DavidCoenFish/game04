#pragma once

class iDag2Node;

class Dag2Collection
{
public:
	Dag2Collection();
	~Dag2Collection();

	static const std::string MakeUniqueName();

	void AddNode(const std::string& name, const std::shared_ptr< iDag2Node >& in_dagNode);
	void RemoveNode(const std::string& name);

	iDag2Node* const GetNode(const std::string& name) const;

	static void AddLinkStack(iDag2Node* const pInput, iDag2Node* const pOutput);
	static void RemoveLinkStack(iDag2Node* const pInput, iDag2Node* const pOutput);
	static void SetLinkIndex(iDag2Node* const pInputOrNullptr, const int inputIndex, iDag2Node* const pOutput);

private:
	std::map< std::string, std::shared_ptr< iDag2Node > > m_dagNodeMap;

};
