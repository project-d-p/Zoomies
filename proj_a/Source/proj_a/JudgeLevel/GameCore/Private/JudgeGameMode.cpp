#include "JudgeGameMode.h"

#include "DPCharacter.h"
#include "DPInGameState.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "JudgeGameState.h"
#include "Net/UnrealNetwork.h"

AJudgeGameMode::AJudgeGameMode()
    : ExpectedPlayerCount(2)
    , ConnectedPlayerCount(0)
    , bGameInProgress(false)
{
    DefaultPawnClass = ADPCharacter::StaticClass();
    // PlayerStateClass = ADPPlayerState::StaticClass();
    PlayerControllerClass = ADPPlayerController::StaticClass();
    // GameStateClass = AJudgeGameState::StaticClass();
    GameStateClass = ADPInGameState::StaticClass();
    
    TimerManager = CreateDefaultSubobject<UServerTimerManager>(TEXT("TimerManager"));
    ChatManager = CreateDefaultSubobject<UServerChatManager>(TEXT("ChatManager"));

    bReplicates = true;
    bUseSeamlessTravel = true;
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

void AJudgeGameMode::EndTimer()
{
    FNetLogger::EditerLog(FColor::Emerald, TEXT("EndTimer"));
    // GetWorld()->ServerTravel("calculateLevel?listen");
}

void AJudgeGameMode::StartPlay()
{
    Super::StartPlay();

    TimerManager->StartTimer<ADPInGameState>(60.0f, &AJudgeGameMode::EndTimer, this);
}

void AJudgeGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void AJudgeGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
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