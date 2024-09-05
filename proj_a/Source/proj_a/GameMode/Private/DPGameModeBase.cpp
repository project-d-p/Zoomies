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
#include "proj_a/GameInstance/GI_Zoomies.h"

ADPGameModeBase::ADPGameModeBase()
{
	bUseSeamlessTravel = true;
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	
	DefaultPawnClass = ADPCharacter::StaticClass();
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

	monster_controllers_.resize(NUM_OF_MAX_MONSTERS, nullptr);
	empty_monster_slots_.reserve(NUM_OF_MAX_MONSTERS);

	for (int i = 0; i < NUM_OF_MAX_MONSTERS; i++)
	{
		empty_monster_slots_.push_back(i);
	}
}

void ADPGameModeBase::OnGameStart()
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	check(GameInstance);

	GameInstance->ChangeJoinInProgress(false);
	this->bStart = true;
}

// Only Called in Server : PlayerController && PlayerState Automatically Travel
void ADPGameModeBase::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransition, ActorList);

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (!GameInstance)
		return ;
	
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
		FVector(-230.000000,230.000000,10.000000),
		FVector(-230.000000,-250.000000,10.000000),
		FVector(270.000000,-250.000000,10.000000),
		FVector(270.000000,230.000000,10.000000),
	};
	static int idx = 0;
	if (idx >= 4)
		idx = 0;
	FVector SpawnLocation = Location[idx++];

	// ADPCharacter* NewCharacter = GetWorld()->SpawnActor<ADPCharacter>(DefaultPawnClass, SpawnLocation, FRotator::ZeroRotator);
	
	// As we set the default pawn class to ADPCharacter, we can use the following code to relocate an existing character.
	NewPlayer->GetCharacter()->SetActorLocation(SpawnLocation);
}

void ADPGameModeBase::UpdateMonsterData(ABaseMonsterCharacter* InMonster)
{
	auto [ClosestPlayer, MinDistance] = FindClosestPlayer(InMonster);
	if (ClosestPlayer)
	{
		float MoveInterval = CalculateMoveInterval(MinDistance);
		// FNetLogger::EditerLog(FColor::Green, TEXT("Monster: %s, ClosestPlayer: %s, Distance: %f, Interval: %f"),
		// 	*InMonster->GetName(), *ClosestPlayer->GetName(), MinDistance, MoveInterval);
		
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

	FString name = player_state->GetPlayerName();
	std::string key(TCHAR_TO_UTF8(*name));
	
	if (player_controllers_.find(key) != player_controllers_.end())
	{
		player_state->SetPlayerName(name + "1");
		key = std::string(TCHAR_TO_UTF8(*player_state->GetPlayerName()));
	}
	player_controllers_[key] = Cast<ADPPlayerController>(newPlayer);
	player_controllers_[key]->SwitchLevelComponent(ELevelComponentType::MAIN);

	SpawnNewCharacter(newPlayer); 
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
		StartGame();
	}
}

void ADPGameModeBase::StartGame()
{
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
	std::string key(TCHAR_TO_UTF8(*controller->PlayerState->GetPlayerName()));
	if (player_controllers_.find(key) != player_controllers_.end())
	{
		player_controllers_.erase(key);
	}
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->AddBanPlayer(controller->PlayerState->GetUniqueId()->ToString());
	}
}

void ADPGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	BlockingVolume = GetWorld()->SpawnActor<ABlockingBoxVolume>(ABlockingBoxVolume::StaticClass(), FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
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

#if EDITOR_MODE
	NetworkManager->Initialize(ENetworkTypeZoomies::NONE);
#elif LAN_MODE
	NetworkManager->Initialize(ENetworkTypeZoomies::SOCKET_STEAM_LAN);
#else
	NetworkManager->Initialize(ENetworkTypeZoomies::SOCKET_STEAM_P2P);
#endif
	
	NetworkManager->SetGameStartCallback(NUM_OF_MAX_CLIENTS, [this]()
	{
		this->OnGameStart();
	});
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
			BlockingVolume->DeactiveBlockingVolume();
			bTimeSet = true;
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
			TimerManager->StartTimer<ADPInGameState>(300.f, &ADPGameModeBase::EndGame, this);
		}

		this->ProcessData(delta_time);
#if EDITOR_MODE != 1
	}
#endif
}

void ADPGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("Call end play."));

	NetworkManager->Shutdown();
}

void ADPGameModeBase::ProcessData(float delta_time)
{
	message_queue_ = NetworkManager->GetRecievedMessages();

	this->SpawnMonsters(delta_time);
	this->MonsterMoveSimulate(delta_time);
	while (!this->message_queue_.empty())
	{
		Message message = this->message_queue_.front();
		this->message_queue_.pop();
		ADPPlayerController* controller = this->player_controllers_[message.player_id()];
		message_handler_.HandleMessage(message)->ExecuteIfBound(controller, message, delta_time);
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
	for (auto& pair: player_controllers_)
	{
		Message msg = MessageMaker::MakePositionMessage(pair.second);
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
