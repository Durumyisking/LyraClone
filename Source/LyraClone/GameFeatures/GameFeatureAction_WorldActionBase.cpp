#include "GameFeatureAction_WorldActionBase.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_WorldActionBase) // gen.cpp파일을 Inline화 한다.

void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	// 엔진 내 월드 순회하면서
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		// For문 변수인 현재 WorldContext랑, 해당함수를 호출한 WorldContext가 일치하는지 확인
		// 각 월드 내의 요소들에게 작용하려면 Context를 알아야 하기 때문에 AddToWorld의 인자로 넘겨준 것으로 보인다.
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			AddToWorld(WorldContext, Context);
		}
	}
}