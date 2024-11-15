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
    DefaultPawnClass = ADynamicTexturedCharacter::StaticClass();
    GameStateClass = AJudgeGameState::StaticClass();
    JudgedInformation = NewObject<UJudgeData>(this, TEXT("JudgedInformation"));
    JudgedInformation->InitializeData();
    
    TimerManager = CreateDefaultSubobject<UServerTimerManager>(TEXT("TimerManager"));
    ChatManager = CreateDefaultSubobject<UServerChatManager>(TEXT("ChatManager"));
    bUseSeamlessTravel = true;
}

void AJudgeGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    AJudgePlayerController* PC = Cast<AJudgePlayerController>(NewPlayer);
    check(PC)
    PC->RequestUIData();
}

FUIInitData AJudgeGameMode::GetUiData()
{
    FUIInitData UIData;
    int32 i = 0;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AJudgePlayerController* PC = Cast<AJudgePlayerController>(*It);
        if (!PC)
            return UIData;

        AJudgePlayerState* PS = Cast<AJudgePlayerState>(PC->PlayerState);
        if (!PS)
        {
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
            {
                GetUiData();
            }), 0.1f, false);
            return UIData;
        }

        AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
        if (!GS)
            return UIData;

        FPlayerInitData PlayerData;
        PlayerData.PlayerName = PS->GetPlayerName();
        PlayerData.Score = PS->GetScore();
        PlayerData.PlayerId = PS->GetPlayerId();

        bool bIsDuplicated = false;
        for (const FPlayerInitData& Data : GS->GS_PlayerData)
        {
            if (Data.PlayerId == PlayerData.PlayerId)
            {
                bIsDuplicated = true;
                break;
            }
        }

        if (!bIsDuplicated)
        {
            GS->GS_PlayerData.Add(PlayerData);
            UIData.PlayerData.Add(PlayerData);
        }
        i++;
    }

    AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
    if (!GS)
        return UIData;

    GS->GS_PlayerData.Sort([](const FPlayerInitData& A, const FPlayerInitData& B) {
        return A.PlayerId < B.PlayerId;
    });

    for (int32 Index = 0; Index < GS->GS_PlayerData.Num(); ++Index)
    {
        GS->GS_PlayerData[Index].CameraIndex = Index;
    }

    UIData.VoterName = Cast<AJudgePlayerState>(GetWorld()->GetGameState<AJudgeGameState>()->PlayerArray[CurrentPlayerIndex])->GetPlayerName();
    // for now, following code is temporary. (Current player index)
    while (true)
    {
        FString PName = Cast<AJudgePlayerState>(GetWorld()->GetGameState<AJudgeGameState>()->PlayerArray[CurrentPlayerIndex])->GetPlayerName();
        if (!JudgedInformation->IsJudgedPlayer(PName))
        {
            UIData.VoterName = Cast<AJudgePlayerState>(GetWorld()->GetGameState<AJudgeGameState>()->PlayerArray[CurrentPlayerIndex])->GetPlayerName();
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
    if (PlayerVotes.IsEmpty())
    {
        return EPlayerJob::JOB_NONE;
    }

    TMap<EPlayerJob, int32> VoteCounts;

    for (const EPlayerJob& Vote : PlayerVotes)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, OccupationToString(Vote));
        FNetLogger::EditerLog(FColor::Cyan, TEXT("%s"), *OccupationToString(Vote));
        VoteCounts.FindOrAdd(Vote)++;
    }
    PlayerVotes.Empty();
    auto MostVotedPair = Algo::MaxElementBy(VoteCounts, [](const auto& Pair) { return Pair.Value; });

    // For now, if the votes are the same, choose the first one. may be the server's vote is first.
    return MostVotedPair->Key;
}

AJudgePlayerState* AJudgeGameMode::GetCurrentVotedPlayerState()
{
    AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
    check(GS)
    for (APlayerState* PS : GS->PlayerArray)
    {
        AJudgePlayerState* JPS = Cast<AJudgePlayerState>(PS);
        if (JPS->GetPlayerName().Contains(GS->CurrentVotedPlayerName))
        {
            return JPS;
        }
    }
    return nullptr;
}

void AJudgeGameMode::HandlePlayerStateNull()
{
    AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
    if (!GS->PlayerArray.IsValidIndex(CurrentPlayerIndex - 1))
    {
        if (CurrentPlayerIndex < TOTAL_PLAYER)
        {
            TOTAL_PLAYER--;
        }
        FTimerHandle VoteCollectionTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(VoteCollectionTimerHandle, this, &AJudgeGameMode::EndTimer, 1.0f, false);
    }
    else
    {
        CurrentPlayerIndex--;
        TOTAL_PLAYER--;
        FTimerHandle VoteCollectionTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(VoteCollectionTimerHandle, this, &AJudgeGameMode::EndTimer, 1.0f, false);
    }
}

void AJudgeGameMode::ProcessVotingResults()
{
    EPlayerJob MostVotedOccupation = CollectVotingResults();
    GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::White, OccupationToString(MostVotedOccupation));

    AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
    check(GS)
    AJudgePlayerState* PS = GetCurrentVotedPlayerState();
    if (!PS)
    {
        HandlePlayerStateNull();
        return ;
    }

    if (PS->GetPlayerJob() == MostVotedOccupation)
    {
        PS->SetIsDetected(true);
    }

    if (CurrentPlayerIndex >= TOTAL_PLAYER)
    {
        FTimerHandle VoteCollectionTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(VoteCollectionTimerHandle, this, &AJudgeGameMode::EndTimer, 1.0f, false);
        return;
    }

    if (!GS->PlayerArray.IsValidIndex(CurrentPlayerIndex))
    {
        FTimerHandle VoteCollectionTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(VoteCollectionTimerHandle, this, &AJudgeGameMode::EndTimer, 1.0f, false);
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
    if (CurrentPlayerIndex < TOTAL_PLAYER && GS->PlayerArray.IsValidIndex(CurrentPlayerIndex))
    {
        TimerManager->StartTimer<AJudgeGameState>(WAIT_TIME, &AJudgeGameMode::EndTimer, this);
    }
    else
    {
        FTimerHandle VoteCollectionTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(VoteCollectionTimerHandle, this, &AJudgeGameMode::EndTimer, 1.0f, false);
    }
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

        // FTimerHandle VoteCollationTimerHandle;
        // GetWorldTimerManager().SetTimer(VoteCollationTimerHandle, this, &AJudgeGameMode::ProcessVotingResults, 1.0f, false);
        ProcessVotingResults();
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

void AJudgeGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
    if (GameInstance)
    {
        GameInstance->AddBanPlayer(Exiting->PlayerState->GetUniqueId()->ToString());
    }
    JudgedInformation->AddJudgedPlayerName(Exiting->PlayerState->GetPlayerName());
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

void AJudgeGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
    Super::GetSeamlessTravelActorList(bToTransition, ActorList);

    UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
    check(GameInstance)
    GameInstance->network_failure_manager_->TryReset();
}
