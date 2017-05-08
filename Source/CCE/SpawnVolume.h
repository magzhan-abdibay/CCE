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

#include "Agent.h"
#include "AgentController.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class CCE_API ASpawnVolume : public AActor {
	GENERATED_BODY()
	
	/** What spawn. Subclass of AAgent*/
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AAgent> WhatToSpawn;

	/** Box Component to specify where agents should be spawned */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning",
	meta = (AllowPrivateAccess = "true"))
	class UBoxComponent *WhereToSpawn;

public:
	ASpawnVolume();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Destroyed() override;

	/** Find a random point within the BoxComponent */
	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetRandomPointInVolume();

	/** Returns the WhereToSpawn subobject */
	FORCEINLINE class UBoxComponent *GetWhereToSpawn() const {return WhereToSpawn;}

private:
	/*Neat stuff*/
	NEAT::Population *Population = 0;
		
	int CompatAdjustFrequency = 0;
	
	bool Win = false;
	
	int NumSpeciesTarget = 0;
	
	int OffspringCount = 0;

	int TicksFromLastCalculate = 0;

	void InitNeat();
	
	NEAT::Population* ReadPopulation(char * filePath);

	NEAT::Genome* ReadGenome(char * filePath);

	AAgent* SpawnAgent();

	NEAT::Population* SpawnInitialPopulation(NEAT::Genome* startGenome);

	bool EvaluateAgent(AAgentController* AgentController);

	void NeatTick(int count);

	AAgentController* SpawnAgentAndAttachNeatOrganism(NEAT::Organism *Org);
};
