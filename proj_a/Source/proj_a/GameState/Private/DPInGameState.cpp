#include "DPInGameState.h"

ADPInGameState::ADPInGameState()
{
	ScoreManagerComponent = CreateDefaultSubobject<UScoreManagerComponent>(TEXT("ScoreManagerComponent"));
}
