#include "CCE.h"
#include "AgentEvolver.h"

AAgentEvolver::AAgentEvolver() {
	PrimaryActorTick.bCanEverTick = true;

	WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
	RootComponent = WhereToSpawn;
}

void AAgentEvolver::BeginPlay() {
	Super::BeginPlay();
	InitNeat();

	//ReadPopulation("E:\\UnrealProjects\\CCE\\Config\\NEAT\\Population");
}

void AAgentEvolver::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (TicksFromLastCalculate++ > 60) {
		TicksFromLastCalculate = 0;
		NeatTick(OffspringCount++);
	}
}

void AAgentEvolver::InitNeat() {
	char *NeatParamsPath =
		"E:\\UnrealProjects\\CCE\\Config\\NEAT\\NeatVariables.ne";
	NEAT::loadNeatParams(NeatParamsPath, true);

	char *GenomeParamsPath =
		"E:\\UnrealProjects\\CCE\\Config\\NEAT\\StartGene";
	NEAT::Genome *startGenome = ReadGenome(GenomeParamsPath);

	Population = SpawnInitialPopulation(startGenome);

	char *PrintPopulationPath =
		"E:\\UnrealProjects\\CCE\\Config\\NEAT\\WinnerPopulation";
	Population->printToFileBySpecies(PrintPopulationPath);
}

NEAT::Genome* AAgentEvolver::ReadGenome(char * FilePath) {
	UE_LOG(LogTemp, Warning, TEXT("Reading in the start genome"));
	char CurWord[20];
	int Id;
	std::ifstream InFile(FilePath, std::ios::in);
	InFile >> CurWord;
	InFile >> Id;

	UE_LOG(LogTemp, Warning, TEXT("Reading in Genome"));
	NEAT::Genome *startGenome = new NEAT::Genome(Id, InFile);
	InFile.close();
	return startGenome;
}

NEAT::Population* AAgentEvolver::ReadPopulation(char * filePath) {

	NEAT::Population *pop = new NEAT::Population(filePath);

	pop->verifyPopulation();

	// Initially, we evaluate the whole population
	// Evaluate each organism on a test
	std::vector<NEAT::Organism *>::iterator curOrg;
	for (curOrg = (pop->organisms).begin(); curOrg != (pop->organisms).end(); ++curOrg) {
		// shouldn't happen
		if (((*curOrg)->gnome) == 0) {
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("ERROR EMPTY GENOME!")));
			return nullptr;
		}
		//Spawn Agent and Attach NN 
		AAgentController* AgentController = AttachOrganismToAgentController(SpawnAgent(), (*curOrg));
		if (EvaluateAgent(AgentController))
			Win = true;

	}

	// Get ready for real-time loop
	// Rank all the organisms from best to worst in each species
	pop->rankWithinSpecies();

	// Assign each species an average fitness
	// This average must be kept up-to-date by rtNEAT in order to select species
	// probabailistically for reproduction
	pop->estimateAllAverages();

	return pop;
}

NEAT::Population* AAgentEvolver::SpawnInitialPopulation(NEAT::Genome* startGenome) {

	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString(TEXT("Spawning Population off Genome")));

	NEAT::Population* SpawnedPopulation = new NEAT::Population(startGenome, NEAT::popSize);
	SpawnedPopulation->verifyPopulation();

	// We try to keep the number of species constant at this number
	NumSpeciesTarget = NEAT::popSize / 15;

	// This is where we determine the frequency of compatibility threshold adjustment
	CompatAdjustFrequency = NEAT::popSize / 10;
	if (CompatAdjustFrequency < 1)
		CompatAdjustFrequency = 1;

	// Initially, we evaluate the whole population
	// Evaluate each organism on a test
	std::vector<NEAT::Organism *>::iterator CurOrg;
	for (CurOrg = (SpawnedPopulation->organisms).begin(); CurOrg != (SpawnedPopulation->organisms).end(); ++CurOrg) {
		// shouldn't happen
		if (((*CurOrg)->gnome) == 0) {
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("ERROR EMPTY GENOME!")));
			return nullptr;
		}

		AAgentController* AgentController = AttachOrganismToAgentController(SpawnAgent(), (*CurOrg));
		if (EvaluateAgent(AgentController))
			Win = true;
	}

	// Get ready for real-time loop
	// Rank all the organisms from best to worst in each species
	SpawnedPopulation->rankWithinSpecies();

	// Assign each species an average fitness
	// This average must be kept up-to-date by rtNEAT in order to select species
	// probabailistically for reproduction
	SpawnedPopulation->estimateAllAverages();

	return SpawnedPopulation;
}

AAgentController* AAgentEvolver::AttachOrganismToAgentController(AAgent* Agent, NEAT::Organism *Org) {
	if (Agent) {
		AAgentController* AgentController = (AAgentController*)Agent->GetController();
		if (AgentController) {
			AgentController->SetNeatOrganism(Org);
			Org->fitness = AgentController->EvaluateFitness();
			AgentControllers.push_back(AgentController);
			return AgentController;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("AgentController is null"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Agent is null"));
	}
	return nullptr;
}

void AAgentEvolver::NeatTick(int count) {

	if (Win)
		return;

	// Every popSize reproductions, adjust the compatThresh to better match the NumSpeciesTarget  and reassign the population to new species
	if (CompatAdjustFrequency && count % CompatAdjustFrequency == 0) {

		int NumSpecies = (int)Population->species.size();
		double CompatMod = 0.1; // Modify compat thresh to control speciation

								// This tinkers with the compatibility threshold
		if (NumSpecies < NumSpeciesTarget) {
			NEAT::compatThreshold -= CompatMod;
		}
		else if (NumSpecies > NumSpeciesTarget)
			NEAT::compatThreshold += CompatMod;

		if (NEAT::compatThreshold < 0.3)
			NEAT::compatThreshold = 0.3;

		// Go through entire population, reassigning organisms to new species
		std::vector<NEAT::Organism *>::iterator CurOrg;
		for (CurOrg = (Population->organisms).begin(); CurOrg != Population->organisms.end(); ++CurOrg) {
			Population->reassignSpecies(*CurOrg);
		}
	}
	NEAT::Organism* WorstOrganism = Population->removeWorst();
	NEAT::Organism *NewOrg = (Population->chooseParentSpecies())->reproduceOne(count, Population, Population->species);
	AAgentController* AgentController = FindAgentControllerByNeatOrganism(WorstOrganism);
	AAgent* Agent = AgentController->GetAgent();
	AgentController = AttachOrganismToAgentController(Agent, NewOrg);
	if (EvaluateAgent(AgentController))
		Win = true;

	// Now we evaluate the new individual
	// Note that in a true real-time simulation, evaluation would be happening to
	// all individuals at all times.  That is, this call would not appear here in
	// a true online simulation.
	if (Win) {
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, "WINNER");
		Population->printToFileBySpecies((char *)"rt_winpop");
		return;
	}

	// Now we reestimate the baby's species' fitness
	NewOrg->species->estimateAverage();

	// Remove the worst organism
	//Population->removeWorst();
	//FindWorstAgentController();
}

AAgentController* AAgentEvolver::FindWorstAgentController() {
	double AdjustedFitness;
	double MinFitness = 999999;
	AAgentController* WorstAgentController = nullptr;

	std::vector<AAgentController *>::iterator ÑurAgentConntoller;
	for (ÑurAgentConntoller = AgentControllers.begin(); ÑurAgentConntoller != AgentControllers.end(); ++ÑurAgentConntoller) {
		AdjustedFitness = ((*ÑurAgentConntoller)->GetNeatOrganism()->fitness) / ((*ÑurAgentConntoller)->GetNeatOrganism()->species->organisms.size());
		if ((AdjustedFitness < MinFitness) && (((*ÑurAgentConntoller)->GetNeatOrganism()->timeAlive) >= NEAT::timeAliveMinimum)) {
			MinFitness = AdjustedFitness;
			WorstAgentController = (*ÑurAgentConntoller);
		}
	}

	return WorstAgentController;
}

AAgentController* AAgentEvolver::FindAgentControllerByNeatOrganism(NEAT::Organism* Org) {
	std::vector<AAgentController *>::iterator ÑurAgentConntoller;
	for (ÑurAgentConntoller = AgentControllers.begin(); ÑurAgentConntoller != AgentControllers.end(); ++ÑurAgentConntoller) {
		if ((*ÑurAgentConntoller)->GetNeatOrganism() == Org)
			return (*ÑurAgentConntoller);
	}
	return nullptr;
}

bool AAgentEvolver::EvaluateAgent(AAgentController* AgentController) {
	if (AgentController) {
		NEAT::Organism *Org = AgentController->GetNeatOrganism();
		NEAT::Network *Net = Org->net;
		Org->fitness = AgentController->EvaluateFitness();
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString::SanitizeFloat(Org->fitness));
		if (Org->fitness >= 1) {
			Org->winner = true;
			return true;
		}
		else {
			Org->winner = false;
			return false;
		}
	}
	else {
		return false;
	}
}

FVector AAgentEvolver::GetRandomPointInVolume() {
	FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
	FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;
	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
}

AAgent* AAgentEvolver::SpawnAgent() {
	if (WhatToSpawn) {
		UWorld *const World = GetWorld();
		if (World) {
			FActorSpawnParameters SpawnParams;

			SpawnParams.Owner = this;

			SpawnParams.Instigator = Instigator;

			FVector SpawnLocation = GetRandomPointInVolume();

			FRotator SpawnRotation(0, FMath::FRand() * 360.0f, 0);

			AAgent *const SpawnedActor = World->SpawnActor<AAgent>(
				WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

			return SpawnedActor;
		}
	}
	return nullptr;
}

void AAgentEvolver::Destroyed() {
	Super::Destroyed();
	if (Population) {
		delete Population;
	}
}


