// Fill out your copyright notice in the Description page of Project Settings.


#include "DPGameModeBase.h"

#include "BaseMonsterAIController.h"
#include "BaseMonsterCharacter.h"
#include "DPCharacter.h"
#include "DPInGameState.h"
#include "DPPlayerController.h"
#include "DPPlayerState.h"
#include "SocketManager.h"
#include "Kismet/GameplayStatics.h"
#include "FNetLogger.h"
#include "MammothCharacter.h"
#include "MessageMaker.h"

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
		if (listen_socket_ == nullptr)
		{
			listen_socket_ = new FListenSocketRunnable(b_is_game_started);
			ADPInGameState* game_state_ = Cast<ADPInGameState>(GameState);
			if (game_state_ != nullptr)
				game_state_->bServerTraveled = true;
			UE_LOG(LogTemp, Warning, TEXT("Server traveled[SERVER]"));
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
	
	// Player tate
	ADPPlayerState* player_state = Cast<ADPPlayerState>(newPlayer->PlayerState);
	if (!player_state)
	{
		return;
	}

	FString name = player_state->GetPlayerName();
	FNetLogger::EditerLog(FColor::Blue, TEXT("Player name: %s"), *name);
	std::string key(TCHAR_TO_UTF8(*name));
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

	// 테스트
	SpawnAndPossessAI();
}

void ADPGameModeBase::SpawnAndPossessAI()
{
	FVector SpawnLocation = FVector(1500.0f, 0.0f, 300.0f);
	FRotator SpawnRotation = FRotator::ZeroRotator;

	// 테스트용 디폴트 StaticClass 사용
	FActorSpawnParameters SpawnParameters;

	SpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AMammothCharacter* SpawnedAICharacter =
		GetWorld()->SpawnActor<AMammothCharacter>(
			AMammothCharacter::StaticClass(),
			SpawnLocation,
			SpawnRotation,
			SpawnParameters);

	if (SpawnedAICharacter)
	{
		ABaseMonsterAIController* AIController =
			GetWorld()->SpawnActor<ABaseMonsterAIController>(ABaseMonsterAIController::StaticClass());
		if (AIController)
		{
			AIController->Possess(SpawnedAICharacter);
		}
		else
		{
			FNetLogger::LogError(TEXT("Failed to possess AI character - AIController is nullptr"));
		}
	}
	else
	{
		FNetLogger::LogError(TEXT("Failed to spawn AI character - SpawnedAICharacter is nullptr"));
	}
}


void ADPGameModeBase::Tick(float delta_time)
{
	Super::Tick(delta_time);
	if (b_is_game_started)
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
	if (listen_socket_)
	{
		delete listen_socket_;
		listen_socket_ = nullptr;
	}
}

void ADPGameModeBase::ProcessData(float delta_time)
{
	this->MergeMessages();
	// FNetLogger::EditerLog(FColor::Red, TEXT("size of message queue: %d"), this->message_queue_.size());
	if (this->message_queue_.empty())
	{
		return;
	}
	while (!this->message_queue_.empty())
	{
		Message message = this->message_queue_.front();
		// Message message = this->message_queue_.top();
		this->message_queue_.pop();
		ADPPlayerController* controller = this->player_controllers_[message.player_id()];
		message_handler_.HandleMessage(message)->ExecuteIfBound(controller, message);
	}
	this->SyncMovement();
	listen_socket_->FlushUdpQueue();
}

void ADPGameModeBase::MergeMessages()
{
	UE_LOG(LogTemp, Warning, TEXT("Merge Messages."));
	this->listen_socket_->FillMessageQueue(this->message_queue_);
}

ADPGameModeBase::~ADPGameModeBase()
{
}

void ADPGameModeBase::SyncMovement()
{
	for (auto& pair: player_controllers_)
	{
		// FNetLogger::EditerLog(FColor::Green, TEXT("player name: %s"), *FString(pair.first.c_str()));
		// UE_LOG(LogTemp, Warning, TEXT("sender name: %s"), *FString(pair.first.c_str()));
		// Message msg = MessageMaker::MakeMessage(pair.second);
		// listen_socket_->PushUdpFlushMessage(msg);
	}
}
