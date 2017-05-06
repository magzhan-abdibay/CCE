#include "CCE.h"
#include "SpawnVolume.h"
#include "Agent.h"
#include "AgentController.h"
#include "Kismet/KismetMathLibrary.h"

ASpawnVolume::ASpawnVolume() {
  PrimaryActorTick.bCanEverTick = true;

  // Create the Box Component to represent the spawn volume
  WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
  RootComponent = WhereToSpawn;
}

void ASpawnVolume::BeginPlay() { 
	Super::BeginPlay(); 
	
	// Init NEAT
	//InitNeat();

	ReadPopulation("E:\\UnrealProjects\\CCE\\Binaries\\Win64\\rt_winpop");
}

void ASpawnVolume::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);
  //NeatTick(OffspringCount++);
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

	  //GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, SpawnedActor->GetController()->GetClass()->GetName());
	  return SpawnedActor;
    }
  }
  return NULL;
}

NEAT::Population* ASpawnVolume::ReadPopulation(char * filePath) {

	NEAT::Population *pop = new NEAT::Population(filePath);
	
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString(TEXT("Verifying population")));
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
		//GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString(TEXT("Fitness: "))+FString::FromInt((*curOrg)->fitness));
		AAgent* SpwanedAgent=SpawnAgent();
		DoStuff((*curOrg)->net, SpwanedAgent);

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

void ASpawnVolume::DoStuff(NEAT::Network *Net, AAgent* Agent) {
	if (!Agent||!Net)
		return;

	AAgentController* AgentController=(AAgentController*)Agent->GetController();
	AgentController->SetNeuralNetwork(Net);

	double input[7];
	input[0] = 0.8 / 4.8;
	input[1] = 1 / 2;
	input[2] = 0.5 / 0.52;
	input[3] = 1.5 / 2;
	input[4] = 1 / 0.52;
	input[5] = 1 / 2;
	input[6] = .5;

	AgentController->GetNeuralNetwork()->loadSensors(input);

	//Activate the net
	//If it loops, exit returning only fitness of 1 step
	if (!(AgentController->GetNeuralNetwork()->activate())) return;

	double output = (*(AgentController->GetNeuralNetwork()->outputs.begin()))->activation;

	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Emerald, FString::SanitizeFloat(output));
}

void ASpawnVolume::InitNeat() {
  char *NeatParamsPath =
      "E:\\UnrealProjects\\CCE\\NEAT\\NEATConsoleApplication\\pole2_markov.ne";
  NEAT::loadNeatParams(NeatParamsPath, true);

  char *GenomeParamsPath =
	  "E:\\UnrealProjects\\CCE\\NEAT\\NEATConsoleApplication\\pole2startgenes1";
  NEAT::Genome *startGenome = ReadGenome(GenomeParamsPath);

  Cart = new CartPole(1);
  Cart->N_MARKOV_LONG = false;
  Cart->GENERALIZATION_TEST = false;

  SpawnInitialPopulation(startGenome);
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

void ASpawnVolume::SpawnInitialPopulation(NEAT::Genome* startGenome) {

	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString(TEXT("Spawning Population off Genome")));

	Population = new NEAT::Population(startGenome, NEAT::popSize);
	
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("Verifying Spawned Pop")));
	Population->verifyPopulation();

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
	for (curOrg = (Population->organisms).begin(); curOrg != (Population->organisms).end(); ++curOrg) {
		// shouldn't happen
		if (((*curOrg)->gnome) == 0) {
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("ERROR EMPTY GENOME!")));
			return;
		}
		if (Pole2Evaluate((*curOrg))) {
			Win = true;
		}
	}

	// Get ready for real-time loop
	// Rank all the organisms from best to worst in each species
	Population->rankWithinSpecies();

	// Assign each species an average fitness
	// This average must be kept up-to-date by rtNEAT in order to select species
	// probabailistically for reproduction
	Population->estimateAllAverages();
}

void ASpawnVolume::NeatTick(int count) {

  if (Win)
    return;
  if (CompatAdjustFrequency == 0)
    return;

  // Every popSize reproductions, adjust the compatThresh to better match the
  // numSpeciesTarger  and reassign the population to new species
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
  vector<NEAT::Species *>::iterator curSpec;
  for (curSpec = (Population->species).begin();
       curSpec != (Population->species).end(); curSpec++) {
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("Species "))+FString::FromInt((*curSpec)->id));
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("size ")) + FString::FromInt((*curSpec)->organisms.size()));
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("average= ")) + FString::FromInt((*curSpec)->avgEst));
  }
  GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("Pop size: ")) + FString::FromInt(Population->organisms.size()));

  // Here we call two rtNEAT calls:
  // 1) chooseParentSpecies() decides which species should produce the next
  // offspring  2) reproduceOne(...) creates a single offspring from the chosen
  // species
  NEAT::Organism *newOrg =
      (Population->chooseParentSpecies())
          ->reproduceOne(count, Population, Population->species);

  // Now we evaluate the new individual
  // Note that in a true real-time simulation, evaluation would be happening to
  // all individuals at all times.  That is, this call would not appear here in
  // a true online simulation.
  GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("Evaluating new baby: ")));
  if (Pole2Evaluate(newOrg))
    Win = true;

  if (Win) {
    GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, "WINNER");
    Population->printToFileBySpecies((char *)"rt_winpop");
    return;
  }

  // Now we reestimate the baby's species' fitness
  newOrg->species->estimateAverage();

  // Remove the worst organism
  Population->removeWorst();
}

bool ASpawnVolume::Pole2Evaluate(NEAT::Organism *org) {
  NEAT::Network *net;

  int pause;

  net = org->net;

  // Try to balance a pole now
  org->fitness = Cart->evalNet(net);

#ifndef NO_SCREEN_OUT
  if (org->popChampChild)
    cout << " <<DUPLICATE OF CHAMPION>> ";

  // Output to screen
  GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString(TEXT("Org  ")+FString::FromInt((org->gnome)->genomeId)));
  GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString(TEXT(" fitness: ") + FString::FromInt(org->fitness)));
  GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString(TEXT(" (") + FString::FromInt((org->gnome)->genes.size()))+ FString(TEXT(" / ") + FString::FromInt((org->gnome)->nodes.size())) + FString(TEXT(" ) ")));
 
  if (org->mutStructBaby)
	  GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString(TEXT(" [struct]")));
  if (org->mateBaby)
	  GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString(TEXT(" [mate]")));
#endif

  if ((!(Cart->GENERALIZATION_TEST)) && (!(Cart->N_MARKOV_LONG)))
    if (org->popChampChild) {
      cout << org->gnome << endl;
      // DEBUG CHECK
      if (org->highFit > org->fitness) {
        cout << "ALERT: ORGANISM DAMAGED" << endl;
        printGenomeToFile(org->gnome, "failure_champ_genome");
        cin >> pause;
      }
    }

  // Decide if its a winner, in Markov Case
  if (Cart->MARKOV) {
    if (org->fitness >= (Cart->MAX_FITNESS - 1)) {
      org->winner = true;
      return true;
    } else {
      org->winner = false;
      return false;
    }
  }
  // if doing the long test non-markov
  else if (Cart->N_MARKOV_LONG) {
    if (org->fitness >= 99999) {
      org->winner = true;
      return true;
    } else {
      org->winner = false;
      return false;
    }
  } else if (Cart->GENERALIZATION_TEST) {
    if (org->fitness >= 999) {
      org->winner = true;
      return true;
    } else {
      org->winner = false;
      return false;
    }
  } else {
    org->winner = false;
    return false; // Winners not decided here in non-Markov
  }
}

void ASpawnVolume::Destroyed() {
  Super::Destroyed();
  if (Population) {
    delete Population;
  }
}
