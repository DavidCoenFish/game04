#include "UnitTestPCH.h"
#include "Common\DAG2\Dag2Collection.h"
#include "Common\DAG2\Dag2Value.h"
#include "Common\DAG2\Dag2.h"
#include "Common\DAG2\Dag2ValueHelper.h"
#include "Common\DAG2\Dag2NodeCalculate.h"
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
			const std::vector< iDag2Node* >& arrayStack, 
			const std::vector< iDag2Node* >& //arrayValue
			)
		{
			int sum = 0;
			for (auto iter : arrayStack)
			{
				auto pValue = iter->GetValue();
				auto pValueType = dynamic_cast< Dag2Value<int>* >(pValue);
				if (pValueType)
				{
					sum += pValueType->Get();
				}
			}

			Dag2ValueHelper::Assign(pResult, sum);

			return;
		}

		TEST_METHOD(SanityCreation0)
		{
			auto pCollection = std::make_shared< Dag2Collection >();
			Assert::IsNotNull( pCollection.get() );

			{
				auto pNode = Dag2NodeVariable::Factory<int>(3);
				pCollection->AddNode( "n0", pNode );
			}
			pCollection.reset();
		}

		TEST_METHOD(SanityCreation1)
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
				auto pNode = Dag2Calculate::Factory(&CalculateStackSum);
				pCollection->AddNode( "n2", pNode );
			}

			pCollection->AddLinkStack(pCollection->GetNode("n0"), pCollection->GetNode("n2")); 
			pCollection->AddLinkStack(pCollection->GetNode("n1"), pCollection->GetNode("n2")); 

			auto pResultNode = pCollection->GetNode("n2");
			Assert::IsNotNull( pResultNode );
			auto result = Dag2ValueHelper::Get<int>(pResultNode->GetValue());
			Assert::AreEqual(8, result );

			pCollection.reset();
		}

	};
}
