// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "AgentController.generated.h"

/**
 * 
 */
UCLASS()
class CCE_API AAgentController : public AAIController
{
	GENERATED_BODY()

	UBlackboardComponent* AgentBlackBoardComponent;
	UBehaviorTreeComponent* AgentBehaviorTreeComponent;

	UPROPERTY(EditDefaultsOnly, Category = Behavior)
		FName LocationToGoKey;
	
	TArray<AActor*> LocationTargets;
	int32 CurrentLocationTarget;

	virtual void Possess(APawn* Pawn) override;
	
public:
	AAgentController();

	FORCEINLINE UBlackboardComponent* GetAgentBlackBoardComponent() const { return AgentBlackBoardComponent; }
	FORCEINLINE UBehaviorTreeComponent* GetAgentBehaviorTreeComponent() const { return AgentBehaviorTreeComponent; }
	FORCEINLINE TArray<AActor*> GetLocationTargets() const { return LocationTargets; }
	FORCEINLINE int32 GetCurrentLocationTarget() const { return CurrentLocationTarget; }
	FORCEINLINE void SetCurrentLocationTarget(int32 value) { CurrentLocationTarget=value; }

};
