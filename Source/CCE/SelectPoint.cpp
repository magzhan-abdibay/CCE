// Fill out your copyright notice in the Description page of Project Settings.

#include "CCE.h"
#include "SelectPoint.h"
#include "AgentController.h"
#include "Engine/TargetPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <string>
#include <ctime>

USelectPoint::USelectPoint() {
	int64 DateInSeconds = FDateTime::Now().ToUnixTimestamp();
	FRandomStream SRand = FRandomStream();
	SRand.Initialize(DateInSeconds);
}

EBTNodeResult::Type USelectPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAgentController* Controller = Cast<AAgentController>(OwnerComp.GetAIOwner());
	if (Controller)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Controller->GetCharacter()->GetActorLocation().ToString());
		FVector agentPosition=Controller->GetCharacter()->GetActorLocation();
		FVector targetPosition = GenerateTargetVector(agentPosition);
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Blue, targetPosition.ToString());
		UBlackboardComponent* BlackBoard = Controller->GetAgentBlackBoardComponent();
		BlackBoard->SetValueAsVector("TargetPosition", targetPosition);
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, BlackBoard->GetValueAsVector("TargetPosition").ToString());
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

FVector USelectPoint::GenerateTargetVector(FVector& value) const
{
	int32 orientation = FMath::RandRange(1, 4);
	int32 moveStep = 500;
	FVector result;
	switch (orientation)
	{
	case 1:
	
		result = FVector::ForwardVector*moveStep;
		break;
	case 2:
		result = -FVector::ForwardVector*moveStep;
		break;
	case 3:
		result = FVector::RightVector*moveStep;
		break;
	case 4:
		result = -FVector::RightVector*moveStep;
		break;
	default:
		result = FVector::ZeroVector;
	}
	result += value;
	return result;
}