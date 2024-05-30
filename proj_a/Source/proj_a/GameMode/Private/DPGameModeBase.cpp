// Fill out your copyright notice in the Description page of Project Settings.


#include "DPGameModeBase.h"
#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "SocketManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ADPGameModeBase::ADPGameModeBase()
{
	DefaultPawnClass = ADPCharacter::StaticClass();
	PlayerControllerClass = ADPPlayerController::StaticClass();
}

void ADPGameModeBase::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);

	UE_LOG(LogTemp, Log, TEXT("mainLevel PostLogin"));
	UE_LOG(LogTemp, Log, TEXT("SESSION JOINED: %d"), GetNumPlayers());
	TArray<AActor*> FoundCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADPCharacter::StaticClass(), FoundCharacters);
	int32 NumberOfCharacters = FoundCharacters.Num();

	UE_LOG(LogTemp, Log, TEXT("Number of ADPCharacters in the world: %d"), NumberOfCharacters);
	ADPCharacter* player_character = Cast<ADPCharacter>(newPlayer->GetPawn());
	if (player_character != nullptr)
	{
		// if i set this to false, the character in client side was not seen.
		// player_character->SetReplicates(false);
		// if i set this to false, i don't know what happen. Nothings Different.
		player_character->GetCharacterMovement()->SetIsReplicated(false);
	}
}

void ADPGameModeBase::DisableReplicationForCharacters()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADPCharacter::StaticClass(), FoundActors);
	for (AActor* actor : FoundActors)
	{
		UE_LOG(LogTemp, Log, TEXT("Disable replication for character."));
		ADPCharacter* character = Cast<ADPCharacter>(actor);
		if (character)
		{
			character->SetReplicates(false);
			character->GetCharacterMovement()->SetIsReplicated(false);
		}
	}
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
	// this->DisableReplicationForCharacters();
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle_DisableReplication, this, &ADPGameModeBase::DisableReplicationForCharacters, 5.0f, false);
}

void ADPGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("Call end play."));
	FSocketManager::GetInstance().Close();
}