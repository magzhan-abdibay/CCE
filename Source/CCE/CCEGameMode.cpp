// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CCE.h"
#include "CCEGameMode.h"
#include "CCECharacter.h"

ACCEGameMode::ACCEGameMode()
{
	// set default pawn class
	DefaultPawnClass = ACCECharacter::StaticClass();
}
