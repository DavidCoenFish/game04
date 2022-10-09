#include "UnitTestPCH.h"
#include "Common\DAG2\Dag2Collection.h"
#include "Common\DAG2\Dag2Value.h"
#include "Common\DAG2\Dag2.h"
#include "Common\DAG2\Dag2ValueHelper.h"
#include "Common\DAG2\Dag2NodeCalculate.h"
#include "Common\DAG2\Dag2NodeHelper.h"
#include "Common\DAG2\Dag2NodeVariable.h"
#include "Common\DAG2\iDag2Node.h"
#include "Common\DAG2\iDag2Value.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonDag2
{
	TEST_CLASS(Sanity)
	{
	public:
		static void CalculateStackSum(
			std::shared_ptr< iDag2Value >& pResult,
			const std::vector< iDag2Value* >& arrayStack, 
			const std::vector< iDag2Value* >& //arrayValue
			)
		{
			int sum = 0;
			for (auto iter : arrayStack)
			{
				sum += Dag2ValueHelper::Get<int>(iter);
				//Dag2ValueHelper::Get<int>(iter->GetValue());
			}

			Dag2ValueHelper::Assign(pResult, sum);

			return;
		}

		TEST_METHOD(Creation0)
		{
			auto pCollection = std::make_shared< Dag2Collection >();
			Assert::IsNotNull( pCollection.get() );

			{
				auto pNode = Dag2NodeVariable::Factory<int>(3);
				pCollection->AddNode( "n0", pNode );
			}
			pCollection.reset();
		}

		TEST_METHOD(Sum0)
		{
			auto pCollection = std::make_shared< Dag2Collection >();
			Assert::IsNotNull( pCollection.get() );

			{
				auto pNode = Dag2NodeVariable::Factory<int>(3);
				pCollection->AddNode( "n0", pNode );
			}
			{
				auto pNode = Dag2NodeVariable::Factory<int>(5);
				pCollection->AddNode( "n1", pNode );
			}
			{
				auto pNode = Dag2NodeCalculate::Factory(&CalculateStackSum);
				pCollection->AddNode( "n2", pNode );
			}

			pCollection->AddLinkStack(pCollection->GetNode("n0"), pCollection->GetNode("n2")); 
			pCollection->AddLinkStack(pCollection->GetNode("n1"), pCollection->GetNode("n2")); 

			auto pResultNode = pCollection->GetNode("n2");
			Assert::IsNotNull( pResultNode );
			//auto result = Dag2ValueHelper::Get<int>(pResultNode->GetValue());
			auto result = Dag2NodeHelper::GetNodeValue<int>(pResultNode);
			Assert::AreEqual(8, result );

			pCollection.reset();
		}

		TEST_METHOD(Dirty0)
		{
			auto pCollection = std::make_shared< Dag2Collection >();
			Assert::IsNotNull( pCollection.get() );

			iDag2Node* pInputNode = nullptr;
			{
				auto pNode = Dag2NodeVariable::Factory<int>(3, Dag2::DirtyCase::ValueChanged);
				pInputNode = pNode.get();
				pCollection->AddNode( "n0", pNode );
			}
			Dag2NodeCalculate* pCalculateNode = nullptr;
			{
				auto pNode1 = Dag2NodeCalculate::Factory(&CalculateStackSum);
				pCalculateNode = pNode1.get();
				pCollection->AddNode( "n2", pNode1 );
			}

			pCollection->AddLinkStack(pInputNode, pCalculateNode); 

			Assert::AreEqual( true, pCalculateNode->GetDirty() );

			{
				auto result = Dag2NodeHelper::GetNodeValue<int>(pCalculateNode);
				Assert::AreEqual(3, result );
			}

			Assert::AreEqual( false, pCalculateNode->GetDirty() );

			Dag2NodeHelper::SetNodeValue(pInputNode, 2);

			Assert::AreEqual( true, pCalculateNode->GetDirty() );

			{
				auto result = Dag2NodeHelper::GetNodeValue<int>(pCalculateNode);
				Assert::AreEqual(2, result );
			}

			Assert::AreEqual( false, pCalculateNode->GetDirty() );

			pCollection.reset();
		}

	};
}
