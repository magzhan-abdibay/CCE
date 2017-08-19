// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CCE.h"
#include "CCEGameMode.h"
#include "CCEGameState.h"
#include "Agent.h"

ACCEGameMode::ACCEGameMode()
{
	DefaultPawnClass = AAgent::StaticClass();
	GameStateClass = ACCEGameState::StaticClass();
}
