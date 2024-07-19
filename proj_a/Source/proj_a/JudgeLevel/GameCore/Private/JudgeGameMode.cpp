#include "JudgeGameMode.h"

#include "DPCharacter.h"
#include "DPInGameState.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "JudgeGameState.h"
#include "JudgeLevelComponent.h"
#include "JudgePlayerController.h"
#include "Camera/CameraActor.h"

AJudgeGameMode::AJudgeGameMode()
{
    PlayerStateClass = ADPPlayerState::StaticClass();
    PlayerControllerClass = AJudgePlayerController::StaticClass();
    GameStateClass = AJudgeGameState::StaticClass();
    
    TimerManager = CreateDefaultSubobject<UServerTimerManager>(TEXT("TimerManager"));
    ChatManager = CreateDefaultSubobject<UServerChatManager>(TEXT("ChatManager"));
    
    bUseSeamlessTravel = true;
}

void AJudgeGameMode::EndTimer()
{
    FNetLogger::EditerLog(FColor::Emerald, TEXT("EndTimer"));
    // GetWorld()->ServerTravel("calculateLevel?listen");
}

void AJudgeGameMode::StartPlay()
{
    Super::StartPlay();
    
    TimerManager->StartTimer<AJudgeGameState>(60.0f, &AJudgeGameMode::EndTimer, this);
}

void AJudgeGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

    UWorld* W = GetWorld();
    check(W)

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    const FVector Loc = FVector(0.069079f, 1673.736449f, 1410.153101f);
    const FRotator Rot = FRotator(-30.0f, -90.0f, 0.0f);
    ACameraActor* CamAct =
        W->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), Loc, Rot, SpawnParams);
    check(CamAct)

    AJudgePlayerController* PC = Cast<AJudgePlayerController>(NewPlayer);
    check(PC)
    PC->SetViewTarget(CamAct);
    
    // ADPPlayerState* PS = Cast<ADPPlayerState>(PC->PlayerState);
    // FString PlayerName = PS->GetPlayerName();
    // FNetLogger::EditerLog(FColor::Emerald, TEXT("Player Name: %s"), *PlayerName);
    // UPrivateScoreManager* PSM = PC->GetPrivateScoreManagerComponent();
    // FNetLogger::EditerLog(FColor::Emerald, TEXT("Player Score: %d"), PSM->GetPrivatePlayerScore());
}
