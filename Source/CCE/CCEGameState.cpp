// Fill out your copyright notice in the Description page of Project Settings.

#include "CCE.h"
#include "CCEGameState.h"
#include <string>

ACCEGameState::ACCEGameState() { PrimaryActorTick.bCanEverTick = true; }

void ACCEGameState::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  // GEngine->AddOnScreenDebugMessage(-1, 7.f,
  // FColor::Blue,FString::FromInt(score++));
}
