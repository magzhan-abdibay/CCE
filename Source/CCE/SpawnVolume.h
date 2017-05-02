// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <algorithm>
#include <string>
#include "Neat.h"
#include "Network.h"
#include "Population.h"
#include "Organism.h"
#include "Genome.h"
#include "Species.h"
#include "CartPole.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class CCE_API ASpawnVolume : public AActor {
  GENERATED_BODY()
public:
  /** Sets default values for this actor's properties */
  ASpawnVolume();

  /** Called when the game starts or when spawned */
  virtual void BeginPlay() override;

  /**  Called every frame */
  virtual void Tick(float DeltaSeconds) override;

  /** Find a random point within the BoxComponent */
  UFUNCTION(BlueprintPure, Category = "Spawning")
  FVector GetRandomPointInVolume();

  /**Returns the WhereToSpawn subobject */
  FORCEINLINE class UBoxComponent *GetWhereToSpawn() const {
    return WhereToSpawn;
  }

protected:
  /** what spawn subclass of Agent*/
  UPROPERTY(EditAnywhere, Category = "Spawning")
  TSubclassOf<class AAgent> WhatToSpawn;

private:
  /** Box Component to specify where agents should be spawned */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning",
            meta = (AllowPrivateAccess = "true"))
  class UBoxComponent *WhereToSpawn;

  void SpawnAgent();

  /*Neat stuff*/
  void InitNeat();

  NEAT::Population *pole2TestRealTime();

  int pole2RealTimeLoop(NEAT::Population *pop, CartPole *cart);

  bool pole2Evaluate(NEAT::Organism *org, CartPole *cart);
};
