#include "GS_MatchingLobby.h"

#include "DPCharacter.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/MatchingLobby/GM_MatchingLobby/GM_MatchingLobby.h"

AGS_MatchingLobby::AGS_MatchingLobby() {
	// Set Players Num. need to be Set
	ReadyPlayers.SetNum(MAX_USERS, false);
	LobbyInfos.SetNum(MAX_USERS, false);
	BestHostPlayer = nullptr;
	LowestAveragePing = 202406071806.0f;
	HostPlayerIndex = -1;
}

void AGS_MatchingLobby::OnRep_LobbyInfo()
{
	UpdateLobbyInfo();
}

void AGS_MatchingLobby::UpdateLobbyInfo() const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADPCharacter::StaticClass(), FoundActors);
	for (int32 j = 0; j < FoundActors.Num(); j++)
	{
		ADPCharacter* Character = Cast<ADPCharacter>(FoundActors[j]);
		if (Character && Character->LobbyInfoWidgetComponent)
		{
			UUserWidget* Widget = Character->LobbyInfoWidgetComponent->GetUserWidgetObject();
			if (Widget)
			{
				FString Command = FString::Printf(TEXT("Update %d"), j);
				Widget->CallFunctionByNameWithArguments(*Command, *GLog, nullptr, true);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("&*Widget is null for Character at index %d"), j);
			}
		}
	}
}

void AGS_MatchingLobby::SetPlayerReady(int32 PlayerIndex, bool bIsReady)
{
	if (PlayerIndex >= 0 && PlayerIndex < ReadyPlayers.Num())
	{
		ReadyPlayers[PlayerIndex] = bIsReady;
		LobbyInfos[PlayerIndex].bIsReady = bIsReady;
		UpdateLobbyInfo();
	}
	if (HasAuthority())
	{
		AGM_MatchingLobby* GM = GetWorld()->GetAuthGameMode<AGM_MatchingLobby>();
		if (GM)
		{
			GM->CheckReadyToStart();
		}
	}
}

// set the ReadyPlayers array to be replicated
void AGS_MatchingLobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGS_MatchingLobby, ReadyPlayers);
	DOREPLIFETIME(AGS_MatchingLobby, LobbyInfos);
	DOREPLIFETIME(AGS_MatchingLobby, LowestAveragePing);
	DOREPLIFETIME(AGS_MatchingLobby, BestHostPlayer);
}

bool AGS_MatchingLobby::AreAllPlayersReady()
{
	for (int32 i = 0; i < ReadyPlayers.Num(); ++i)
	{
		if (!ReadyPlayers[i])
		{
			return false;
		}
	}
	FindFastestPlayer();
	return true;
}

//Find the fastest player
void AGS_MatchingLobby::FindFastestPlayer() 
{
	float average_ping = 0.0f;
	APlayerState* CurrentPlayerState = GetWorld()->GetFirstPlayerController()->PlayerState;
	
	AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
	if (GameState)
	{
		for (APlayerState* PlayerState : GameState->PlayerArray)
		{
			float PlayerPing = PlayerState->ExactPing;
			average_ping += PlayerPing;
		}
		average_ping /= GameState->PlayerArray.Num();
		
		//logging
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				30.f,
				FColor::Green,
				FString::Printf(TEXT("Average Ping: %f"), average_ping));
		}
		ReportPing(CurrentPlayerState, average_ping);
	}
}

void AGS_MatchingLobby::ReportPing_Implementation(APlayerState* ReportingPlayer, float AveragePing)
{
	if (AveragePing < LowestAveragePing)
	{
		LowestAveragePing = AveragePing;
		BestHostPlayer = ReportingPlayer;
	}
}

void AGS_MatchingLobby::MulticastShowLoadingWidget_Implementation()
{
	UClass* WidgetClass = nullptr;
	FString WidgetPath = TEXT("/Game/widget/widget_loading.widget_loading_C");

	WidgetClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *WidgetPath));

	if (WidgetClass != nullptr)
	{
		UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), WidgetClass);

		if (LoadingWidget != nullptr)
		{
			LoadingWidget->AddToViewport();
		}
		else
		{
			//logging on screen about failed to create widget
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					30.f,
					FColor::Red,
					FString::Printf(TEXT("Failed to create widget")));
			}
		}
	}
}


