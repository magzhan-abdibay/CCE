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
	FORCEINLINE class UBoxComponent *GetWhereToSpawn() const {return WhereToSpawn;
}

private:
	/*Neat stuff*/
	NEAT::Population *Population = 0;
	
	CartPole *Cart = 0;
	
	int CompatAdjustFrequency = 0;
	
	bool Win = false;
	
	int NumSpeciesTarget = 0;
	
	int OffspringCount = 0;

	AAgent* SpawnAgent();

	void InitNeat();
	
	NEAT::Population* ReadPopulation(char * filePath);

	NEAT::Genome* ReadGenome(char * filePath);

	void SpawnInitialPopulation(NEAT::Genome* startGenome);

	void Pole2TestRealTime();

	int Pole2RealTimeLoop();

	void NeatTick(int count);

	bool Pole2Evaluate(NEAT::Organism *org);

	void DoStuff(NEAT::Network *Net,AAgent* Agent);

};
