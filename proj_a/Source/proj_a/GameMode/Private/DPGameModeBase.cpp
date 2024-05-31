// Fill out your copyright notice in the Description page of Project Settings.

#include "DPGameModeBase.h"
#include "DPCharacter.h"
#include "DPInGameState.h"
#include "DPPlayerController.h"
#include "FNetLogger.h"
#include "SocketManager.h"

ADPGameModeBase::ADPGameModeBase()
{
	DefaultPawnClass = ADPCharacter::StaticClass();
    // static ConstructorHelpers::FClassFinder<APawn> CHARACTER
    // (TEXT("/Game/blueprints/bp_character.bp_character_C"));
    // if (CHARACTER.Class != nullptr)
    // {
    //     DefaultPawnClass = CHARACTER.Class;
    // }
	PlayerControllerClass = ADPPlayerController::StaticClass();
	GameStateClass = ADPInGameState::StaticClass();
}

void ADPGameModeBase::StartPlay()
{
	Super::StartPlay();
	if (GEngine)
	{
		GEngine->GameViewport->Exec(nullptr, TEXT("r.SetRes 1280x720w"), *GLog);
	}

	// 재시도 로직 추가 해야함.
	// FSocketManager &SocketManager = FSocketManager::GetInstance();
	//
	// bool success = SocketManager.Connect("10.19.225.124", 4242);
	// if (success) {
	// 	UE_LOG(LogTemp, Warning, TEXT("Connection established successfully."));
	// } else {
	// 	UE_LOG(LogTemp, Warning, TEXT("Failed to establish connection."));
	// }
	// FSocketManager::GetInstance().RunTask();
}

/**
 * 적절한 스폰 위치 설정  
*/
#define SPAWN_LOCATION (FVector(0.0f, 0.0f, 300.0f));

void ADPGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (NewPlayer->GetNetConnection())
	{
		FString ClientAddress = NewPlayer->GetNetConnection()->LowLevelGetRemoteAddress();
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Client connected: %s"), *ClientAddress);
	}
	
	if (DefaultPawnClass != nullptr && NewPlayer)
	{
		FVector SpawnLocation = SPAWN_LOCATION;
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = NewPlayer;
		SpawnParams.Instigator = NewPlayer->GetPawn();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ACharacter* NewCharacter = GetWorld()->SpawnActor<ACharacter>(DefaultPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
        
		if (NewCharacter)
		{
			NewPlayer->Possess(NewCharacter);
		}
	}
}

void ADPGameModeBase::BroadcastChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ADPPlayerController* PC = Cast<ADPPlayerController>(*It);
		if (PC)
		{
			PC->ReceiveChatMessage(SenderName, Message);
		}
	}
}

void ADPGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("Call end play."));
	FSocketManager::GetInstance().Close();
}
