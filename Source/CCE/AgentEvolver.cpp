#include "CCE.h"
#include "AgentEvolver.h"

AAgentEvolver::AAgentEvolver()
{
	PrimaryActorTick.bCanEverTick = true;

	WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
	RootComponent = WhereToSpawn;
}

void AAgentEvolver::BeginPlay()
{
	Super::BeginPlay();

	NeatInit();

}

void AAgentEvolver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	NeatTick();
}

void AAgentEvolver::NeatInit()
{
	NEAT::loadNeatParams(static_cast<char*>(TCHAR_TO_ANSI(*FileNeatParamsPath)), true);

	NEAT::Genome* StartGenome = ReadGenome(static_cast<char*>(TCHAR_TO_ANSI(*FileStartGenomePath)));

	Population = GeneratePopulation(StartGenome);

//	Population=ReadPopulation(static_cast<char*>(TCHAR_TO_ANSI(*FileLastPopulationPath)));
}

NEAT::Genome* AAgentEvolver::ReadGenome(char* FilePath) const
{
	UE_LOG(LogTemp, Warning, TEXT("Reading in the start genome"));
	char CurWord[20];
	int Id;
	std::ifstream InFile(FilePath, std::ios::in);
	InFile >> CurWord;
	InFile >> Id;

	UE_LOG(LogTemp, Warning, TEXT("Reading in Genome"));
	NEAT::Genome* startGenome = new NEAT::Genome(Id, InFile);
	InFile.close();
	return startGenome;
}

NEAT::Population* AAgentEvolver::ReadPopulation(char* FilePath)
{
	NEAT::Population* ReadPopulation = new NEAT::Population(FilePath);

	ReadPopulation->verifyPopulation();

	// Initially, we evaluate the whole population
	// Evaluate each organism on a test
	int Count = 0;
	for (std::vector<NEAT::Organism *>::iterator CurOrg = (ReadPopulation->organisms).begin(); CurOrg != (ReadPopulation->organisms).end(); ++CurOrg , ++Count)
	{
		// shouldn't happen
		if (((*CurOrg)->gnome) == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("ERROR EMPTY GENOME!")));
			return nullptr;
		}

		//Spawn Agent and Attach NN 
		AAgent* Agent = SpawnAgent(Count % 2);
		if (Agent)
		{
			AAgentController* AgentController = static_cast<AAgentController*>(Agent->GetController());
			if (AgentController)
			{
				AgentController = AttachOrganismToAgentController(AgentController, (*CurOrg));
				AgentControllers.push_back(AgentController);
				if (EvaluateAgentController(AgentController))
				{
					WinnnerFound = true;
				}
			}
		}
	}

	// Get ready for real-time loop
	// Rank all the organisms from best to worst in each species
	ReadPopulation->rankWithinSpecies();

	// Assign each species an average fitness
	// This average must be kept up-to-date by rtNEAT in order to select species
	// probabailistically for reproduction
	ReadPopulation->estimateAllAverages();

	return ReadPopulation;
}

NEAT::Population* AAgentEvolver::GeneratePopulation(NEAT::Genome* StartGenome)
{
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString(TEXT("Generating population")));

	NEAT::Population* SpawnedPopulation = new NEAT::Population(StartGenome, NEAT::popSize);
	SpawnedPopulation->verifyPopulation();

	// We try to keep the number of species constant at this number
	NumSpeciesTarget = NEAT::popSize / 15;

	// This is where we determine the frequency of compatibility threshold adjustment
	CompatAdjustFrequency = NEAT::popSize / 10;
	if (CompatAdjustFrequency < 1)
	{
		CompatAdjustFrequency = 1;
	}

	// Initially, we evaluate the whole population
	// Evaluate each organism on a test
	int Count = 0;
	for (std::vector<NEAT::Organism *>::iterator CurOrg = (SpawnedPopulation->organisms).begin(); CurOrg != (SpawnedPopulation->organisms).end(); ++CurOrg , ++Count)
	{
		// shouldn't happen
		if (((*CurOrg)->gnome) == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(TEXT("ERROR EMPTY GENOME!")));
			return nullptr;
		}

		AAgent* Agent = SpawnAgent(Count % 2);
		if (Agent)
		{
			AAgentController* AgentController = static_cast<AAgentController*>(Agent->GetController());
			if (AgentController)
			{
				AgentController = AttachOrganismToAgentController(AgentController, (*CurOrg));
				AgentControllers.push_back(AgentController);
				if (EvaluateAgentController(AgentController))
				{
					WinnnerFound = true;
				}
			}
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

AAgentController* AAgentEvolver::AttachOrganismToAgentController(AAgentController* AgentController, NEAT::Organism* Org)
{
	if (AgentController)
	{
		AgentController->SetNeatOrganism(Org);
		AgentController->GetAgent()->SetNumberOfKicks(0);
		AgentController->GetAgent()->SeNumberOfFalseKicks(0);
		AgentController->GetAgent()->SetScoredPoints(0);
		
//		for (TActorIterator<ABall> BallItr(GetWorld()); BallItr; ++BallItr)
//		{
//			FVector BallLocation = BallItr->GetActorLocation();
			FVector SpawnLocation= FVector(0, 0, 600);
			AgentController->GetAgent()->GetRootComponent()->SetWorldLocation(SpawnLocation);
//		}

//		if(WhereToSpawn){
//		
//			FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
//			FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;
//			FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
//			FRotator SpawnRotation(0, FMath::FRand() * 360.0f, 0);
//			AgentController->GetAgent()->GetRootComponent()->SetWorldLocation(SpawnLocation);
//		}

		Org->fitness = AgentController->EvaluateFitness();
		return AgentController;
	}
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString("AttachOrganismToAgentController null"));
	return nullptr;
}

void AAgentEvolver::NeatTick()
{
	if (TicksFromLastCalculate++ > CalculatingFrequencyInTicks)
	{
		TicksFromLastCalculate = 0;
		NeatTick(OffspringCount++);
	}
}

void AAgentEvolver::NeatTick(int Offsprings)
{
	if (WinnnerFound){
		return;
	}

	// Every popSize reproductions, adjust the compatThresh to better match the NumSpeciesTarget  and reassign the population to new species
	if (CompatAdjustFrequency && Offsprings % CompatAdjustFrequency == 0){
		int NumSpecies = static_cast<int>(Population->species.size());
		// Modify compat thresh to control speciation
		double CompatMod = 0.1;

		// This tinkers with the compatibility threshold						
		if (NumSpecies < NumSpeciesTarget)
		{
			NEAT::compatThreshold -= CompatMod;
		}
		else if (NumSpecies > NumSpeciesTarget)
		{
			NEAT::compatThreshold += CompatMod;
		}

		if (NEAT::compatThreshold < 0.3)
		{
			NEAT::compatThreshold = 0.3;
		}

		// Go through entire population, reassigning organisms to new species
		for (std::vector<NEAT::Organism *>::iterator CurOrg = (Population->organisms).begin(); CurOrg != Population->organisms.end(); ++CurOrg)
		{
			Population->reassignSpecies(*CurOrg);
		}
	}

	//For printing only
	for (std::vector<NEAT::Species *>::iterator curSpec = (Population->species).begin(); curSpec != (Population->species).end(); ++curSpec)
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString("Species: ") + FString::FromInt((*curSpec)->id)
		                                 + FString(" Size: ") + FString::FromInt((*curSpec)->organisms.size())
		                                 + FString(" Average fitness: ") + FString::SanitizeFloat((*curSpec)->avgEst));
	}
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString("Population size: ") + FString::FromInt(Population->organisms.size()));

	NEAT::Organism* WorstOrganism = Population->removeWorst();
	AAgentController* AgentController = FindAgentControllerByNeatOrganism(WorstOrganism);
	if (AgentController)
	{
		NEAT::Organism* NewOrg = (Population->chooseParentSpecies())->reproduceOne(Offsprings, Population, Population->species);
		AgentController = AttachOrganismToAgentController(AgentController, NewOrg);
		// Now we reestimate the baby's species' fitness
		NewOrg->species->estimateAverage();
		if (EvaluateAgentController(AgentController))
		{
			WinnnerFound = true;
		}
	}

	// Now we evaluate the new individual
	// Note that in a true real-time simulation, evaluation would be happening to
	// all individuals at all times.  That is, this call would not appear here in
	// a true online simulation.
	if (WinnnerFound)
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, "Winner found");
		Population->printToFileBySpecies(static_cast<char*>(TCHAR_TO_ANSI(*FileWinnerPopulationPath)));
		//TEMPORARY: Completely meaningless stuff
		//UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
		return;
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Green, FString("Printing last population to file"));
		Population->printToFileBySpecies(static_cast<char*>(TCHAR_TO_ANSI(*FileLastPopulationPath)));
	}
}

AAgentController* AAgentEvolver::FindAgentControllerByNeatOrganism(NEAT::Organism* Org)
{
	for (std::vector<AAgentController *>::iterator ÑurAgentConntoller = AgentControllers.begin(); ÑurAgentConntoller != AgentControllers.end(); ++ÑurAgentConntoller)
	{
		if ((*ÑurAgentConntoller)->GetNeatOrganism() == Org)
		{
			return (*ÑurAgentConntoller);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString("FindAgentControllerByNeatOrganism in null"));
	return nullptr;
}

bool AAgentEvolver::EvaluateAgentController(AAgentController* AgentController)
{
	if (AgentController)
	{
		NEAT::Organism* Org = AgentController->GetNeatOrganism();
		Org->fitness = AgentController->EvaluateFitness();
		double DesirableValue = INT_MAX;

		//Output to screen
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString("Org: ") + FString::FromInt((Org->gnome)->genomeId) + FString(" fitness: ") + FString::FromInt(Org->fitness) + FString(" species id : ") + FString::FromInt(Org->species->id));
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString("( ") + FString::FromInt((Org->gnome)->genes.size()) + FString(" / ") + FString::FromInt((Org->gnome)->nodes.size()) + FString(" ) ") );
		if (Org->mutStructBaby) GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(" [struct]"));
		if (Org->mateBaby) GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString(" [mate]"));

		if (Org->fitness >= DesirableValue)
		{
			Org->winner = true;
			return true;
		}
		else
		{
			Org->winner = false;
			return false;
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString("AgentController in null"));
	return false;
}

AAgent* AAgentEvolver::SpawnAgent(int8 Team)
{
	if (WhatToSpawn)
	{
		UWorld*const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			//SpawnParams.bNoFail = true;

			FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
			FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;
			FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);

			FRotator SpawnRotation(0, FMath::FRand() * 360.0f, 0);

			AAgent*const SpawnedActor = World->SpawnActor<AAgent>(WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
			if (SpawnedActor)
			{
				SpawnedActor->SetTeam(Team);
				GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Magenta, FString("Spawning Agent - Team: ") + FString::FromInt(SpawnedActor->GetTeam()));
				return SpawnedActor;
			}
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString("SpawnAgent is null"));
	return nullptr;
}
