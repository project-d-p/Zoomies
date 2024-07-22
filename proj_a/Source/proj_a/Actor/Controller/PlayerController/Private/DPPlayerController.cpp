// Fill out your copyright notice in the Description page of Project Settings.

#include "DPPlayerController.h"

#include "DPCharacter.h"
#include "DPGameModeBase.h"
#include "DPPlayerState.h"
#include "FNetLogger.h"
#include "GameHelper.h"
#include "BaseInputComponent.h"
#include "MainLevelComponent.h"
#include "ResultLevelComponent.h"
#include "GameFramework/GameMode.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

DEFINE_LOG_CATEGORY(LogNetwork);

ADPPlayerController::ADPPlayerController()
{
	FNetLogger::LogError(TEXT("CREATED ADPPlayerController: %d"), this->GetUniqueID());
	FNetLogger::EditerLog(FColor::Red, TEXT("CREATED ADPPlayerController: %d"), this->GetUniqueID());
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
	Socket = CreateDefaultSubobject<UClientSocket>(TEXT("MySocket"));

	UBaseLevelComponent* MainLevelComponet = CreateDefaultSubobject<UMainLevelComponent>(TEXT("MainLevelComponent"));
	UBaseLevelComponent* ResultLevelComponet = CreateDefaultSubobject<UResultLevelComponent>(TEXT("ResultLevelComponent"));

	MainLevelComponet->InitializeController(this);
	ResultLevelComponet->InitializeController(this);
	
	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::MAIN), MainLevelComponet);
	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::RESULT), ResultLevelComponet);
	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::NONE), nullptr);

	// TODO: Change to private score manager later
	PrivateScoreManager = CreateDefaultSubobject<UPrivateScoreManager>(TEXT("PrivateScoreManager"));
}

ADPPlayerController::~ADPPlayerController()
{
	FNetLogger::EditerLog(FColor::Red, TEXT("ADPPlayerController::~ADPPlayerController"));
	FNetLogger::LogError(TEXT("PlayerController ID[~Destructor]: %d"), this->GetUniqueID());
}

void ADPPlayerController::SendChatMessageToServer(const FString& Message)
{
	if (ChatManager == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChatManager is null"));
		return;
	}

	FString SenderName = PlayerState->GetPlayerName();
	ChatManager->ServerSendChatMessage(SenderName, Message);
}

void ADPPlayerController::ReceiveChatMessage(const FString& SenderName, const FString& Message)
{
	if (ChatManager)
	{
		ChatManager->ClientReceiveChatMessage(SenderName, Message);
	}
}

void ADPPlayerController::InitChatManager(UChatUI* ChatUI)
{
	ChatManager->setChatUI(ChatUI);
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

UClientSocket* ADPPlayerController::GetClientSocket() const
{
	return Socket;
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
			if (CurrentLevelName.Contains("resultLevel"))
			{
				DPCharacter->SetReplicatingMovement(true);
				SwitchLevelComponent(ELevelComponentType::RESULT);
			}
		}
	}
}

void ADPPlayerController::ReleaseMemory()
{
	if (Socket)
	{
		Socket->DestoryInstance();
		Socket->DestroyComponent();
		Socket = nullptr;
	}
	if (ChatManager)
	{
		ChatManager->DestroyComponent();
		ChatManager = nullptr;
	}
}

void ADPPlayerController::ClientDestroySession_Implementation()
{
	// 게임 인스턴스를 통해 세션 제거
	FNetLogger::LogError(TEXT("ClientDestroySession_Implementation Before"));
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		IOnlineSessionPtr SessionInt = GameInstance->GetOnlineSessionInterface();
		if (SessionInt.IsValid())
		{
			FNetLogger::LogError(TEXT("ClientDestroySession_Implementation After"));
			SessionInt->DestroySession(NAME_GameSession);
		}
	}
}

void ADPPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	Cast<UMainLevelComponent>(LevelComponents[static_cast<uint32>(ELevelComponentType::MAIN)])->SetStateComponent();
}

void ADPPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ADPPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (Socket)
	{
		Socket->DestoryInstance();
		Socket->DestroyComponent();
		Socket = nullptr;
	}
	if (ChatManager)
	{
		ChatManager->DestroyComponent();
		ChatManager = nullptr;
	}
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

void ADPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ADPPlayerController::SwitchLevelComponent(ELevelComponentType Type)
{
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
void ADPPlayerController::GetSeamlessTravelActorList(bool bToTransitionMap, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToTransitionMap, ActorList);

	if (IsLocalPlayerController())
	{
		UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
		if (!GameInstance)
		{
			return ;
		}
		GameInstance->LocalController = this;
	}
	this->SwitchLevelComponent(ELevelComponentType::NONE);
}
