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

	//// TEST
	NetworkManager = CreateDefaultSubobject<UServerNetworkManager>(TEXT("NetworkManager"));
	//// TEST

	monster_controllers_.resize(NUM_OF_MAX_MONSTERS, nullptr);
	empty_monster_slots_.reserve(NUM_OF_MAX_MONSTERS);

	for (int i = 0; i < NUM_OF_MAX_MONSTERS; i++)
	{
		empty_monster_slots_.push_back(i);
	}
}

void ADPGameModeBase::OnGameStart()
{
	this->bStart = true;
	
	TimerManager->StartTimer<ADPInGameState>(30.0f, &ADPGameModeBase::EndGame, this);
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

void ADPGameModeBase::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);
	/*
	 * TEST : COMMENTS
#if UE_BUILD_DEBUG == 0
	if (steam_listen_socket_ == nullptr)
	{
		steam_listen_socket_ = new SteamNetworkingSocket();
		ADPInGameState* game_state_ = Cast<ADPInGameState>(GameState);
		if (game_state_ != nullptr)
			game_state_->bServerTraveled = true;
	}
#endif
	*/
	check(newPlayer);
	// Player state
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

	if (!newPlayer->IsLocalController())
	{
		player_controllers_[key]->ConnectToServer();
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
}

void ADPGameModeBase::EndGame()
{
	FNetLogger::EditerLog(FColor::Green, TEXT("Game Ended."));

	/*
	 * TEST : COMMENTS 
	if (steam_listen_socket_)
	{
		steam_listen_socket_->DestoryInstance();
		delete steam_listen_socket_;
		steam_listen_socket_ = nullptr;
	}
	*/

	/// TEST
	NetworkManager->Shutdown();
	///
	
	GetWorld()->ServerTravel("judgeLevel?listen");
}

void ADPGameModeBase::StartPlay()
{
	Super::StartPlay();

	NetworkManager->Initialize(ENetworkTypeZoomies::SOCKET_STEAM_LAN);
	NetworkManager->SetGameStartCallback(NUM_OF_MAX_CLIENTS, [this]()
	{
		this->OnGameStart();
	});
}

void ADPGameModeBase::Tick(float delta_time)
{
	Super::Tick(delta_time);

	/*
	 * TEST : COMMENTS
	if (steam_listen_socket_ == nullptr)
	{
		return;
	} 
	if (steam_listen_socket_->IsGameStarted())
	*/
	if (bStart)
	{
		this->ProcessData(delta_time);
	}
}

void ADPGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("Call end play."));
	/*
	 * TEST : COMMENTS
	if (steam_listen_socket_)
	{
		steam_listen_socket_->DestoryInstance();
		delete steam_listen_socket_;
		steam_listen_socket_ = nullptr;
	}
	*/

	/// TEST
	NetworkManager->Shutdown();
	///
}

void ADPGameModeBase::ProcessData(float delta_time)
{
	/*
	 * TEST : COMMENTS
	message_queue_ = steam_listen_socket_->GetReadBuffer();
	 */

	/// TEST
	message_queue_ = NetworkManager->GetRecievedMessages();
	///

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
	/*
	 * TEST : COMMENTS
	if (steam_listen_socket_)
	{
		steam_listen_socket_->DestoryInstance();
		delete steam_listen_socket_;
		steam_listen_socket_ = nullptr;
	}
	 */
	/// TEST
	NetworkManager->Shutdown();
	///
}

void ADPGameModeBase::SyncMovement()
{
	for (auto& pair: player_controllers_)
	{
		Message msg = MessageMaker::MakePositionMessage(pair.second);
		/* 
		 * TEST : COMMENTS
		steam_listen_socket_->PushUdpFlushMessage(msg);
		 */

		/// TEST
		NetworkManager->SendData(msg);
		///
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
		/*
		 * TEST : COMMENTS
		MainLevelComponent->SimulateGunFire(steam_listen_socket_);
		*/
		/// TEST
		MainLevelComponent->SimulateGunFire(NetworkManager);
		///
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
		/*
		 * TEST : COMMENTS
		MainLevelComponent->SimulateCatch(steam_listen_socket_);
		*/
		/// TEST
		MainLevelComponent->SimulateCatch(NetworkManager);
		///
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
		/*
		 * TEST : COMMENTS
		MainLevelComponent->SimulateAim(steam_listen_socket_);
		*/
		/// TEST
		MainLevelComponent->SimulateAim(NetworkManager);
		///
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
		/*
		 * TEST : COMMENTS
		steam_listen_socket_->PushUdpFlushMessage(message);
		 */

		/// TEST
		NetworkManager->SendData(message);
		///
	}
}
