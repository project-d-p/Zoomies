// Fill out your copyright notice in the Description page of Project Settings.


#include "DPGameModeBase.h"
#include "DPCharacter.h"
#include "DPInGameState.h"
#include "DPPlayerController.h"
#include "Net/UnrealNetwork.h"

ADPGameModeBase::ADPGameModeBase()
{
	DefaultPawnClass = ADPCharacter::StaticClass();
	PlayerControllerClass = ADPPlayerController::StaticClass();
	GameStateClass = ADPInGameState::StaticClass();
	TimerManager = CreateDefaultSubobject<UServerTimerManager>(TEXT("TimerManager"));

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ADPGameModeBase::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);
	try
	{
		if (listen_socket_ == nullptr)
			listen_socket_ = new FListenSocketRunnable(b_is_game_started);
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create listen socket: %hs"), UTF8_TO_TCHAR(e.what()));
	}
	if (newPlayer)
	{
		APawn* pawn = newPlayer->GetPawn();
		if (pawn)
		{
			FString name = pawn->GetName();
			UE_LOG(LogTemp, Warning, TEXT("Player name: %s"), *name);
			player_controllers_.emplace(name, Cast<ADPPlayerController>(newPlayer));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NO PLAYER NAME"));
		}
	}
}

void ADPGameModeBase::StartPlay()
{
	Super::StartPlay();
}

void ADPGameModeBase::Tick(float delta_time)
{
	Super::Tick(delta_time);
	if (b_is_game_started)
	{
		TimerManager->StartTimer(60.0f);
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

ADPGameModeBase::~ADPGameModeBase()
{
}

void ADPGameModeBase::ProcessData(float delta_time)
{
	this->MergeMessages();
	while (!this->message_queue_.empty())
	{
		Message message = this->message_queue_.top();
		this->message_queue_.pop();
		ADPPlayerController* controller = this->player_controllers_[message.player_id().c_str()];
		
	}
	this->UpdateTime(delta_time);
}

void ADPGameModeBase::MergeMessages()
{
	UE_LOG(LogTemp, Warning, TEXT("Merge Messages."));
	this->listen_socket_->FillMessageQueue(this->message_queue_);
}