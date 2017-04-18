// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CCE.h"
#include "CCEGameMode.h"
#include "SpectatorCamera.h"
#include <ctime>
#include "Agent.h"
#include "CCEGameState.h"

ACCEGameMode::ACCEGameMode()
{
	DefaultPawnClass = AAgent::StaticClass();
	GameStateClass = ACCEGameState::StaticClass();
}

/*
void ACCEGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Blue, "asas");


}
*/
