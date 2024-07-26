#include "JudgeGameMode.h"

#include "FNetLogger.h"
#include "JudgeGameState.h"
#include "JudgePlayerController.h"
#include "JudgePlayerState.h"
#include "Algo/MaxElement.h"
#include "Camera/CameraActor.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

AJudgeGameMode::AJudgeGameMode()
{
    PlayerStateClass = AJudgePlayerState::StaticClass();
    PlayerControllerClass = AJudgePlayerController::StaticClass();
    GameStateClass = AJudgeGameState::StaticClass();
    
    TimerManager = CreateDefaultSubobject<UServerTimerManager>(TEXT("TimerManager"));
    ChatManager = CreateDefaultSubobject<UServerChatManager>(TEXT("ChatManager"));
    
    bUseSeamlessTravel = true;
}

EPlayerJob AJudgeGameMode::CollateVotingResults()
{
    TMap<EPlayerJob, int32> VoteCounts;

    FNetLogger::EditerLog(FColor::Emerald, TEXT("PlayerVotes.Num(): %d"), PlayerVotes.Num());
    for (const EPlayerJob& Vote : PlayerVotes)
    {
        VoteCounts.FindOrAdd(Vote)++;
    }
    PlayerVotes.Empty();
    auto MostVotedPair = Algo::MaxElementBy(VoteCounts, [](const auto& Pair) { return Pair.Value; });

    // For now, if the votes are the same, choose the first one. may be the server's vote is first.
    return MostVotedPair->Key;
}

void AJudgeGameMode::ProcessVotingResults()
{
    EPlayerJob MostVotedOccupation = CollateVotingResults();

    AJudgePlayerState* PS = Cast<AJudgePlayerState>(GetWorld()->GetGameState<AJudgeGameState>()->PlayerArray[CurrentPlayerIndex - 1]);
    check(PS)
    if (PS->GetPlayerJob() == MostVotedOccupation)
    {
        PS->SetIsDetected(true);
    }
    
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AJudgePlayerController* PC = Cast<AJudgePlayerController>(*It);
        check(PC)
        PC->SetOccupationeName(CurrentPlayerIndex - 1, OccupationToString(MostVotedOccupation));
    }
    TimerManager->StartTimer<AJudgeGameState>(WAIT_TIME, &AJudgeGameMode::EndTimer, this);
}

void AJudgeGameMode::EndTimer()
{
    constexpr int TOTAL_PLAYER = 2;
    if (++CurrentPlayerIndex < TOTAL_PLAYER)
    {
        AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
        check(GS)
        GS->NotifyTimerEnd();

        FTimerHandle VoteCollationTimerHandle;
        GetWorldTimerManager().SetTimer(VoteCollationTimerHandle, this, &AJudgeGameMode::ProcessVotingResults, 1.0f, false);
    }
    else
    {
        UGI_Zoomies* GI = Cast<UGI_Zoomies>(GetGameInstance());
        check(GI)
        GI->player_count = GetWorld()->GetNumControllers();
        GetWorld()->ServerTravel("calculateLevel?listen");   
    }
}

void AJudgeGameMode::StartPlay()
{
    Super::StartPlay();
    
    TimerManager->StartTimer<AJudgeGameState>(WAIT_TIME, &AJudgeGameMode::EndTimer, this);
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
