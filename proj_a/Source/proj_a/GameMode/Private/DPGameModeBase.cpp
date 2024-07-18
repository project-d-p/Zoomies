// Fill out your copyright notice in the Description page of Project Settings.

#include "DPGameModeBase.h"

#include "BaseMonsterAIController.h"
#include "CrabCharacter.h"
#include "DPCharacter.h"
#include "DPInGameState.h"
#include "DPPlayerController.h"
#include "DPPlayerState.h"
#include "SocketManager.h"
#include "Kismet/GameplayStatics.h"
#include "FNetLogger.h"
#include "ELevelComponentType.h"
#include <exception>

#include "MainLevelComponent.h"
#include "MessageMaker.h"
#include "NetworkMessage.h"
#include "OnlineSubsystem.h"
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

	monster_controllers_.resize(NUM_OF_MAX_MONSTERS, nullptr);
	empty_monster_slots_.reserve(NUM_OF_MAX_MONSTERS);

	for (int i = 0; i < NUM_OF_MAX_MONSTERS; i++)
	{
		empty_monster_slots_.push_back(i);
	}
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
			GameInstance->PlayerCharacters.Add(PlayerController, PlayerController->GetPrivateScoreManagerComponent()->GetPrivatePlayerScore());
		}
	}
}

void ADPGameModeBase::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);
#if UE_BUILD_DEBUG == 0
	if (steam_listen_socket_ == nullptr)
	{
		steam_listen_socket_ = new SteamNetworkingSocket();
		ADPInGameState* game_state_ = Cast<ADPInGameState>(GameState);
		if (game_state_ != nullptr)
			game_state_->bServerTraveled = true;
	}
#endif
	if (!newPlayer)
	{
		return ;
	}
	
	// Player state
	ADPPlayerState* player_state = Cast<ADPPlayerState>(newPlayer->PlayerState);
	if (!player_state)
	{
		return;
	}
	FString name = player_state->GetPlayerName();
	std::string key(TCHAR_TO_UTF8(*name));
	
	if (player_controllers_.find(key) != player_controllers_.end())
	{
		player_state->SetPlayerName(name + "1");
		key = std::string(TCHAR_TO_UTF8(*player_state->GetPlayerName()));
	}
	player_controllers_[key] = Cast<ADPPlayerController>(newPlayer);
	player_controllers_[key]->SwitchLevelComponent(ELevelComponentType::MAIN);
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
}

void ADPGameModeBase::EndGame()
{
	FNetLogger::EditerLog(FColor::Green, TEXT("Game Ended."));

	if (steam_listen_socket_)
	{
		steam_listen_socket_->DestoryInstance();
		delete steam_listen_socket_;
		steam_listen_socket_ = nullptr;
	}
	GetWorld()->ServerTravel("judgeLevel?listen");
}

void ADPGameModeBase::StartPlay()
{
	Super::StartPlay();

	// 재시도 로직 추가 해야함.
	UE_LOG(LogTemp, Log, TEXT("Start play."));

	TArray<AActor*> FoundCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADPCharacter::StaticClass(), FoundCharacters);
	int32 NumberOfCharacters = FoundCharacters.Num();

	UE_LOG(LogTemp, Log, TEXT("Number of ADPCharacters in the world: %d"), NumberOfCharacters);
	UE_LOG(LogTemp, Log, TEXT("Number of Players in this Session: %d"), GetNumPlayers());

	TimerManager->StartTimer<ADPInGameState>(5.0f, &ADPGameModeBase::EndGame, this);
}

void ADPGameModeBase::Tick(float delta_time)
{
	Super::Tick(delta_time);

	if (steam_listen_socket_ == nullptr)
	{
		return;
	}
	if (steam_listen_socket_->IsGameStarted())
	{
		this->ProcessData(delta_time);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Game is not started yet."));
	}
	
	// FNetLogger::EditerLog(FColor::Green, TEXT("Number of monsters: %d"), NUM_OF_MAX_MONSTERS - empty_monster_slots_.size());
}

void ADPGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("Call end play."));
	if (steam_listen_socket_)
	{
		steam_listen_socket_->DestoryInstance();
		delete steam_listen_socket_;
		steam_listen_socket_ = nullptr;
	}
}

void ADPGameModeBase::ProcessData(float delta_time)
{
	message_queue_ = steam_listen_socket_->GetReadBuffer();
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
	if (steam_listen_socket_)
	{
		steam_listen_socket_->DestoryInstance();
		delete steam_listen_socket_;
		steam_listen_socket_ = nullptr;
	}
}

void ADPGameModeBase::SyncMovement()
{
	for (auto& pair: player_controllers_)
	{
		Message msg = MessageMaker::MakePositionMessage(pair.second);
		steam_listen_socket_->PushUdpFlushMessage(msg);
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
		MainLevelComponent->SimulateGunFire(steam_listen_socket_);
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
		MainLevelComponent->SimulateCatch(steam_listen_socket_);
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
		MainLevelComponent->SimulateAim(steam_listen_socket_);
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
		
		// 1024 bytes limit to make sure that the message is not over MTU size.
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
		steam_listen_socket_->PushUdpFlushMessage(message);
	}
}
