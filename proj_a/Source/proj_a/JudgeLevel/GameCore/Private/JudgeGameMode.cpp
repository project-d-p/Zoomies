#include "JudgeGameMode.h"

#include "CompileMode.h"
#include "EngineUtils.h"
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
    // DefaultPawnClass = ADynamicTexturedCharacter::StaticClass();
    GameStateClass = AJudgeGameState::StaticClass();
    JudgedInformation = NewObject<UJudgeData>(this, TEXT("JudgedInformation"));
    JudgedInformation->InitializeData();
    
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
    while (true)
    {
        FString PName = Cast<AJudgePlayerState>(GetWorld()->GetGameState<AJudgeGameState>()->PlayerArray[CurrentPlayerIndex])->GetPlayerName();
        if (!JudgedInformation->IsJudgedPlayer(PName))
        {
            UIData.VoterName = Cast<AJudgePlayerState>(GetWorld()->GetGameState<AJudgeGameState>()->PlayerArray[CurrentPlayerIndex])->GetPlayerName();
            AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
            check(GS)
            GS->CurrentVotedPlayerName = PName;
            break ;
        }
        CurrentPlayerIndex++;
    }
    UIData.bInitSuccessful = true;
    return UIData;
}

EPlayerJob AJudgeGameMode::CollectVotingResults()
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
    EPlayerJob MostVotedOccupation = CollectVotingResults();

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

    if (CurrentPlayerIndex >= TOTAL_PLAYER)
    {
        EndTimer();
        return;
    }

    if (!GS->PlayerArray.IsValidIndex(CurrentPlayerIndex))
    {
        FNetLogger::LogError(TEXT("Failed to get %d player state."), CurrentPlayerIndex);
        return ;
    }

    while (true)
    {
        PS = Cast<AJudgePlayerState>(GetWorld()->GetGameState<AJudgeGameState>()->PlayerArray[CurrentPlayerIndex]);
        FString PName = PS->GetPlayerName();
        if (!JudgedInformation->IsJudgedPlayer(PName))
        {
            GS->SetVoterName(PName);
            break;
        }
        CurrentPlayerIndex++;
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
    UGI_Zoomies* GI = Cast<UGI_Zoomies>(GetGameInstance());
    check(GI)
    
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
        GI->player_count = GetWorld()->GetNumControllers();
        GetWorld()->ServerTravel("calculateLevel?listen");   
    }
}

void AJudgeGameMode::InitializeGameMode()
{
    UGI_Zoomies* GI = Cast<UGI_Zoomies>(GetGameInstance());
    check(GI)

    if (GI->network_failure_manager_->GetDesiredMaxPlayers() != 0)
    {
        TOTAL_PLAYER = GI->network_failure_manager_->GetDesiredMaxPlayers();
        UDataManager* DataManager = GI->network_failure_manager_->GetDataManager();
        check(DataManager)
        UJudgeData* SavedData = DataManager->GetSingleDataAs<UJudgeData>("JudgeData");
        if (SavedData)
        {
            JudgedInformation->SetJudgedPlayerNames(SavedData->GetJudgedPlayerNames());
        }
    }
    else
    {
        TOTAL_PLAYER = GI->player_count;
    }
}

void AJudgeGameMode::StartPlay()
{
    Super::StartPlay();

    InitializeGameMode();
    TimerManager->StartTimer<AJudgeGameState>(WAIT_TIME, &AJudgeGameMode::EndTimer, this);
}

void AJudgeGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

    // UWorld* W = GetWorld();
    // check(W)
    //
    // FActorSpawnParameters SpawnParams;
    // SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    // const FVector Loc = FVector(0.069079f, 1673.736449f, 1410.153101f);
    // const FRotator Rot = FRotator(-30.0f, -90.0f, 0.0f);
    // ACameraActor* CamAct =
    //     W->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), Loc, Rot, SpawnParams);
    // check(CamAct)
    //
    // NewPlayer->SetViewTarget(CamAct);
}
