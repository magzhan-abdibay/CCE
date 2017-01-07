#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "SelectPoint.generated.h"


UCLASS()
class CCE_API USelectPoint : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
