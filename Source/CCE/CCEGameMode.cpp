// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CCE.h"
#include "CCEGameMode.h"
#include "SpectatorCamera.h"

ACCEGameMode::ACCEGameMode()
{
	DefaultPawnClass = ASpectatorCamera::StaticClass();
}
