// Fill out your copyright notice in the Description page of Project Settings.


#include "DPGameModeBase.h"
#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "Net/UnrealNetwork.h"

ADPGameModeBase::ADPGameModeBase()
{
	DefaultPawnClass = ADPCharacter::StaticClass();
	PlayerControllerClass = ADPPlayerController::StaticClass();
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
		if (this->IsGameOver())
		{
			UE_LOG(LogTemp, Warning, TEXT("Game Over!"));
			return ;
		}
		this->StartGameLogic(delta_time);
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

void ADPGameModeBase::UpdateTime(float delta_time)
{
	time_accumulator_ += delta_time;
	if (time_accumulator_ >= 1.0f)
	{
		remain_time_ -= 1;
		time_accumulator_ = 0.0f;
	}
}

bool ADPGameModeBase::IsGameOver() const
{
	return remain_time_ <= 0;
}

void ADPGameModeBase::StartGameLogic(float delta_time)
{
	this->MergeMessages();
	while (!this->message_queue_.empty())
	{
		Message message = this->message_queue_.top();
		this->message_queue_.pop();
		UE_LOG(LogTemp, Warning, TEXT("Message: %s"), message.DebugString().c_str());
	}
	this->UpdateTime(delta_time);
}

void ADPGameModeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPGameModeBase, remain_time_);
}

void ADPGameModeBase::MergeMessages()
{
	UE_LOG(LogTemp, Warning, TEXT("Merge Messages."));
	this->listen_socket_->FillMessageQueue(this->message_queue_);
}