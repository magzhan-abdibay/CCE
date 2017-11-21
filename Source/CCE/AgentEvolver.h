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
#include "Agent.h"
#include "AgentController.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "AgentEvolver.generated.h"

UCLASS()

class CCE_API AAgentEvolver : public AActor
{
	GENERATED_BODY()

	/** What spawn. Subclass of AAgent*/
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AAgent> WhatToSpawn;

	class UBoxComponent* WhereToSpawn;
public:
	AAgentEvolver();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE class UBoxComponent* GetWhereToSpawn() const { return WhereToSpawn; }

private:
	//TODO: make it editable from UEditor
	FString FileNeatParamsPath = FPaths::GameConfigDir().Append("\\NEAT\\NeatVariables.ne");
	FString FileStartGenomePath = FPaths::GameConfigDir().Append("\\NEAT\\StartGene");
	FString FileWinnerPopulationPath = FPaths::GameConfigDir().Append("\\NEAT\\WinnerPopulation");
	FString FileLastPopulationPath = FPaths::GameConfigDir().Append("\\NEAT\\LastPopulation");
	FString FilePopulationToReadPath = FPaths::GameConfigDir().Append("\\NEAT\\LastPopulation-Last");
	FString FileInitialPopulationPath = FPaths::GameConfigDir().Append("\\NEAT\\Population");

	std::vector<AAgentController*> AgentControllers;
	NEAT::Population* Population;
	int CompatAdjustFrequency = 0;
	int NumSpeciesTarget = 0;
	int OffspringCount = 0;
	int8 TicksFromLastCalculate = 0;
	int8 const CalculatingFrequencyInTicks = 60;
	bool WinnnerFound = false;

	void NeatInit();

	void NeatTick(int count);

	void NeatTick();

	NEAT::Population* ReadPopulation(char* filePath);

	NEAT::Genome* ReadGenome(char* filePath) const;

	NEAT::Population* GeneratePopulation(NEAT::Genome* startGenome);

	AAgentController* AttachOrganismToAgentController(AAgentController* AgentController, NEAT::Organism* Org);

	AAgentController* FindAgentControllerByNeatOrganism(NEAT::Organism* Org);

	static bool EvaluateAgentController(AAgentController* AgentController);

	AAgent* SpawnAgent(int8 Team);
};

