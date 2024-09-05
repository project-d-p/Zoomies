#include "GM_MatchingLobby.h"

#include "CompileMode.h"
#include "DPCharacter.h"
#include "isteamfriends.h"
#include "isteamuser.h"
#include "isteamutils.h"
#include "steamclientpublic.h"
#include "Components/BackgroundBlur.h"
#include "GameFramework/PlayerState.h"
#include "proj_a/MatchingLobby/GS_MachingLobby/GS_MatchingLobby.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"
#include "proj_a/MatchingLobby/PS_MatchingLobby/PS_MatchingLobby.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

AGM_MatchingLobby::AGM_MatchingLobby() {
	GameStateClass = AGS_MatchingLobby::StaticClass();
	PlayerControllerClass = APC_MatchingLobby::StaticClass();
	PlayerStateClass = APS_MatchingLobby::StaticClass();
	DefaultPawnClass = nullptr;
}

void AGM_MatchingLobby::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);
	AGS_MatchingLobby* GS = GetGameState<AGS_MatchingLobby>();
	
	if (GS->HostPlayerIndex == -1)
	{
		GS->HostPlayerIndex = NewPlayer->PlayerState->GetPlayerId();
	}
	
	PCs.Add(NewPlayer);
	CheckAndUpdateLobbyPlatform();
}

void AGM_MatchingLobby::Logout(AController* Exiting) {
	Super::Logout(Exiting);

	if (APlayerController* ExitingPlayer = Cast<APlayerController>(Exiting))
	{
		PCs.Remove(ExitingPlayer);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AGM_MatchingLobby::Logout: Exiting is not a PlayerController"));
	}
	CheckAndUpdateLobbyPlatform();
}

void AGM_MatchingLobby::BeginPlay() {
	Super::BeginPlay();
	FindAndStoreLobbyPlatforms();
	getMatchLobbyUI();
}

void AGM_MatchingLobby::CheckReadyToStart() 
{
	if (HasAuthority())
	{
		AGS_MatchingLobby* GS = GetGameState<AGS_MatchingLobby>();
		if (GS && GS->AreAllPlayersReady() && GetNumPlayers() >= MAX_USERS)
		{
			GS->MulticastShowLoadingWidget();
			StartGame_t();
		}
	}
}

void AGM_MatchingLobby::StartGame_t() const
{
	if (HasAuthority())
	{
		GetWorld()->ServerTravel(TEXT("mainLevel?listen") , true);
	}
}

void AGM_MatchingLobby::FindAndStoreLobbyPlatforms()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALobbyPlatform::StaticClass(), FoundActors);

	LobbyPlatforms.Init(nullptr, MAX_USERS);
	if (FoundActors.Num() != MAX_USERS)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGM_MatchingLobby::FindAndStoreLobbyPlatforms: FoundActors.Num() != MAX_USERS"));
	}
	
	for (AActor* Actor : FoundActors)
	{
		if (Actor->Tags.Num() > 0)
		{
			FString Tag = Actor->Tags[0].ToString();
			int32 Index = FCString::Atoi(Tag.GetCharArray().GetData());
	
			if (Index >= 0 && Index < LobbyPlatforms.Num())
			{
				LobbyPlatforms[Index] = Cast<ALobbyPlatform>(Actor);
			}
		}
	}
	
	bIsLobbyPlatformReady = true;
}


void AGM_MatchingLobby::CheckAndUpdateLobbyPlatform()
{
	if (!bIsLobbyPlatformReady)
	{
		GetWorldTimerManager().SetTimer(
			UnusedHandle,
			this, &AGM_MatchingLobby::CheckAndUpdateLobbyPlatform,
			0.01f,
			false);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(UnusedHandle);
		UpdatePlayerOnPlatform();
	}
}

void AGM_MatchingLobby::UpdatePlayerOnPlatform()
{
	for (int32 i = 0; i < PCs.Num(); i++)
	{
		bool bIsPlayerOnPlatform = false;
		
		//if player is on platform, set bIsPlayerOnPlatform to true
		if (LobbyPlatforms.IsValidIndex(i))
		{
			for (int32 j = 0; j < LobbyPlatforms.Num(); j++)
			{
				if (LobbyPlatforms[j] && LobbyPlatforms[j]->PC != nullptr)
				{
					if (PCs[i] == LobbyPlatforms[j]->PC)
					{
						bIsPlayerOnPlatform = true;
						break;
					}
				}
			}
		}
		
		//if player is not on platform, spawn character on platform
		if (!bIsPlayerOnPlatform)
		{
			for (int32 j = 0; j < LobbyPlatforms.Num(); j++)
			{
				if (LobbyPlatforms[j] && LobbyPlatforms[j]->PC == nullptr)
				{
					LobbyPlatforms[j]->SpawnCharacter(PCs[i]);
					if (AGS_MatchingLobby* GS = GetGameState<AGS_MatchingLobby>())
					{
						FString steam_username = PCs[i]->PlayerState->GetPlayerName();
						GS->LobbyInfos[j].Name = steam_username;
						GS->LobbyInfos[j].PC = Cast<APC_MatchingLobby>(PCs[i]);
						GS->LobbyInfos[j].PS = Cast<APS_MatchingLobby>(PCs[i]->PlayerState);
						GS->UpdateLobbyInfo();
					}
					break;
				}
			}
		}
	}
	
	for (int32 i = 0; i < LobbyPlatforms.Num(); i++)
	{
		if (LobbyPlatforms[i])
		{
			if (LobbyPlatforms[i]->PC && !PCs.Contains(LobbyPlatforms[i]->PC))
			{
				LobbyPlatforms[i]->Clear_Platform();
				if (AGS_MatchingLobby* GS = GetGameState<AGS_MatchingLobby>())
				{
					int32 playerIndex = i;
					if (playerIndex >= 0 && playerIndex < GS->ReadyPlayers.Num())
					{
						GS->ReadyPlayers[i] = false;
						GS->LobbyInfos[i].bIsReady = false;
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("AGM_MatchingLobby::UpdatePlayerOnPlatform: playerIndex is out of range"));
					}
				}
			}
		}
	}
}

void AGM_MatchingLobby::getMatchLobbyUI()
{
	// UI 위젯 클래스의 경로 설정
	FString WidgetPath = TEXT("/Game/widget/WBP_MatchLobby/widget_match_ready.widget_match_ready_C");

	// UClass를 로드
	UClass* WidgetClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *WidgetPath));

	if (WidgetClass != nullptr)
	{
		// 위젯을 생성하고 화면에 추가
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			MatchLobbyWidget = CreateWidget<UUserWidget>(PlayerController, WidgetClass);

			if (MatchLobbyWidget != nullptr)
			{
				MatchLobbyWidget->AddToViewport();
				UE_LOG(LogTemp, Log, TEXT("AGM_MatchingLobby::getMatchLobbyUI: Widget successfully created and added to viewport."));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AGM_MatchingLobby::getMatchLobbyUI: MatchLobbyWidget is nullptr after creation."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AGM_MatchingLobby::getMatchLobbyUI: PlayerController is nullptr."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AGM_MatchingLobby::getMatchLobbyUI: Unable to load widget class from path: %s"), *WidgetPath);
	}
}

void AGM_MatchingLobby::UpdateUIVisibility()
{
	if (MatchLobbyWidget)
	{
		APC_MatchingLobby* PC = Cast<APC_MatchingLobby>(GetWorld()->GetFirstPlayerController());
		UUserWidget* WarningExitWidget = Cast<UUserWidget>(PC->GetWidgetByName(MatchLobbyWidget, TEXT("WBP_Warning_ExitWidget")));
		UBackgroundBlur* BlurBackgroundWidget = Cast<UBackgroundBlur>(MatchLobbyWidget->GetWidgetFromName(FName(TEXT("BackgroundBlur"))));

		if (WarningExitWidget)
		{
			WarningExitWidget->SetVisibility(ESlateVisibility::Visible); // 또는 ESlateVisibility::Hidden
			UE_LOG(LogTemp, Log, TEXT("UpdateUIVisibility: WBP_Warning_ExitWidget visibility updated."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateUIVisibility: WBP_Warning_ExitWidget not found."));
		}

		if (BlurBackgroundWidget)
		{
			BlurBackgroundWidget->SetVisibility(ESlateVisibility::Visible); // 또는 ESlateVisibility::Hidden
			UE_LOG(LogTemp, Log, TEXT("UpdateUIVisibility: Blur_BackGround visibility updated."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateUIVisibility: Blur_BackGround not found."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateUIVisibility: MatchLobbyWidget is nullptr."));
	}
}

