#include "JudgeGameMode.h"

#include "DPCharacter.h"
#include "JudgeGameState.h"
#include "Net/UnrealNetwork.h"

AJudgeGameMode::AJudgeGameMode()
    : ExpectedPlayerCount(2)
    , ConnectedPlayerCount(0)
    , bGameInProgress(false)
{
    // DefaultPawnClass = ADPCharacter::StaticClass();
    // PlayerControllerClass = JudgePlayerController::StaticClass();
    // PlayerStateClass = ADPPlayerState::StaticClass();
    GameStateClass = AJudgeGameState::StaticClass();
    
    TimerManager = CreateDefaultSubobject<UServerTimerManager>(TEXT("TimerManager"));
}

void AJudgeGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ConnectedPlayerCount++;
    CheckAllPlayersConnected();
}

void AJudgeGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    ConnectedPlayerCount--;
}

void AJudgeGameMode::StartPlay()
{
    Super::StartPlay();

    TimerManager->StartTimer(60.0f);
}

void AJudgeGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (TimerManager->IsTimeOver())
    {
        // 게임 종료
    }
}

void AJudgeGameMode::SetExpectedPlayerCount(int32 Count)
{
    ExpectedPlayerCount = Count;
    CheckAllPlayersConnected();
}

bool AJudgeGameMode::AreAllPlayersConnected() const
{
    return ConnectedPlayerCount == ExpectedPlayerCount;
}

void AJudgeGameMode::CheckAllPlayersConnected()
{
    if (AreAllPlayersConnected() && !bGameInProgress)
    {
        OnAllPlayersConnected();
    }
}

void AJudgeGameMode::OnAllPlayersConnected()
{
    bGameInProgress = true;
}

void AJudgeGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AJudgeGameMode, bGameInProgress);
}