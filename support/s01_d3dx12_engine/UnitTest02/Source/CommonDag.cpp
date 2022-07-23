#include "UnitTestPCH.h"
#include "Common\DAG\DagCollection.h"
#include "Common\DAG\IDagValue.h"
#include "Common\DAG\DagNodeValue.h"
#include "Common\DAG\DagNodeCalculate.h"
#include "Common\DAG\DagValueVariant.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CommonDag
{
	TEST_CLASS(Sanity)
	{
	public:
		static std::shared_ptr< iDagValue > CalculateStackSum(
			const std::vector< iDagNode* >& arrayStack, 
			const std::vector< iDagNode* >& ,//arrayValue, 
			const std::shared_ptr< iDagValue >& value
			)
		{
			std::shared_ptr< iDagValue > pResult = value;
			auto pResultType = pResult ? dynamic_cast< DagValueVariant<int>* >(pResult.get()) : nullptr;
			if (nullptr == pResultType)
			{
				auto pTemp = std::make_shared< DagValueVariant<int> >(0);
				pResult = pTemp;
				pResultType = pTemp.get();
			}

			int sum = 0;
			for (auto iter : arrayStack)
			{
				auto pValue = iter->GetValue();
				auto pValueType = dynamic_cast< DagValueVariant<int>* >(pValue.get());
				if (pValueType)
				{
					sum += pValueType->Get<int>();
				}
			}
			if (pResultType)
			{
				pResultType->Set( sum );
			}

			return pResult;
		}

		TEST_METHOD(SanityCreation0)
		{
			//Logger::WriteMessage(L"Hello world");
			auto pCollection = std::make_shared< DagCollection >();
			Assert::IsNotNull( pCollection.get() );

			{
				auto pNode0 = std::make_shared< DagNodeValue >( std::make_shared< DagValueVariant<int> >(4) );
				pCollection->AddDagNode( "n0", pNode0 );
			}
			pCollection.reset();
		}

		TEST_METHOD(SanityCreation1)
		{
			auto pCollection = std::make_shared< DagCollection >();
			Assert::IsNotNull( pCollection.get() );

			{
				auto pNode0 = std::make_shared< DagNodeValue >( std::make_shared< DagValueVariant<int> >(4) );
				pCollection->AddDagNode( "n0", pNode0 );
			}
			{
				auto pNode1 = std::make_shared< DagNodeValue >( std::make_shared< DagValueVariant<int> >(5) );
				pCollection->AddDagNode( "n1", pNode1 );
			}
			{
				auto pNode2 = std::make_shared< DagNodeCalculate >( &CalculateStackSum );
				pCollection->AddDagNode( "n2", pNode2 );
			}

			pCollection->LinkPush(pCollection->GetDagNode("n0"), pCollection->GetDagNode("n2")); 
			pCollection->LinkPush(pCollection->GetDagNode("n1"), pCollection->GetDagNode("n2")); 

			auto pResult = pCollection->GetDagNode("n2")->GetValue();
			auto pResultType = dynamic_cast< DagValueVariant<int>* >(pResult.get());
			Assert::IsNotNull( pResultType );
			Assert::AreEqual(9, pResultType->Get<int>() );

			pCollection.reset();
		}

		TEST_METHOD(CheckDirty)
		{
			auto pCollection = std::make_shared< DagCollection >();
			Assert::IsNotNull( pCollection.get() );

			{
				auto pNode0 = std::make_shared< DagNodeValue >( std::make_shared< DagValueVariant<int> >(6) );
				pCollection->AddDagNode( "n0", pNode0 );
			}
			{
				auto pNode2 = std::make_shared< DagNodeCalculate >( &CalculateStackSum );
				pCollection->AddDagNode( "n1", pNode2 );
			}

			pCollection->LinkPush(pCollection->GetDagNode("n0"), pCollection->GetDagNode("n1")); 

			{
				auto pResult = pCollection->GetDagNode("n1")->GetValue();
				auto pResultType = dynamic_cast< DagValueVariant<int>* >(pResult.get());
				Assert::IsNotNull( pResultType );
				Assert::AreEqual(6, pResultType->Get<int>() );
			}
			{
				auto pNode = pCollection->GetDagNode("n0");
				//auto pValue = pNode->GetValue();
				//auto pValueType = dynamic_cast< DagValueVariant<int>* >(pValue.get());
				//if (pValueType)
				//{
				//	pValueType->Set(7);
				//}
				pNode->SetValue(std::make_shared< DagValueVariant<int> >(7));
			}
			{
				auto pResult = pCollection->GetDagNode("n1")->GetValue();
				auto pResultType = dynamic_cast< DagValueVariant<int>* >(pResult.get());
				Assert::IsNotNull( pResultType );
				Assert::AreEqual(7, pResultType->Get<int>() );
			}



		}


	};
}
