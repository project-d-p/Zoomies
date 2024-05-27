// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DPCharacter.h"

void ALobbyGameMode::PostLogin(APlayerController* newPlayer) {
	Super::PostLogin(newPlayer);

	this->CheckAndStartGame();
}

void ALobbyGameMode::CheckAndStartGame() {
	UE_LOG(LogTemp, Log, TEXT("Number of Players: %d"), GetNumPlayers());
	if (GetNumPlayers() >= 4)
		this->StartGame();
}

void ALobbyGameMode::StartGame() {
	if (HasAuthority()) {
		GetWorld()->ServerTravel(TEXT("mainLevel?listen"), true);
	}
	TArray<AActor*> found_actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADPCharacter::StaticClass(), found_actors);

	for (AActor* actor : found_actors) {
		ADPCharacter* my_character = Cast<ADPCharacter>(actor);
		if (my_character) {
			my_character->SetReplicates(false);
			my_character->GetCharacterMovement()->SetIsReplicated(false);
		}
	}
}