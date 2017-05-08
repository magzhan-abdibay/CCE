#include "CCE.h"
#include "SpawnVolume.h"

ASpawnVolume::ASpawnVolume() {
  PrimaryActorTick.bCanEverTick = true;

  // Create the Box Component to represent the spawn volume
  WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
  RootComponent = WhereToSpawn;
}

void ASpawnVolume::BeginPlay() { 
	Super::BeginPlay(); 
	
	// Init NEAT
	InitNeat();

	//ReadPopulation("E:\\UnrealProjects\\CCE\\Config\\NEAT\\Population");
}

void ASpawnVolume::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
  if (TicksFromLastCalculate++ > 60) {
	  TicksFromLastCalculate = 0;
	  NeatTick(OffspringCount++);
  }
}

FVector ASpawnVolume::GetRandomPointInVolume() {
  FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
  FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;
  return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
}

AAgent* ASpawnVolume::SpawnAgent() {
  // If we have set something to spawn:
  if (WhatToSpawn != NULL) {
    // Check for a valid World:
    UWorld *const World = GetWorld();
    if (World) {
      // Set the spawn parameters
      FActorSpawnParameters SpawnParams;
      SpawnParams.Owner = this;
      SpawnParams.Instigator = Instigator;

      // Get a random location to spawn at
      FVector SpawnLocation = GetRandomPointInVolume();

      // Get a random rotation for the spawned item
      FRotator SpawnRotation(0, FMath::FRand() * 360.0f, 0);

      // spawn
      AAgent *const SpawnedActor = World->SpawnActor<AAgent>(
          WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

	  return SpawnedActor;
    }
  }
  return NULL;
}

NEAT::Population* ASpawnVolume::ReadPopulation(char * filePath) {

	NEAT::Population *pop = new NEAT::Population(filePath);
	
	pop->verifyPopulation();

	// Initially, we evaluate the whole population
	// Evaluate each organism on a test
	vector<NEAT::Organism *>::iterator curOrg;
	for (curOrg = (pop->organisms).begin(); curOrg != (pop->organisms).end(); ++curOrg) {
		// shouldn't happen
		if (((*curOrg)->gnome) == 0) {
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("ERROR EMPTY GENOME!")));
			return NULL;
		}
		//Spawn Agent and Attach NN 
		AAgentController* AgentController=SpawnAgentAndAttachNeatOrganism((*curOrg));
		(*curOrg)->fitness = AgentController->EvaluateFitness();
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

void ASpawnVolume::InitNeat() {
  char *NeatParamsPath =
      "E:\\UnrealProjects\\CCE\\Config\\NEAT\\NeatVariables.ne";
  NEAT::loadNeatParams(NeatParamsPath, true);

  char *GenomeParamsPath =
	  "E:\\UnrealProjects\\CCE\\Config\\NEAT\\StartGene";
  NEAT::Genome *startGenome = ReadGenome(GenomeParamsPath);

  Population=SpawnInitialPopulation(startGenome);

  char *PrintPopulationPath =
	  "E:\\UnrealProjects\\CCE\\Config\\NEAT\\WinnerPopulation";
  Population->printToFileBySpecies(PrintPopulationPath);
}

NEAT::Genome* ASpawnVolume::ReadGenome(char * filePath) {
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, "START DOUBLE POLE BALANCING REAL-TIME EVOLUTION VALIDATION");
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, "Reading in the start genome");
	char curWord[20];
	int id;
	ifstream iFile(filePath,ios::in);
	iFile >> curWord;
	iFile >> id;
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString(TEXT("Reading in Genome id ")) + FString::FromInt(id));

	NEAT::Genome *startGenome = new NEAT::Genome(id, iFile);
	iFile.close();
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString(TEXT("Start Genome: ")) + FString::FromInt(id));
	return startGenome;
}

NEAT::Population* ASpawnVolume::SpawnInitialPopulation(NEAT::Genome* startGenome) {

	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString(TEXT("Spawning Population off Genome")));

	NEAT::Population* SpawnedPopulation = new NEAT::Population(startGenome, NEAT::popSize);
	
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("Verifying Spawned Pop")));
	SpawnedPopulation->verifyPopulation();

	// We try to keep the number of species constant at this number
	NumSpeciesTarget = NEAT::popSize / 15;

	// This is where we determine the frequency of compatibility threshold
	// adjustment
	CompatAdjustFrequency = NEAT::popSize / 10;
	if (CompatAdjustFrequency < 1)
		CompatAdjustFrequency = 1;

	// Initially, we evaluate the whole population
	// Evaluate each organism on a test
	vector<NEAT::Organism *>::iterator curOrg;
	for (curOrg = (SpawnedPopulation->organisms).begin(); curOrg != (SpawnedPopulation->organisms).end(); ++curOrg) {
		// shouldn't happen
		if (((*curOrg)->gnome) == 0) {
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("ERROR EMPTY GENOME!")));
			return NULL;
		}
		
		AAgentController* AgentController = SpawnAgentAndAttachNeatOrganism((*curOrg));
		(*curOrg)->fitness = AgentController->EvaluateFitness();
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

AAgentController* ASpawnVolume::SpawnAgentAndAttachNeatOrganism(NEAT::Organism *Org) {
	AAgent* Agent = SpawnAgent();
	AAgentController* AgentController = (AAgentController*)Agent->GetController();
	AgentController->SetNeatOrganism(Org);
	return AgentController;
}

void ASpawnVolume::NeatTick(int count) {

  if (Win)
    return;
  if (CompatAdjustFrequency == 0)
    return;

  // Every popSize reproductions, adjust the compatThresh to better match the numSpeciesTarger  and reassign the population to new species
  if (count % CompatAdjustFrequency == 0) {
    int numSpecies = (int)Population->species.size();
    double compatMod = 0.1; // Modify compat thresh to control speciation

    // This tinkers with the compatibility threshold
    if (numSpecies < NumSpeciesTarget) {
      NEAT::compatThreshold -= compatMod;
    } else if (numSpecies > NumSpeciesTarget)
      NEAT::compatThreshold += compatMod;

    if (NEAT::compatThreshold < 0.3)
      NEAT::compatThreshold = 0.3;

	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("compatThreshold = "))+FString::FromInt(NEAT::compatThreshold));

    // Go through entire population, reassigning organisms to new species
    vector<NEAT::Organism *>::iterator curOrg;
    for (curOrg = (Population->organisms).begin();
         curOrg != Population->organisms.end(); ++curOrg) {
      Population->reassignSpecies(*curOrg);
    }
  }

  // For printing only
  //vector<NEAT::Species *>::iterator curSpec;
  //for (curSpec = (Population->species).begin();
  //     curSpec != (Population->species).end(); curSpec++) {
  // GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("Species "))+FString::FromInt((*curSpec)->id));
  // GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("size ")) + FString::FromInt((*curSpec)->organisms.size()));
  //	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("average= ")) + FString::FromInt((*curSpec)->avgEst));
  //}
  //GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("Pop size: ")) + FString::FromInt(Population->organisms.size()));

  // Here we call two rtNEAT calls:
  // 1) chooseParentSpecies() decides which species should produce the next
  // offspring  2) reproduceOne(...) creates a single offspring from the chosen
  // species
  
  NEAT::Organism *newOrg =(Population->chooseParentSpecies())->reproduceOne(count, Population, Population->species);
  AAgentController* AgentController = SpawnAgentAndAttachNeatOrganism(newOrg);
  newOrg->fitness = AgentController->EvaluateFitness();
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
  newOrg->species->estimateAverage();

  // Remove the worst organism
  //Population->removeWorst();
}

bool ASpawnVolume::EvaluateAgent(AAgentController* AgentController) {
	NEAT::Organism *Org = AgentController->GetNeatOrganism();
	NEAT::Network *Net= Org->net;
	Org->fitness = AgentController->EvaluateFitness();
	
	if (Org->fitness >= 2) {
		Org->winner = true;
		return true;
	}
	else {
		Org->winner = false;
		return false;
	}
}

void ASpawnVolume::Destroyed() {
  Super::Destroyed();
  if (Population) {
    delete Population;
  }
}
