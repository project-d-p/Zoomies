// Fill out your copyright notice in the Description page of Project Settings.

#include "DPPlayerController.h"

#include "DPCharacter.h"
#include "DPPlayerState.h"
#include "FNetLogger.h"
#include "BaseInputComponent.h"
#include "ClientNetworkManager.h"
#include "JudgeGameMode.h"
#include "MainLevelComponent.h"
#include "ResultLevelComponent.h"
#include "CompileMode.h"
#include "DPInGameState.h"
#include "EngineUtils.h"
#include "JudgeLevelComponent.h"
#include "LobbyLevelComponent.h"
#include "Chaos/ChaosPerfTest.h"
#include "Kismet/GameplayStatics.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

DEFINE_LOG_CATEGORY(LogNetwork);

ADPPlayerController::ADPPlayerController()
{
#if EDITOR_MODE
	NetworkManager = CreateDefaultSubobject<UANetworkManager>(TEXT("NetworkManager"));
#else
	NetworkManager = CreateDefaultSubobject<UClientNetworkManager>(TEXT("NetworkManager"));
#endif
	
	TextureTransferManager = CreateDefaultSubobject<UTextureTransferManager>(TEXT("DataTransferManager"));
	TextureTransferManager->OnDataTransferComplete.BindDynamic(TextureTransferManager, &UTextureTransferManager::OnTextureTransferComplete);
	PrivateScoreManager = CreateDefaultSubobject<UPrivateScoreManager>(TEXT("PrivateScoreManager"));
	
	UBaseLevelComponent* MainLevelComponet = CreateDefaultSubobject<UMainLevelComponent>(TEXT("MainLevelComponent"));
	// UBaseLevelComponent* JudgeLevelComponent = CreateDefaultSubobject<UJudgeLevelComponent>(TEXT("JudgeLevelComponent"));
	UBaseLevelComponent* ResultLevelComponet = CreateDefaultSubobject<UResultLevelComponent>(TEXT("ResultLevelComponent"));
	UBaseLevelComponent* LobbyLevelComponent = CreateDefaultSubobject<ULobbyLevelComponent>(TEXT("LobbyLevelComponent"));

	MainLevelComponet->InitializeController(this);
	ResultLevelComponet->InitializeController(this);
	LobbyLevelComponent->InitializeController(this);
	// JudgeLevelComponent->InitializeController(this);
	
	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::MAIN), MainLevelComponet);
	// LevelComponents.Add(static_cast<uint32>(ELevelComponentType::JUDGE), JudgeLevelComponent);
	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::RESULT), ResultLevelComponet);
	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::LOBBY), LobbyLevelComponent);
	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::NONE), nullptr);

	LevelEnumMap.Add(TEXT("mainLevel"), static_cast<uint32>(ELevelComponentType::MAIN));
	LevelEnumMap.Add(TEXT("lobbyLevel"), static_cast<uint32>(ELevelComponentType::LOBBY));
	LevelEnumMap.Add(TEXT("calculateLevel"), static_cast<uint32>(ELevelComponentType::RESULT));
}

ADPPlayerController::~ADPPlayerController()
{
}

UPlayerScoreComp* ADPPlayerController::GetScoreManagerComponent() const
{
	if (PlayerState == nullptr)
	{
		return nullptr;
	}
	return Cast<ADPPlayerState>(PlayerState)->GetPlayerScoreComp();
}

UPrivateScoreManager* ADPPlayerController::GetPrivateScoreManagerComponent() const
{
	return PrivateScoreManager;
}

UANetworkManager* ADPPlayerController::GetNetworkManager() const
{
	return NetworkManager;
}

void ADPPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	// 클라이언트 측에서 추가 초기화 작업 수행
	if (GetLocalRole() < ROLE_Authority)
	{
		ADPCharacter* DPCharacter = Cast<ADPCharacter>(P);
		if (!DPCharacter)
		{
			return ;
		}
		
		// disable move replication : set bReplicateMovement to false
		if (UWorld* World = GetWorld())
		{
			FString CurrentLevelName = World->GetMapName();
			if (CurrentLevelName.Contains("calculateLevel"))
			{
				DPCharacter->SetReplicatingMovement(true);
				SwitchLevelComponent(ELevelComponentType::RESULT);
			}
		}
	}
}

void ADPPlayerController::ServerSendChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	IChatGameMode* GM = GetWorld()->GetAuthGameMode<IChatGameMode>();
	check(GM && GM->GetChatManager())
	GM->GetChatManager()->BroadcastChatMessage(SenderName, Message);
}

void ADPPlayerController::ReleaseMemory()
{
	NetworkManager->Shutdown();
}

void ADPPlayerController::ClientDestroySession_Implementation()
{
	// 게임 인스턴스를 통해 세션 제거
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		IOnlineSessionPtr SessionInt = GameInstance->GetOnlineSessionInterface();
		if (SessionInt.IsValid())
		{
			SessionInt->DestroySession(GameInstance->SessionName);
		}
	}
}

void ADPPlayerController::ConnectToServer_Implementation(ELevelComponentType Type)
{
	if (IsLocalController())
	{
		
#if EDITOR_MODE
	NetworkManager->Initialize(ENetworkTypeZoomies::NONE);
#elif LAN_MODE
	// NetworkManager->Initialize(ENetworkTypeZoomies::SOCKET_STEAM_LAN);
	NetworkManager->Initialize(ENetworkTypeZoomies::ENGINE_SOCKET);
#else
	NetworkManager->Initialize(ENetworkTypeZoomies::SOCKET_STEAM_P2P);
#endif
	}
}

void ADPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	Cast<UMainLevelComponent>(LevelComponents[static_cast<uint32>(ELevelComponentType::MAIN)])->SetStateComponent();
	SetLevelComponent();
}

void ADPPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ADPPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	NetworkManager->Shutdown();
}

void ADPPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Set StateComponet in MainLevel
	for (auto& Pair : LevelComponents)
	{
		if (Pair.Value)
		{
			Pair.Value->SetPlayerCharacter(InPawn);
			Pair.Value->GetInputComponent()->SetPlayerCharacter(InPawn);
		}
	}
	Cast<UMainLevelComponent>(LevelComponents[static_cast<uint32>(ELevelComponentType::MAIN)])->SetStateComponent();
}

void ADPPlayerController::SetLevelComponent()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ELevelComponentType LevelType = LevelEnumMap.Find(World->GetMapName()) ? static_cast<ELevelComponentType>(*(LevelEnumMap.Find(World->GetMapName()))) : ELevelComponentType::NONE;
		SwitchLevelComponent(LevelType);
	}
}

void ADPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ADPPlayerController::SwitchLevelComponent(ELevelComponentType Type)
{
	if (ActiveComponent == LevelComponents[static_cast<uint32>(Type)])
	{
		return ;
	}
	if (ActiveComponent)
	{
		DeactiveCurrentComponent();
		ActivateComponent(Type);
	}
	else
	{
		ActivateComponent(Type);
	}
}

UBaseLevelComponent* ADPPlayerController::GetLevelComponent() const
{
	return ActiveComponent;
}

UBaseLevelComponent* ADPPlayerController::GetLevelComponent(ELevelComponentType Type) const
{
	return LevelComponents[static_cast<uint32>(Type)];
}

void ADPPlayerController::DeactiveCurrentComponent()
{
	if (ActiveComponent)
	{
		// 입력 컴포넌트 비활성화
		if (UBaseInputComponent* InputComp = ActiveComponent->GetInputComponent())
		{
			InputComp->Deactivate();
		}
		
		ActiveComponent->Deactivate();
		ActiveComponent->SetComponentTickEnabled(false);
		ActiveComponent->PrimaryComponentTick.bCanEverTick = false;
		ActiveComponent->RegisterComponent();
		
		ActiveComponent = nullptr;
	}
}

void ADPPlayerController::ActivateComponent(ELevelComponentType Type)
{
	UBaseLevelComponent* SelectedComponent = LevelComponents[static_cast<uint32>(Type)];
	if (SelectedComponent)
	{
		SelectedComponent->PrimaryComponentTick.bCanEverTick = true;
		SelectedComponent->Activate(true);
		SelectedComponent->SetComponentTickEnabled(true);
		SelectedComponent->RegisterComponent();
		
		// 입력 컴포넌트 활성화
		if (UBaseInputComponent* InputComp = SelectedComponent->GetInputComponent())
		{
			InputComp->Activate(true);
		}
		ActiveComponent = SelectedComponent;
	}
}

// Local PlayerController && Local PlayerState (Server && Client) Automatically Saved && Called Right Before Seamless Travel
// this is called for both parts of the transition because actors might change while in the middle (e.g. players might join or leave the game)
void ADPPlayerController::GetSeamlessTravelActorList(bool bToTransitionMap, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransitionMap, ActorList);

	if (!GetWorld()->GetMapName().Contains("mainLevel"))
	{
		return ;
	}
	if (IsLocalPlayerController())
	{
		UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
		if (!GameInstance)
		{
			return ;
		}
		NetworkManager->Shutdown();
	}

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	check(GameInstance)
	GameInstance->network_failure_manager_->TryReset();
	UDataManager* DataManager = GameInstance->network_failure_manager_->GetDataManager();
	check(DataManager)
	DataManager->ClearSeamlessDataArray();
	ADPInGameState* DPInGameState = GetWorld()->GetGameState<ADPInGameState>();
	check(DPInGameState)

	for (auto PlayerState_ : DPInGameState->PlayerArray)
	{
		ADPPlayerState* DPPlayerState = Cast<ADPPlayerState>(PlayerState_);
		if (DPPlayerState)
		{
			UPlayerScoreData* NewData = NewObject<UPlayerScoreData>(DataManager);
			if (NewData)
			{
				UPlayerScoreData* PlayerScoreData = DPPlayerState->GetPlayerScoreData();
				NewData->InitializeData();
				NewData->SetPlayerName(PlayerScoreData->GetPlayerName());
				NewData->SetPlayerId(PlayerScoreData->GetPlayerId());
				NewData->SetPlayerJob(PlayerScoreData->GetPlayerJob());
				NewData->SetScore(PlayerScoreData->GetScore());
				DataManager->AddSeamlessDataToArray(TEXT("PlayerScoreSeamless"), NewData);
			}
		}
	}
	this->SwitchLevelComponent(ELevelComponentType::NONE);
}

void ADPPlayerController::getUIWidget()
{
	if (UWorld* World = GetWorld())
	{
		FString CurrentLevelName = World->GetMapName();
		if (CurrentLevelName.Contains("main"))
		{
			UMainLevelComponent* ML = Cast<UMainLevelComponent>(LevelComponents[static_cast<uint32>(ELevelComponentType::MAIN)]);
			UIWidget = ML->GetInGameWidget();
		}
	}
	if (UIWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("DPPlayerController::getUIWidget: Widget is nullptr"));
	}
}

void ADPPlayerController::RemoveUIWidget()
{
	if (UIWidget != nullptr)
	{
		UIWidget->RemoveFromParent();
		UIWidget = nullptr;
		UE_LOG(LogTemp, Log, TEXT("DPPlayerController::RemoveMatchLobbyUI: Widget successfully removed from viewport and cleared."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DPPlayerController::RemoveMatchLobbyUI: MatchLobbyWidget is nullptr, nothing to remove."));
	}
}

void ADPPlayerController::ShowUI_ESC()
{
	if (UIWidget == nullptr)
	{
		getUIWidget();
	}
	if (UIWidget)
	{
		UUserWidget* WidgetESC = Cast<UUserWidget>(UIWidget->GetWidgetFromName("WBP_Esc_Menu"));
		if (WidgetESC)
		{
			ESlateVisibility CurrentVisibility = WidgetESC->GetVisibility();
			if (CurrentVisibility == ESlateVisibility::Visible)
			{
				WidgetESC->SetVisibility(ESlateVisibility::Hidden);
				bShowMouseCursor = false;
				SetInputMode(FInputModeGameOnly());
			}
			else
			{
				WidgetESC->SetVisibility(ESlateVisibility::Visible);
				bShowMouseCursor = true;
				SetInputMode(FInputModeGameAndUI());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DPPlayerController:: UIWidget->GetWidgetFromName(ESC) is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DPPlayerController:: UIWidget is nullptr"));
	}
}
