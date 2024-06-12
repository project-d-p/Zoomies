// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

void ALobbyGameMode::PostLogin(APlayerController* newPlayer) {
	Super::PostLogin(newPlayer);

	UE_LOG(LogTemp, Log, TEXT("SESSION JOINED"));
	CheckAndStartGame();
}

void ALobbyGameMode::CheckAndStartGame() {
	if (HasAuthority()) {
		UE_LOG(LogTemp, Log, TEXT("Number of Players: %d"), GetNumPlayers());
		if (GetNumPlayers() >= 2)
			this->StartGame();
	}
}

void ALobbyGameMode::StartGame() const {
	if (HasAuthority()) {
		GetWorld()->ServerTravel(TEXT("matchLobby?listen"), true);
	}
}
