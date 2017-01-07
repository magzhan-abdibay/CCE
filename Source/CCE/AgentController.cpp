// Fill out your copyright notice in the Description page of Project Settings.

#include "CCE.h"
#include "AgentController.h"
#include "Agent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Engine/TargetPoint.h"

AAgentController::AAgentController()
{
	AgentBlackBoardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("AgentBlackBoardComponent"));
	AgentBehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("AgentBehaviorTreeComponent"));
	
	LocationToGoKey = "LocationToGo";
	CurrentLocationTarget = 0;
}
void AAgentController::Possess(APawn * Pawn)
{
	Super::Possess(Pawn);

	AAgent* Agent = Cast<AAgent>(Pawn);

	if(Agent){
		if (Agent->GetBehaviorTree()->BlackboardAsset)
		{
			AgentBlackBoardComponent->InitializeBlackboard(*(Agent->GetBehaviorTree()->BlackboardAsset));
		}

		UGameplayStatics::GetAllActorsOfClass(GetWorld(),ATargetPoint::StaticClass(),LocationTargets);
		AgentBehaviorTreeComponent->StartTree(*Agent->GetBehaviorTree());
	}
	
}

