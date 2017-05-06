// Fill out your copyright notice in the Description page of Project Settings.

#include "CCE.h"
#include "AgentController.h"
#include "Agent.h"
#include "AI/Navigation/NavigationPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Engine/TargetPoint.h"

AAgentController::AAgentController() {
  AgentBlackBoardComponent = CreateDefaultSubobject<UBlackboardComponent>(
      TEXT("AgentBlackBoardComponent"));
  AgentBehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(
      TEXT("AgentBehaviorTreeComponent"));
}

void AAgentController::Possess(APawn *Value) {
  Super::Possess(Value);

  AAgent *Agent = Cast<AAgent>(Value);
  /*
  if (Agent) {
    if (Agent->GetBehaviorTree()->BlackboardAsset) {
      AgentBlackBoardComponent->InitializeBlackboard(
          *(Agent->GetBehaviorTree()->BlackboardAsset));
    }
    AgentBehaviorTreeComponent->StartTree(*Agent->GetBehaviorTree());
  }
  */
  

 // AAgent *Agent = Cast<AAgent>(Value);

  FVector Dest = FVector(230, 230, 230);
  UNavigationPath* Path = UNavigationSystem::FindPathToLocationSynchronously(this, Agent->GetActorLocation(), Dest, Agent);

  if (Path && Path->IsValid())
  {
	  FAIMoveRequest Req;
	  Req.SetAcceptanceRadius(50);
	  Req.SetUsePathfinding(true);

	  AAIController* AiController = Cast<AAIController>(Agent->GetController());
	  if (AiController)
	  {
		  AiController->RequestMove(Req, Path->GetPath());
	  }
  }
}

/*
void AAgentController::OnPossess(APawn *Value) {
	Super::OnPossess(Value);

	AAgent *Agent = Cast<AAgent>(Value);

	FVector Dest = FVector(0, 0, 230);
	UNavigationPath* Path = UNavigationSystem::FindPathToLocationSynchronously(this, Agent->GetActorLocation(), Dest, Agent);

	if (Path && Path->IsValid())
	{
		FAIMoveRequest Req;
		Req.SetAcceptanceRadius(50);
		Req.SetUsePathfinding(true);

		AAIController* AiController = Cast<AAIController>(Agent->GetController());
		if (AiController)
		{
			AiController->RequestMove(Req, Path->GetPath());
		}
	}

}
*/
