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
	if (TicksFromLastCalculate++ > 4) {
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

	Population = GeneratePopulation(startGenome);
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
	std::vector<NEAT::Organism *>::iterator CurOrg;
	for (CurOrg = (pop->organisms).begin(); CurOrg != (pop->organisms).end(); ++CurOrg) {
		// shouldn't happen
		if (((*CurOrg)->gnome) == 0) {
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("ERROR EMPTY GENOME!")));
			return nullptr;
		}
		//Spawn Agent and Attach NN 
		AAgent* Agent = SpawnAgent();
		if (Agent) {
			AAgentController* AgentController = (AAgentController*)Agent->GetController();
			if (AgentController) {
				AgentController = AttachOrganismToAgentController(AgentController, (*CurOrg));
				AgentControllers.push_back(AgentController);
				if (EvaluateAgent(AgentController)) {
					Win = true;
				}
			}
		}

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

NEAT::Population* AAgentEvolver::GeneratePopulation(NEAT::Genome* startGenome) {

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

		AAgent* Agent = SpawnAgent();
		if (Agent) {
			AAgentController* AgentController =(AAgentController*) Agent->GetController();
			if (AgentController) {
				AgentController = AttachOrganismToAgentController(AgentController, (*CurOrg));
				AgentControllers.push_back(AgentController);
				if (EvaluateAgent(AgentController)) {
					Win = true;
				}
			} else {
				GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Blue, FString("AgentController null"));
			}
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Blue, FString("Agent null"));
		}
				
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

AAgentController* AAgentEvolver::AttachOrganismToAgentController(AAgentController* AgentController, NEAT::Organism *Org) {
	if (AgentController) {
		AgentController->SetNeatOrganism(Org);
		Org->fitness = AgentController->EvaluateFitness();
		return AgentController;
	}
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString("AttachOrganismToAgentController null"));
	return nullptr;
}

void AAgentEvolver::NeatTick(int OffspringCount) {

	if (Win)
		return;

	// Every popSize reproductions, adjust the compatThresh to better match the NumSpeciesTarget  and reassign the population to new species
	if (CompatAdjustFrequency && OffspringCount % CompatAdjustFrequency == 0) {

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
	AAgentController* AgentController = FindAgentControllerByNeatOrganism(WorstOrganism);
	if (AgentController){
		NEAT::Organism *NewOrg = (Population->chooseParentSpecies())->reproduceOne(OffspringCount, Population, Population->species);
		AgentController = AttachOrganismToAgentController(AgentController, NewOrg);
		// Now we reestimate the baby's species' fitness
		NewOrg->species->estimateAverage();
		if (EvaluateAgent(AgentController)) {
			Win = true;
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Black, FString::SanitizeFloat(Population->organisms.size()));
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Black, FString::SanitizeFloat(AgentControllers.size()));
	
	// Now we evaluate the new individual
	// Note that in a true real-time simulation, evaluation would be happening to
	// all individuals at all times.  That is, this call would not appear here in
	// a true online simulation.
	if (Win) {
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, "WINNER");
		char *PrintPopulationPath =
			"E:\\UnrealProjects\\CCE\\Config\\NEAT\\WinnerPopulation";
		Population->printToFileBySpecies(PrintPopulationPath);
		return;
	}




}


AAgentController* AAgentEvolver::FindAgentControllerByNeatOrganism(NEAT::Organism* Org) {
	for (std::vector<AAgentController *>::iterator ÑurAgentConntoller = AgentControllers.begin(); ÑurAgentConntoller != AgentControllers.end(); ++ÑurAgentConntoller) {
		if ((*ÑurAgentConntoller)->GetNeatOrganism() == Org) {
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString("SUUUC"));
			return (*ÑurAgentConntoller);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString("NUUUUUUUUUUUUULLL"));
	return nullptr;
}

bool AAgentEvolver::EvaluateAgent(AAgentController* AgentController) {
	if (AgentController) {
		NEAT::Organism *Org = AgentController->GetNeatOrganism();
		Org->fitness = AgentController->EvaluateFitness();
		if (Org->fitness >= 1) {
			Org->winner = true;
			return true;
		}
		else {
			Org->winner = false;
			return false;
		}
	}
	return false;
}

AAgent* AAgentEvolver::SpawnAgent() {
	if (WhatToSpawn) {
		UWorld *const World = GetWorld();
		if (World) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			SpawnParams.bNoCollisionFail = true;

			FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
			FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;
			FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);

			FRotator SpawnRotation(0, FMath::FRand() * 360.0f, 0);

			AAgent *const SpawnedActor = World->SpawnActor<AAgent>(
				WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
			if (!SpawnedActor) {
				GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString("SpawnAgent is null"));
			}
			return SpawnedActor;
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString("SpawnAgent is null"));
	return nullptr;
}

