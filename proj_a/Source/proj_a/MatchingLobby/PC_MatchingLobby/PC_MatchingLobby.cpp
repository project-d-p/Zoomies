#include "PC_MatchingLobby.h"

#include "CineCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "SteamSocketP2P.h"
#include "Components/BackgroundBlur.h"
#include "GameFramework/PlayerState.h"
#include "proj_a/MatchingLobby/GM_MatchingLobby/GM_MatchingLobby.h"
#include "proj_a/MatchingLobby/GS_MachingLobby/GS_MatchingLobby.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "proj_a/GameInstance/GI_Zoomies.h"
#include "proj_a/MatchingLobby/A_MatchingLobby/LevelComponents/LC_MatchLobby.h"

APC_MatchingLobby::APC_MatchingLobby()
{
	bIsReady = false;
	LevelComponent = CreateDefaultSubobject<ULC_MatchLobby>(TEXT("ULC_MatchLobby"));
}

void APC_MatchingLobby::ServerSetReady_Implementation(bool pIsReady)
{
	AGS_MatchingLobby* GS_MatchLobby= GetWorld()->GetGameState<AGS_MatchingLobby>();
	AGM_MatchingLobby* GM_MatchLobby = GetWorld()->GetAuthGameMode<AGM_MatchingLobby>();
	
	if (GM_MatchLobby && GS_MatchLobby)
	{
		int32 PlayerIndex = GM_MatchLobby->PCs.Find(this);
		GS_MatchLobby->SetPlayerReady(PlayerIndex, pIsReady);
	}
}

bool APC_MatchingLobby::ServerSetReady_Validate(bool pIsReady)
{
	return true;
}

void APC_MatchingLobby::ToggleReadyState()
{
	bIsReady = !bIsReady;
	ServerSetReady(bIsReady);
}

void APC_MatchingLobby::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (IsValid(GameInstance) && GameInstance->FriendsArray.Num() == 0)
	{
		GameInstance->LoadFriendsList();
	}
	SetCineCameraView();
	this->bAutoManageActiveCameraTarget = false;
	getMatchLobbyUI();
}

void APC_MatchingLobby::EndPlay( const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	RemoveMatchLobbyUI();
	DeactiveCurrentComponent();
}

void APC_MatchingLobby::SetCineCameraView()
{
	if (FixedCamera == nullptr)
	{
		FindCineCamera();
	}
	if (FixedCamera != nullptr)
	{
		this->SetViewTarget(FixedCamera);
	}
}

void APC_MatchingLobby::FindCineCamera()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACineCameraActor::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (Actor->GetName() == "CineCameraActor_0")
		{
			FixedCamera = Cast<ACineCameraActor>(Actor);
			break;
		}
	}
	if (FixedCamera == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CineCameraActor not found."));
		return;
	}
}

void APC_MatchingLobby::ActivateCurrentComponent(APC_MatchingLobby* LocalPlayerController)
{
	if (LevelComponent)
	{
		LevelComponent->PrimaryComponentTick.bCanEverTick = true;
		LevelComponent->Activate(true);
		LevelComponent->SetComponentTickEnabled(true);
		LevelComponent->RegisterComponent();
		if (LocalPlayerController)
		{
			LevelComponent->Set_PC(LocalPlayerController);
			if (!LocalPlayerController->IsLocalController())
			{
				UE_LOG(LogTemp, Warning, TEXT("PC_MatchingLobby::LocalPlayerController is not local controller"));
				return ;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PC_MatchingLobby::LocalPlayerController is nullptr"));
			return ;
		}

		if (UIC_MatchLobby* IC_Local = LevelComponent->GetInputComponent())
		{
			IC_Local->Activate(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PC_MatchingLobby::InputComponent is nullptr"));
		}
	}
}


void APC_MatchingLobby::DeactiveCurrentComponent()
{
	if (LevelComponent)
	{
		if (UIC_MatchLobby* IC_Local = LevelComponent->GetInputComponent())
		{
			IC_Local->Deactivate();
		}
		
		LevelComponent->Deactivate();
		LevelComponent->SetComponentTickEnabled(false);
		LevelComponent->PrimaryComponentTick.bCanEverTick = false;
		LevelComponent->RegisterComponent();
		LevelComponent = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PC_MatchingLobby::LevelComponent is nullptr"));
	}
}

void APC_MatchingLobby::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ActivateCurrentComponent(this);

	if (LevelComponent)
	{
		LevelComponent->Set_CHAR(InPawn);
		LevelComponent->GetInputComponent()->Set_CHAR(InPawn);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PC_MatchingLobby::LevelComponent is nullptr"));
	}
}

void APC_MatchingLobby::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	if (GetLocalRole() < ROLE_Authority)
	{
		ADPCharacter* DPCharacter = Cast<ADPCharacter>(P);
		if (!DPCharacter)
		{
			return ;
		}
		
		if (UWorld* World = GetWorld())
		{
			FString CurrentLevelName = World->GetMapName();
			DPCharacter->SetReplicatingMovement(true);
			ActivateCurrentComponent(this);
		}
	}
}


UUserWidget* APC_MatchingLobby::GetWidgetByName(UUserWidget* ParentWidget, const FString& WidgetName)
{
	if (ParentWidget)
	{
		UWidget* FoundWidget = Cast<UWidget>(ParentWidget->GetWidgetFromName(FName(*WidgetName)));
		return Cast<UUserWidget>(FoundWidget);
	}
	return nullptr;
}

void APC_MatchingLobby::getMatchLobbyUI()
{
	FString WidgetPath = TEXT("/Game/widget/WBP_MatchLobby/widget_match_ready.widget_match_ready_C");

	UClass* WidgetClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *WidgetPath));

	if (WidgetClass != nullptr)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController && Cast<APC_MatchingLobby>(PlayerController)->MatchLobbyWidget == nullptr)
		{
			MatchLobbyWidget = CreateWidget<UUserWidget>(PlayerController, WidgetClass);

			if (MatchLobbyWidget != nullptr)
			{
				MatchLobbyWidget->AddToViewport();
				FInputModeGameAndUI InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				InputMode.SetWidgetToFocus(MatchLobbyWidget->TakeWidget());
				SetInputMode(InputMode);
				UE_LOG(LogTemp, Log, TEXT("APC_MatchingLobby::getMatchLobbyUI: Widget successfully created and added to viewport."));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("APC_MatchingLobby::getMatchLobbyUI: MatchLobbyWidget is nullptr after creation."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("APC_MatchingLobby::getMatchLobbyUI: PlayerController is nullptr."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APC_MatchingLobby::getMatchLobbyUI: Unable to load widget class from path: %s"), *WidgetPath);
	}
}

void APC_MatchingLobby::UpdateUIVisibility() const
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

void APC_MatchingLobby::ShowUI_ESC() const
{
	if (MatchLobbyWidget)
	{
		APC_MatchingLobby* PC = Cast<APC_MatchingLobby>(GetWorld()->GetFirstPlayerController());
		UUserWidget* Widget_ESC = Cast<UUserWidget>(PC->GetWidgetByName(MatchLobbyWidget, TEXT("WBP_Esc_Menu")));

		if (Widget_ESC)
		{
			ESlateVisibility CurrentVisibility = Widget_ESC->GetVisibility();

			if (CurrentVisibility == ESlateVisibility::Visible)
			{
				Widget_ESC->SetVisibility(ESlateVisibility::Hidden);
				UE_LOG(LogTemp, Log, TEXT("UpdateUIVisibility: WBP_Esc_Menu visibility set to Hidden."));
			}
			else
			{
				Widget_ESC->SetVisibility(ESlateVisibility::Visible);
				UE_LOG(LogTemp, Log, TEXT("UpdateUIVisibility: WBP_Esc_Menu visibility set to Visible."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateUIVisibility: WBP_Esc_Menu not found."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateUIVisibility: MatchLobbyWidget is nullptr."));
	}
}

void APC_MatchingLobby::RemoveMatchLobbyUI()
{
	if (MatchLobbyWidget != nullptr)
	{
		MatchLobbyWidget->RemoveFromParent();
		MatchLobbyWidget = nullptr;
		UE_LOG(LogTemp, Log, TEXT("APC_MatchingLobby::RemoveMatchLobbyUI: Widget successfully removed from viewport and cleared."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APC_MatchingLobby::RemoveMatchLobbyUI: MatchLobbyWidget is nullptr, nothing to remove."));
	}
}