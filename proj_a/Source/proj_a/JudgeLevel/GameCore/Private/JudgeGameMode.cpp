#include "JudgeGameMode.h"

#include "CompileMode.h"
#include "EngineUtils.h"
#include "FNetLogger.h"
#include "JudgeGameState.h"
#include "JudgeLevelComponent.h"
#include "JudgePlayerController.h"
#include "JudgePlayerState.h"
#include "ServerChatManager.h"
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

void AJudgeGameMode::AddVote(const FString& VotedName, EPlayerJob Occupation)
{
    if (!PlayerVotes.Contains(VotedName))
    {
        PlayerVotes.Add(VotedName, TArray<EPlayerJob>());
    }
    PlayerVotes[VotedName].Add(Occupation);
}

FUIInitData AJudgeGameMode::GetUiData()
{
    FUIInitData UIData;
    int32 i = 0;
    AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
    if (!GS) return UIData;

    //InitializePlayerData
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        AJudgePlayerController* PC = Cast<AJudgePlayerController>(*It);
        if (!PC) continue;

        AJudgePlayerState* PS = Cast<AJudgePlayerState>(PC->PlayerState);
        if (!PS)
        {
            //SetDelayedUiDataFetch
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
            {
                GetUiData();
            }), 0.1f, false);
            return UIData;
        }
        FPlayerInitData PlayerData;
        PlayerData.PlayerName = PS->GetPlayerName();
        PlayerData.Score = PS->GetScore();
        PlayerData.PlayerId = PS->GetPlayerId();

        //Check Duplicates
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
    }

    //SetCameraIndex
    GS->GS_PlayerData.Sort([](const FPlayerInitData& A, const FPlayerInitData& B) {
        return A.PlayerId < B.PlayerId;
    });

    for (int32 Index = 0; Index < GS->GS_PlayerData.Num(); ++Index)
    {
        GS->GS_PlayerData[Index].CameraIndex = Index;
    }

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

void AJudgeGameMode::CollectVotingResults(const FString& CurrentVotedPlayer)
{
    TArray<EPlayerJob> CollectedPlayerVotes;
    if (PlayerVotes.Contains(CurrentVotedPlayer))
    {
        CollectedPlayerVotes = PlayerVotes[CurrentVotedPlayer];
    }
    if (CollectedPlayerVotes.IsEmpty() || CollectedPlayerVotes.Num() < TOTAL_PLAYER)
    {
        FTimerHandle VoteCollectionTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(VoteCollectionTimerHandle, FTimerDelegate::CreateLambda([this, CurrentVotedPlayer]()
        {
            CollectVotingResults(CurrentVotedPlayer);
        }), 1.0f, false);
        return ;
    }

    TMap<EPlayerJob, int32> VoteCounts;

    for (const EPlayerJob& Vote : CollectedPlayerVotes)
    {
        // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, OccupationToString(Vote));
        FNetLogger::EditerLog(FColor::Cyan, TEXT("VotedPlayerName[%s]: %s"), *CurrentVotedPlayer, *OccupationToString(Vote));
        FNetLogger::LogError(TEXT("VotedPlayerName[%s] : %s"), *CurrentVotedPlayer, *OccupationToString(Vote));
        VoteCounts.FindOrAdd(Vote)++;
    }
    auto MostVotedPair = Algo::MaxElementBy(VoteCounts, [](const auto& Pair) { return Pair.Value; });

    AJudgePlayerState* PS = GetPlayerStateFromPlayerName(CurrentVotedPlayer);
    if (PS)
    {
        if (PS->GetPlayerJob() == MostVotedPair->Key)
        {
            PS->SetIsDetected(true);
        }
        else
        {
            PS->SetIsDetected(false);
        }
        JudgedInformation->AddJudgedPlayerName(CurrentVotedPlayer);
        AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
        check(GS)
        GS->NotifyCurrentPlayerVoted(CurrentVotedPlayer);
    }
    else
    {
        HandlePlayerStateNull();
    }
}

AJudgePlayerState* AJudgeGameMode::GetPlayerStateFromPlayerName(const FString& PlayerName)
{
    AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
    check(GS)
    for (APlayerState* PS : GS->PlayerArray)
    {
        AJudgePlayerState* JPS = Cast<AJudgePlayerState>(PS);
        if (JPS->GetPlayerName().Contains(PlayerName))
        {
            return JPS;
        }
    }
    return nullptr;
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
    TOTAL_PLAYER--;
    if (!GS->PlayerArray.IsValidIndex(CurrentPlayerIndex - 1))
    {
        // 중간에 나간 클라이언트가 마지막 순번이었을 경우
        FTimerHandle VoteCollectionTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(VoteCollectionTimerHandle, this, &AJudgeGameMode::EndTimer, 1.0f, false);
    }
    else
    {
        CurrentPlayerIndex--;
        FTimerHandle VoteCollectionTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(VoteCollectionTimerHandle, this, &AJudgeGameMode::EndTimer, 1.0f, false);
    }
}

void AJudgeGameMode::ProcessVotingResults()
{
    AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
    check(GS)
    CollectVotingResults(GS->CurrentVotedPlayerName);

    AJudgePlayerState* PS = GetCurrentVotedPlayerState();
    if (!PS)
    {
        HandlePlayerStateNull();
        return ;
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

bool AJudgeGameMode::IsJudgedAllPlayers()
{
    AJudgeGameState* GS = GetWorld()->GetGameState<AJudgeGameState>();
    check(GS)
    int JudgedPlayerCount = 0;
    for (auto PlayerSate : GS->PlayerArray)
    {
        FString PlayerName = PlayerSate->GetPlayerName();
        if (JudgedInformation->IsJudgedPlayer(PlayerName))
        {
            JudgedPlayerCount++;
        }
    }
    if (JudgedPlayerCount >= TOTAL_PLAYER)
    {
        return true;
    }
    return false;
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
        ProcessVotingResults();
    }
    else
    {
        if (IsJudgedAllPlayers())
        {
            GI->player_count = GetWorld()->GetNumControllers();
            GetWorld()->ServerTravel("calculateLevel?listen");   
        }
        else
        {
            FTimerHandle ReStartEndTimerHandle;
            GetWorld()->GetTimerManager().SetTimer(ReStartEndTimerHandle, this, &AJudgeGameMode::EndTimer, 1.0f, false);
        }
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
}

void AJudgeGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
    Super::GetSeamlessTravelActorList(bToTransition, ActorList);

    UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
    check(GameInstance)
    GameInstance->network_failure_manager_->TryReset();
}
