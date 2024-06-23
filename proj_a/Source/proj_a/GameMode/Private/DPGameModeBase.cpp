// Fill out your copyright notice in the Description page of Project Settings.


#include "DPGameModeBase.h"
#include "DPCharacter.h"
#include "DPInGameState.h"
#include "DPPlayerController.h"
#include "DPPlayerState.h"
#include "SocketManager.h"
#include "Kismet/GameplayStatics.h"
#include "FNetLogger.h"
#include "isteamnetworkingsockets.h"
#include "MessageMaker.h"
// #include "steam_api_flat.h"

ADPGameModeBase::ADPGameModeBase()
{
	DefaultPawnClass = ADPCharacter::StaticClass();
	PlayerControllerClass = ADPPlayerController::StaticClass();
	PlayerStateClass = ADPPlayerState::StaticClass();
	GameStateClass = ADPInGameState::StaticClass();

	TimerManager = CreateDefaultSubobject<UServerTimerManager>(TEXT("TimerManager"));
	ChatManager = CreateDefaultSubobject<UServerChatManager>(TEXT("ChatManager"));
	ScoreManager = CreateDefaultSubobject<UScoreManagerComp>(TEXT("ScoreManager"));

	PrimaryActorTick.bCanEverTick = true;
	// PrimaryActorTick.TickInterval = 0.01f;
	bReplicates = true;
}

void ADPGameModeBase::SendChatToAllClients(const FString& SenderName, const FString& Message)
{
	ChatManager->BroadcastChatMessage(SenderName, Message);
}

void ADPGameModeBase::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);
	try
	{
		if (steam_listen_socket_ == nullptr)
		{
			steam_listen_socket_ = new SteamNetworkingSocket();
			ADPInGameState* game_state_ = Cast<ADPInGameState>(GameState);
			if (game_state_ != nullptr)
				game_state_->bServerTraveled = true;
			FNetLogger::EditerLog(FColor::Cyan, TEXT("Server traveled[SERVER]"));
		}
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create listen socket: %hs"), UTF8_TO_TCHAR(e.what()));
	}

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
	FNetLogger::EditerLog(FColor::Blue, TEXT("Player name: %s"), *name);
	std::string key(TCHAR_TO_UTF8(*name));
	if (player_controllers_.find(key) != player_controllers_.end())
	{
		player_state->SetPlayerName(name + "1");
		FNetLogger::EditerLog(FColor::Red, TEXT("Player name: %s"), *player_state->GetPlayerName());
		key = std::string(TCHAR_TO_UTF8(*player_state->GetPlayerName()));
	}
	player_controllers_[key] = Cast<ADPPlayerController>(newPlayer);
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

	TimerManager->StartTimer(60.0f);
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
}

void ADPGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("Call end play."));
	if (steam_listen_socket_)
	{
		delete steam_listen_socket_;
		steam_listen_socket_ = nullptr;
	}
}

void ADPGameModeBase::ProcessData(float delta_time)
{
	message_queue_ = steam_listen_socket_->GetReadBuffer();
	while (!this->message_queue_.empty())
	{
		Message message = this->message_queue_.front();
		this->message_queue_.pop();
		ADPPlayerController* controller = this->player_controllers_[message.player_id()];
		message_handler_.HandleMessage(message)->ExecuteIfBound(controller, message, delta_time);
	}
	this->SyncHostAiming();
	this->SimulateGunFire();
	this->SyncMovement();
}

ADPGameModeBase::~ADPGameModeBase()
{
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
		ADPPlayerController* controller = pair.second;
		ADPCharacter* character = Cast<ADPCharacter>(controller->GetPawn());
		controller->SimulateGunFire(steam_listen_socket_);
	}
}

void ADPGameModeBase::SyncHostAiming()
{
	ADPPlayerController* host_controller = Cast<ADPPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!host_controller)
	{
		return ;
	}
	while (!host_controller->aim_queue_.empty())
	{
		bool aim_state = host_controller->aim_queue_.front();
		host_controller->aim_queue_.pop();
		Message msg = MessageMaker::MakeAimMessage(host_controller, aim_state);
		steam_listen_socket_->PushUdpFlushMessage(msg);
	}
}
