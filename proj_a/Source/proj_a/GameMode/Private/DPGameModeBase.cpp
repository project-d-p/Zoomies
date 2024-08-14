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

	BlockingVolume = GetWorld()->SpawnActor<ABlockingSphereVolume>(ABlockingSphereVolume::StaticClass(), FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
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
			TimerManager->StartTimer<ADPInGameState>(1000.f, &ADPGameModeBase::EndGame, this);
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
	for (auto& mc: monster_controllers_)
	{
		if (mc != nullptr)
		{
			mc->SimulateMovement(delta_time);
		}
	}
}

void ADPGameModeBase::SpawnMonsters(float delta_time)
{
	static float spawn_timer = 0.0f;
	constexpr float spawn_interval = 0.0f;

	spawn_timer += delta_time;
	if (spawn_timer >= spawn_interval)
	{
		if (empty_monster_slots_.size() != 0)
		{
			int32 idx = empty_monster_slots_.back();
			if ((monster_controllers_[idx] = MonsterFactory->RandomMonsterSpawn(idx)) != nullptr)
				empty_monster_slots_.pop_back();
		}
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
