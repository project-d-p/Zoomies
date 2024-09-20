#include "GS_MatchingLobby.h"

#include "CompileMode.h"
#include "DPCharacter.h"
#include "FNetLogger.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/GameInstance/GI_Zoomies.h"
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

void AGS_MatchingLobby::BeginPlay()
{
	Super::BeginPlay();

	// Delegate for handling network failure On This Level
	if (!HasAuthority())
	{
		UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
		if (GameInstance)
		{
			FNetLogger::EditerLog(FColor::Red, TEXT("GameInstance is not null"));
			GameInstance->network_failure_manager_->OnHostMigration().AddUObject(this, &AGS_MatchingLobby::OnHostMigration);
		}
	}
}

void AGS_MatchingLobby::OnHostMigration(UWorld* World, UDataManager* DataManager)
{
	// UE_LOG(LogTemp, Warning, TEXT("Network Failure: %s"), *ErrorString);
	
	// if (FailureType == ENetworkFailure::ConnectionLost || FailureType == ENetworkFailure::ConnectionTimeout)
	// {
		// 서버 연결이 끊어졌을 때 처리
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, ADPCharacter::StaticClass(), FoundActors);
	
		for (AActor* Actor : FoundActors)
		{
			ADPCharacter* Character = Cast<ADPCharacter>(Actor);
			FNetLogger::EditerLog(FColor::Red, TEXT("Character %s was here."), Character->GetPlayerState() ? *Character->GetPlayerState()->GetPlayerName() : TEXT("No PlayerState"));
		}
	
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PlayerController = It->Get();
			if (PlayerController)
			{
				FNetLogger::EditerLog(FColor::Red, TEXT("PlayerController %d was here."), PlayerController->GetUniqueID());
			}
		}
		
		FString LevelName = World->GetMapName();
		UE_LOG(LogTemp, Warning, TEXT("Connection to the server has been lost or timed out."));
		FNetLogger::EditerLog(FColor::Red, TEXT("Connection to the server has been lost or timed out. in %s"), *LevelName);
	// }
}

void AGS_MatchingLobby::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
}

void AGS_MatchingLobby::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	// if (!HasAuthority())
	// {
	// 	if (GEngine)
	// 	{
	// 		GEngine->OnNetworkFailure().RemoveAll(this);
	// 	}
	// }
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


// 로직 순서
// 1. 네트워크 끊김 감지
// 2. 해당 레벨에 있던 데이터들을 저장
// 2-1. 각 레벨마다 저장할 데이터가 다름
// 3. 해당 레벨을 다른 클라이언트가 다시 호스팅함
// 4. 다시 호스팅된 클라이언트가 저장한 데이터를 불러옴
// 5. 불러온 데이터를 기반으로 다시 게임을 시작함