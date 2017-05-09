// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CCE.h"
#include "CCEGameMode.h"
#include "SpectatorCamera.h"
#include <ctime>
#include "Agent.h"
#include "CCEGameState.h"

ACCEGameMode::ACCEGameMode()
{
	DefaultPawnClass = ASpectatorCamera::StaticClass();
	GameStateClass = ACCEGameState::StaticClass();
}
