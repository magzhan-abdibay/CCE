// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTreeSelectPoint.generated.h"

/**
 * 
 */
UCLASS()
class CCE_API UBehaviorTreeSelectPoint : public UBTTaskNode
{
	GENERATED_BODY()


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
