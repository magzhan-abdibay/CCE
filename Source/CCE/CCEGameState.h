// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "CCEGameState.generated.h"

/**
 * 
 */
UCLASS()
class CCE_API ACCEGameState : public AGameState
{
	GENERATED_BODY()
public:
	ACCEGameState();
	virtual void Tick(float DeltaTime) override;

private:
	int score = 0;
	
	
};
