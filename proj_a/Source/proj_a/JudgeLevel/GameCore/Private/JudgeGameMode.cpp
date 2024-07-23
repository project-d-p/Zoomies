#include "JudgeGameMode.h"

#include "DPCharacter.h"
#include "DPInGameState.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "JudgeGameState.h"
#include "JudgeLevelComponent.h"
#include "JudgePlayerController.h"
#include "JudgePlayerState.h"
#include "Algo/MaxElement.h"
#include "Camera/CameraActor.h"

AJudgeGameMode::AJudgeGameMode()
{
    PlayerStateClass = AJudgePlayerState::StaticClass();
    PlayerControllerClass = AJudgePlayerController::StaticClass();
    GameStateClass = AJudgeGameState::StaticClass();
    
    TimerManager = CreateDefaultSubobject<UServerTimerManager>(TEXT("TimerManager"));
    ChatManager = CreateDefaultSubobject<UServerChatManager>(TEXT("ChatManager"));
    
    bUseSeamlessTravel = true;
}

EOccupation AJudgeGameMode::CollateVotingResults()
{
    TMap<EOccupation, int32> VoteCounts;

    FNetLogger::EditerLog(FColor::Emerald, TEXT("PlayerVotes.Num(): %d"), PlayerVotes.Num());
    for (const EOccupation& Vote : PlayerVotes)
    {
        VoteCounts.FindOrAdd(Vote)++;
    }
    PlayerVotes.Empty();
    auto MostVotedPair = Algo::MaxElementBy(VoteCounts, [](const auto& Pair) { return Pair.Value; });

    // For now, if vote same, choose first one.
    return MostVotedPair->Key;
}

void AJudgeGameMode::ProcessVotingResults()
{
    EOccupation MostVotedOccupation = CollateVotingResults();
    FNetLogger::EditerLog(FColor::Emerald, TEXT("MostVotedOccupation: %d"), static_cast<int>(MostVotedOccupation));
    TimerManager->StartTimer<AJudgeGameState>(10.0f, &AJudgeGameMode::EndTimer, this);
}

void AJudgeGameMode::EndTimer()
{
    FNetLogger::EditerLog(FColor::Emerald, TEXT("EndTimer"));

    constexpr int TOTAL_PLAYER = 2;
    if (CurrentPlayerIndex++ < TOTAL_PLAYER)
    {
        AJudgePlayerController* PC = Cast<AJudgePlayerController>(GetWorld()->GetFirstPlayerController());
        check(PC)
        PC->NotifyTimerEnd();
        SetVote(EOccupation::ARCHAEOLOGIST);

        FTimerHandle VoteCollationTimerHandle;
        GetWorldTimerManager().SetTimer(VoteCollationTimerHandle, this, &AJudgeGameMode::ProcessVotingResults, 1.0f, false);
    }
    else
    {
        FNetLogger::EditerLog(FColor::Emerald, TEXT("EndTimer: ServerTravel"));
        // GetWorld()->ServerTravel("calculateLevel?listen");   
    }
}

void AJudgeGameMode::StartPlay()
{
    Super::StartPlay();
    
    TimerManager->StartTimer<AJudgeGameState>(10.0f, &AJudgeGameMode::EndTimer, this);
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
}
