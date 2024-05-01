// Fill out your copyright notice in the Description page of Project Settings.


#include "DPGameModeBase.h"
#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "SocketManager.h"

ADPGameModeBase::ADPGameModeBase()
{
	DefaultPawnClass = ADPCharacter::StaticClass();
	PlayerControllerClass = ADPPlayerController::StaticClass();
}

void ADPGameModeBase::StartPlay()
{
	Super::StartPlay();

	// 재시도 로직 추가 해야함.
	FSocketManager &SocketManager = FSocketManager::GetInstance();
	
	bool success = SocketManager.Connect("10.19.225.124", 4242);
	if (success) {
		UE_LOG(LogTemp, Warning, TEXT("Connection established successfully."));
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Failed to establish connection."));
	}
	FSocketManager::GetInstance().RunTask();
}

void ADPGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("Call end play."));
	FSocketManager::GetInstance().Close();
}