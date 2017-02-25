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
}
void AAgentController::Possess(APawn * Value)
{
	Super::Possess(Value);

	AAgent* Agent = Cast<AAgent>(Value);

	if(Agent){
		if (Agent->GetBehaviorTree()->BlackboardAsset)
		{
			AgentBlackBoardComponent->InitializeBlackboard(*(Agent->GetBehaviorTree()->BlackboardAsset));
		}
		AgentBehaviorTreeComponent->StartTree(*Agent->GetBehaviorTree());
	}
	
}

