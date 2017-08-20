#include "CCE.h"
#include "CCEGameMode.h"
#include "CCEGameState.h"
#include "Agent.h"

ACCEGameMode::ACCEGameMode()
{
//	DefaultPawnClass = AAgent::StaticClass();
	GameStateClass = ACCEGameState::StaticClass();
}
