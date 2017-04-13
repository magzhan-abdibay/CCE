// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CCE.h"
#include "CCEGameMode.h"
#include "SpectatorCamera.h"
#include <ctime>
#include "Agent.h"

ACCEGameMode::ACCEGameMode()
{
	DefaultPawnClass = AAgent::StaticClass();
}
