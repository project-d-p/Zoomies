#include "JudgeGameMode.h"

#include "CompileMode.h"
#include "FNetLogger.h"
#include "JudgeGameState.h"
#include "JudgeLevelComponent.h"
#include "JudgePlayerController.h"
#include "JudgePlayerState.h"
#include "Algo/MaxElement.h"
#include "Camera/CameraActor.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

AJudgeGameMode::AJudgeGameMode()
{
    PlayerStateClass = AJudgePlayerState::StaticClass();
    PlayerControllerClass = AJudgePlayerController::StaticClass();
    // PlayerControllerClass = ADPPlayerController::StaticClass();
    GameStateClass = AJudgeGameState::StaticClass();
    
    TimerManager = CreateDefaultSubobject<UServerTimerManager>(TEXT("TimerManager"));
    ChatManager = CreateDefaultSubobject<UServerChatManager>(TEXT("ChatManager"));
    
    bUseSeamlessTravel = true;
}

FUIInitData AJudgeGameMode::GetUiData()
{
    FUIInitData UIData;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AJudgePlayerController* PC = Cast<AJudgePlayerController>(*It);
        // ADPPlayerController* PC = Cast<ADPPlayerController>(*It);
        if (!PC)
            return UIData;
        AJudgePlayerState* PS = Cast<AJudgePlayerState>(PC->PlayerState);
        if (!PS)
            return UIData;
        FPlayerInitData PlayerData;
        PlayerData.PlayerName = PS->GetPlayerName();
        PlayerData.Score = PS->GetScore();
        UIData.PlayerData.Add(PlayerData);
    }
    // for now, following code is temporary. (Current player index)
    UIData.VoterName = Cast<AJudgePlayerState>(GetWorld()->GetGameState<AJudgeGameState>()->PlayerArray[CurrentPlayerIndex])->GetPlayerName();
    UIData.bInitSuccessful = true;
    return UIData;
}

EPlayerJob AJudgeGameMode::CollateVotingResults()
{
    TMap<EPlayerJob, int32> VoteCounts;

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

    AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
    check(GS)
    if (!GS->PlayerArray.IsValidIndex(CurrentPlayerIndex - 1))
    {
        FNetLogger::LogError(TEXT("Failed to get %d player state."), CurrentPlayerIndex - 1);
        return ;
    }
    AJudgePlayerState* PS = Cast<AJudgePlayerState>(GetWorld()->GetGameState<AJudgeGameState>()->PlayerArray[CurrentPlayerIndex - 1]);
    check(PS)
    if (PS->GetPlayerJob() == MostVotedOccupation)
    {
        PS->SetIsDetected(true);
    }

    constexpr int TOTAL_PLAYER = Zoomies::MAX_PLAYERS;
    if (CurrentPlayerIndex == TOTAL_PLAYER)
    {
        EndTimer();
        return;
    }

    if (!GS->PlayerArray.IsValidIndex(CurrentPlayerIndex))
    {
        FNetLogger::LogError(TEXT("Failed to get %d player state."), CurrentPlayerIndex);
        return ;
    }
    PS = Cast<AJudgePlayerState>(GetWorld()->GetGameState<AJudgeGameState>()->PlayerArray[CurrentPlayerIndex]);
    FString PName = PS->GetPlayerName();
    GS->SetVoterName(PName);

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AJudgePlayerController* PC = Cast<AJudgePlayerController>(*It);
        // ADPPlayerController* PC = Cast<ADPPlayerController>(*It);
        check(PC)
        // UJudgeLevelComponent* JLC = Cast<UJudgeLevelComponent>(PC->GetLevelComponent());
        // check(JLC)
        // JLC->SetOccupationeName(CurrentPlayerIndex - 1, OccupationToString(MostVotedOccupation));
        PC->SetOccupationeName(CurrentPlayerIndex - 1, OccupationToString(MostVotedOccupation));
    }
    TimerManager->StartTimer<AJudgeGameState>(WAIT_TIME, &AJudgeGameMode::EndTimer, this);
}

void AJudgeGameMode::EndTimer()
{
    constexpr int TOTAL_PLAYER = Zoomies::MAX_PLAYERS;
    if (CurrentPlayerIndex++ < TOTAL_PLAYER)
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

    NewPlayer->SetViewTarget(CamAct);
}
