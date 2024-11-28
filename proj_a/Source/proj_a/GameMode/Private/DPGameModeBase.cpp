// Fill out your copyright notice in the Description page of Project Settings.

#include "DPGameModeBase.h"

#include "BaseMonsterAIController.h"
#include "DPCharacter.h"
#include "DPInGameState.h"
#include "DPPlayerController.h"
#include "DPPlayerState.h"
#include "FNetLogger.h"
#include "ELevelComponentType.h"
#include "ENetworkTypeZoomies.h"
#include "MainLevelComponent.h"
#include "MessageMaker.h"
#include "ServerNetworkManager.h"
#include "CompileMode.h"
#include "MonsterData.h"
#include "TimeData.h"
#include "GameFramework/PawnMovementComponent.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

class UTimeData;

ADPGameModeBase::ADPGameModeBase()
{
	bUseSeamlessTravel = true;
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ADPPlayerController::StaticClass();
	PlayerStateClass = ADPPlayerState::StaticClass();
	GameStateClass = ADPInGameState::StaticClass();

	TimerManager = CreateDefaultSubobject<UServerTimerManager>(TEXT("TimerManager"));
	ChatManager = CreateDefaultSubobject<UServerChatManager>(TEXT("ChatManager"));
	ScoreManager = CreateDefaultSubobject<UScoreManagerComp>(TEXT("ScoreManager"));
	MonsterFactory = CreateDefaultSubobject<UMonsterFactory>(TEXT("MonsterFactory"));

#if EDITOR_MODE
	NetworkManager = CreateDefaultSubobject<UANetworkManager>(TEXT("NetworkManager"));
#elif LAN_MODE
	NetworkManager = CreateDefaultSubobject<UServerNetworkManager>(TEXT("NetworkManager"));
#else
	NetworkManager = CreateDefaultSubobject<UServerNetworkManager>(TEXT("NetworkManager"));
#endif	
}

void ADPGameModeBase::OnGameStart()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	check(GameInstance);

	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]() {
			this->bStart = true;
		}), 10.0f, false);
	});
}

// Only Called in Server : PlayerController && PlayerState Automatically Travel
void ADPGameModeBase::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (!GameInstance)
		return ;
	GameInstance->network_failure_manager_->TryReset();
	for (FConstPlayerControllerIterator IT = GetWorld()->GetPlayerControllerIterator(); IT; ++IT)
	{
		ADPPlayerController* PlayerController = Cast<ADPPlayerController>(*IT);
		if (PlayerController)
		{
			PlayerController->SwitchLevelComponent(ELevelComponentType::NONE);
			PlayerController->ReleaseMemory();
		}
	}
	GameInstance->player_count = GetWorld()->GetNumControllers();
}

void ADPGameModeBase::SpawnNewCharacter(APlayerController* NewPlayer)
{
	FVector Location[4] = {
		FVector(13833.768291,-469.843764,82.428649),
		FVector(13833.768291,79.326964,82.428649),
		FVector(14166.601644,79.326964,82.428649),
		FVector(14166.601644,-469.843764,82.428649),
	};
	static int idx = 0;
	if (idx >= 4)
		idx = 0;
	FVector SpawnLocation = Location[idx++];

	ADPCharacter* NewCharacter = GetWorld()->SpawnActor<ADPCharacter>(ADPCharacter::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
	if (NewPlayer)
	{
		NewPlayer->Possess(NewCharacter);
	}
	// As we set the default pawn class to ADPCharacter, we can use the following code to relocate an existing character.
	// NewPlayer->GetCharacter()->SetActorLocation(SpawnLocation);
}

void ADPGameModeBase::UpdateMonsterData(ABaseMonsterCharacter* InMonster)
{
	auto [ClosestPlayer, MinDistance] = FindClosestPlayer(InMonster);
	if (ClosestPlayer)
	{
		float MoveInterval = CalculateMoveInterval(MinDistance);
		
		FMonsterOptimizationData MOD;
		MOD.ClosestPlayer = ClosestPlayer;
		MOD.Interval = MoveInterval;
		MOD.Dist = MinDistance;
		if (FMonsterOptimizationData* ExistingData = MonsterOptimizationData.Find(InMonster))
		{
			MOD.Timer = ExistingData->Timer;
		}
		MonsterOptimizationData.Add(InMonster, MOD);
	}
}

std::pair<ADPCharacter*, float> ADPGameModeBase::FindClosestPlayer(ABaseMonsterCharacter* InMonster)
{
	ADPCharacter* ClosestPlayer = nullptr;
	float MinDistance = TNumericLimits<float>::Max();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC)
		{
			FNetLogger::LogError(TEXT("Failed to get PlayerController. (Method: FindClosestPlayer)"));
			return std::make_pair(nullptr, MinDistance);
		}
		
		ADPCharacter* PlayerCharacter = Cast<ADPCharacter>(PC->GetPawn());
		if (!PlayerCharacter)
		{
			FNetLogger::LogError(TEXT("Failed to get PlayerCharacter. (Method: FindClosestPlayer)"));
			return std::make_pair(nullptr, MinDistance);
		}
		
		float DistanceToPlayer = FVector::Dist(InMonster->GetActorLocation(), PlayerCharacter->GetActorLocation());
		if (DistanceToPlayer < MinDistance)
		{
			MinDistance = DistanceToPlayer;
			ClosestPlayer = PlayerCharacter;
		}
	}
	return std::make_pair(ClosestPlayer, MinDistance);
}

float ADPGameModeBase::CalculateMoveInterval(float DistanceToPlayer)
{
	/* Distance Setting */
	constexpr float CLOSE_DISTANCE = 1500.f;
	constexpr float FAR_DISTANCE = 4000.f;
	/* interval Setting */
	constexpr float MIN_INTERVAL = 0.033f;
	constexpr float MAX_INTERVAL = 1.0f;

    if (DistanceToPlayer <= CLOSE_DISTANCE)
    {
        return MIN_INTERVAL;
    }
    if (DistanceToPlayer <= FAR_DISTANCE)
    {
        return FMath::Lerp(MIN_INTERVAL, MAX_INTERVAL, (DistanceToPlayer - CLOSE_DISTANCE) / (FAR_DISTANCE - CLOSE_DISTANCE));
    }
    return MAX_INTERVAL;
}

bool ADPGameModeBase:: ShouldProcessMonster(ABaseMonsterCharacter* InMonster, float& delta_time)
{
	float& MonsterTimer = MonsterOptimizationData.FindOrAdd(InMonster).Timer;
	MonsterTimer += delta_time;

	const float MonsterInterval = MonsterOptimizationData.Find(InMonster)->Interval;
	if (MonsterTimer < MonsterInterval)
	{
		return false;
	}

	float tmp = MonsterTimer;
	delta_time = tmp;
	MonsterTimer = 0.f;
	return true;
}

void ADPGameModeBase::RemoveMonsterData(ABaseMonsterCharacter* InMonster)
{
	if (InMonster)
	{
		MonsterOptimizationData.Remove(InMonster);
	}
}

void ADPGameModeBase::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);
	check(newPlayer);

	ADPPlayerState* player_state = Cast<ADPPlayerState>(newPlayer->PlayerState);
	check(player_state);

#if LAN_MODE || EDITOR_MODE
// #if EDITOR_MODE
	FString name = player_state->GetPlayerName();
	std::string key(TCHAR_TO_UTF8(*name));
	
	if (player_controllers_.find(key) != player_controllers_.end())
	{
		player_state->SetPlayerName(name + "1");
		key = std::string(TCHAR_TO_UTF8(*player_state->GetPlayerName()));
	}
#else
	// Online Mode : Steam ID
	std::string key(TCHAR_TO_UTF8(*player_state->GetUniqueId()->ToString()));
#endif
	
	player_controllers_[key] = Cast<ADPPlayerController>(newPlayer);
	if (!player_controllers_[key]->IsLocalController())
	{
		player_controllers_[key]->SwitchLevelComponent(ELevelComponentType::MAIN);
	}

	// Spawn Character in New Place
	SpawnNewCharacter(newPlayer);
	
	// Set Player Random Job
	if (bRestarted == false)
	{
		player_state->SetPlayerRandomJob();
	}
	
	if (!newPlayer->IsLocalController())
	{
		player_controllers_[key]->ConnectToServer(ELevelComponentType::MAIN);
	}

	ADPInGameState* GS = GetGameState<ADPInGameState>();
	check(GS)
	if (GS)
	{
		GS->AddConnectedPlayer();
		CheckAllPlayersConnected();
	}
	else
	{
		FNetLogger::LogError(TEXT("Fail to cast ADPInGameState."));
	}
}

void ADPGameModeBase::CheckAllPlayersConnected()
{
	ADPInGameState* GS = GetGameState<ADPInGameState>();
	check(GS)
	if (GS && GS->AreAllPlayersConnected())
	{
		// StartGame();
	}
}

void ADPGameModeBase::StartGame()
{
	ADPInGameState* GS = GetGameState<ADPInGameState>();
	if (GS)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([GS]() {
			GS->MulticastPlayerJob();
		}), 0.5f, false); 
	}
}

void ADPGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (!Exiting)
	{
		return ;
	}
	ADPPlayerController* controller = Cast<ADPPlayerController>(Exiting);
	if (!controller)
	{
		return ;
	}
#if LAN_MODE || EDITOR_MODE
	std::string key(TCHAR_TO_UTF8(*controller->PlayerState->GetPlayerName()));
#else
	std::string key(TCHAR_TO_UTF8(*controller->PlayerState->GetUniqueId()->ToString()));
#endif
	if (player_controllers_.contains(key))
	{
		player_controllers_.erase(key);
	}
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->AddBanPlayer(controller->PlayerState->GetUniqueId()->ToString());
	}
}

void ADPGameModeBase::InitializeGame()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	check(GameInstance);
	int NumMaxPlayers = GameInstance->network_failure_manager_->GetDesiredMaxPlayers();
	if (NumMaxPlayers == 0)
	{
		BlockingVolume = GetWorld()->SpawnActor<ABlockingBoxVolume>(ABlockingBoxVolume::StaticClass(), FVector(13380.0f, -253.279822f, 70.0f), FRotator(0.0f, 0.0f, 0.0f));
	}
	else
	{
		bRestarted = true;
	}
	NumMaxPlayers = NumMaxPlayers > 0 ? NumMaxPlayers : NUM_OF_MAX_CLIENTS;
	NetworkManager->SetGameStartCallback(NumMaxPlayers, [this]()
	{
		this->OnGameStart();
	});
	UDataManager* DataManager = GameInstance->network_failure_manager_->GetDataManager();
	check(DataManager);

	UTimeData* TimeData = DataManager->GetSingleDataAs<UTimeData>(TEXT("TimeData"));
	if (TimeData)
	{
		PlayTime = TimeData->GetTimeRemaining();
	}
	else
	{
		PlayTime = Zoomies::GAME_TIME;
	}

	monster_controllers_.resize(NUM_OF_MAX_MONSTERS, nullptr);
	empty_monster_slots_.reserve(NUM_OF_MAX_MONSTERS);
	
	for (int i = 0; i < NUM_OF_MAX_MONSTERS; i++)
	{
		empty_monster_slots_.push_back(i);
	}
	UDataArray* MonsterDataArray = DataManager->GetDataArray(TEXT("MonsterData"));
	if (MonsterDataArray)
	{
		for (UBaseData* Data : MonsterDataArray->DataArray)
		{
			UMonsterData* MonsterData = Cast<UMonsterData>(Data);
			if (MonsterData)
			{
				// Monster Stuff
			}
		}
	}
}

void ADPGameModeBase::BeginPlay()
{
	Super::BeginPlay();

#if EDITOR_MODE
	NetworkManager->Initialize(ENetworkTypeZoomies::NONE);
#elif LAN_MODE
	// NetworkManager->Initialize(ENetworkTypeZoomies::SOCKET_STEAM_LAN);
	NetworkManager->Initialize(ENetworkTypeZoomies::ENGINE_SOCKET);
#else
	NetworkManager->Initialize(ENetworkTypeZoomies::SOCKET_STEAM_P2P);
#endif

	InitializeGame();
}

void ADPGameModeBase::EndGame()
{
	bStart = false;
	NetworkManager->Shutdown();
	GetWorld()->ServerTravel("judgeLevel?listen");
}

void ADPGameModeBase::StartPlay()
{
	Super::StartPlay();
}

void ADPGameModeBase::Tick(float delta_time)
{
	Super::Tick(delta_time);
#if EDITOR_MODE != 1
	if (bStart)
	{
#endif
		if (bTimeSet == false)
		{
			bTimeSet = true;
			ADPInGameState* GS = GetGameState<ADPInGameState>();
			check(GS)
			GS->LevelAllReady();
			if (bRestarted == false)
			{
				GS->MulticastPlayerJob();
			}
			if (BlockingVolume)
			{
				BlockingVolume->DeactiveBlockingVolume(bWallDisappear);
			}
			else
			{
				bWallDisappear = true;
			}
			for (auto& pair: player_controllers_)
			{
				ADPPlayerController* Controller = pair.second;
				ADPCharacter* Character = Cast<ADPCharacter>(Controller->GetCharacter());
				if (Character == nullptr)
				{
					check(false)
				}
				Character->SetNameTag();
			}
			TimerManager->StartTimer<ADPInGameState>(PlayTime, &ADPGameModeBase::EndGame, this);
		}
		this->ProcessData(delta_time);
#if EDITOR_MODE != 1
	}
#endif
}

void ADPGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	NetworkManager->Shutdown();
	if (BlockingVolume)
		BlockingVolume->Clear();
	Super::EndPlay(EndPlayReason);

	// NetworkManager->Shutdown();
}

void ADPGameModeBase::ProcessData(float delta_time)
{
	message_queue_ = NetworkManager->GetRecievedMessages();

	if (bWallDisappear)
	{
		this->SpawnMonsters(delta_time);
		this->MonsterMoveSimulate(delta_time);
	}
	while (!this->message_queue_.empty())
	{
		Message message = this->message_queue_.front();
		this->message_queue_.pop();
		ADPPlayerController* controller = this->player_controllers_[message.player_id()];
		if (!controller)
		{
			FNetLogger::LogError(TEXT("Failed to get PlayerController. (Method: ProcessData)"));
			continue;
		}
		FServerMessageDelegate* delegate = this->message_handler_.HandleMessage(message);
		if (delegate)
		{
			delegate->ExecuteIfBound(controller, message, delta_time);
		}
	}
	this->SimulateAiming();
	this->SimulateGunFire();
	this->SimulateCatch();
	this->SyncMovement();
	this->SyncMonsterMovement();
}

void ADPGameModeBase::MonsterMoveSimulate(float delta_time)
{
	constexpr float PLAYER_SEARCH_INTERVAL = 0.5f;
	static float player_search_timer = 0.f;
	player_search_timer += delta_time;

	for (auto mc : monster_controllers_)
	{
		if (!mc)
		{
			continue;
		}
		
		ABaseMonsterCharacter* M = mc->GetPawn<ABaseMonsterCharacter>();
		
		if (!M || !IsValid(M))
		{
			RemoveMonsterData(M);
			continue;
		}
		
		if (player_search_timer >= PLAYER_SEARCH_INTERVAL)
		{
			UpdateMonsterData(M);
			M->GetMovementComponent()->PrimaryComponentTick.TickInterval = CalculateTickInterval(M);
		}

		float tTime = delta_time;
		if (ShouldProcessMonster(M, tTime))
		{
			mc->SimulateMovement(tTime);
		}
	}
	if (player_search_timer >= PLAYER_SEARCH_INTERVAL)
	{
		player_search_timer = 0.f;
	}
}

float ADPGameModeBase::CalculateTickInterval(ABaseMonsterCharacter* InMonster)
{
	FMonsterOptimizationData* MD = MonsterOptimizationData.Find(InMonster);
	if (!MD)
		return 0.f;
	const float Dist = MD->Dist;
	if (Dist >= 10000.0f)
	{
		return 1.f;
	}

	/* Total of 0~5 steps, including 10000 */
	int32 Step = FMath::FloorToInt(Dist / 2000.0f);
	if (Step == 0)
	{
		return 0;
	}
	return 0.033f * (1 << (Step - 1));
}

void ADPGameModeBase::SpawnMonsters(float delta_time)
{
	static float spawn_timer = 0.0f;
	constexpr float spawn_interval = 0.1f;
	
	spawn_timer += delta_time;
	if (spawn_timer >= spawn_interval)
	{
		if (empty_monster_slots_.size() != 0)
		{
			int32 idx = empty_monster_slots_.back();
			if ((monster_controllers_[idx] = MonsterFactory->RandomMonsterSpawn(idx)) != nullptr)
				empty_monster_slots_.pop_back();
		}
		spawn_timer = 0.0f;
	}
}

ADPGameModeBase::~ADPGameModeBase()
{
}

void ADPGameModeBase::SyncMovement()
{
	static int count = 0;
	for (auto& pair: player_controllers_)
	{
		Message msg = MessageMaker::MakePositionMessage(pair.second);
		if (pair.second->IsLocalController())
		{
			// FNetLogger::EditerLog(FColor::Cyan, TEXT("Local Player Position : %d"), count++);
		}
		NetworkManager->SendData(msg);
	}
}

void ADPGameModeBase::SimulateGunFire()
{
	for (auto& pair: player_controllers_)
	{
		ADPPlayerController* Controller = pair.second;
		UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(Controller->GetLevelComponent());
		if (MainLevelComponent == nullptr)
		{
			continue;
		}
		MainLevelComponent->SimulateGunFire(NetworkManager);
	}
}

void ADPGameModeBase::SimulateCatch()
{
	for (auto& pair: player_controllers_)
	{
		ADPPlayerController* Controller = pair.second;
		UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(Controller->GetLevelComponent());
		if (MainLevelComponent == nullptr)
		{
			continue;
		}
		MainLevelComponent->SimulateCatch(NetworkManager);
	}
}

void ADPGameModeBase::SimulateAiming()
{
	for (auto& pair: player_controllers_)
	{
		ADPPlayerController* Controller = pair.second;
		UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(Controller->GetLevelComponent());
		if (MainLevelComponent == nullptr)
		{
			continue;
		}
		MainLevelComponent->SimulateAim(NetworkManager);
	}
}

void ADPGameModeBase::SyncMonsterMovement()
{
	std::vector<MonsterPositionList> msg_list;
	msg_list.emplace_back();
	for (int i = 0; i < NUM_OF_MAX_MONSTERS; i++)
	{
		if (monster_controllers_[i] == nullptr)
		{
			continue;
		}
		MonsterPosition msg = MessageMaker::MakeMonsterPositionMessage(monster_controllers_[i]);
		if (msg.ByteSizeLong() == 0)
		{
			continue;
		}
		
		if (msg_list.back().ByteSizeLong() + msg.ByteSizeLong() > 1024)
		{
			msg_list.emplace_back();
		}
		msg_list.back().add_monster_positions()->CopyFrom(msg);
	}
	
	if (msg_list[0].ByteSizeLong() == 0)
	{
		return;
	}
	
	for (auto& msg: msg_list)
	{
		Message message;
		*message.mutable_monster_position() = msg;
		NetworkManager->SendData(message);
	}
}
