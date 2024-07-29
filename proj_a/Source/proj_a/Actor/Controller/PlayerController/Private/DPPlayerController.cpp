// Fill out your copyright notice in the Description page of Project Settings.

#include "DPPlayerController.h"

#include "DPCharacter.h"
#include "DPPlayerState.h"
#include "FNetLogger.h"
#include "BaseInputComponent.h"
#include "ClientNetworkManager.h"
#include "JudgeGameMode.h"
#include "JudgeLevelComponent.h"
#include "MainLevelComponent.h"
#include "ResultLevelComponent.h"
#include "proj_a/GameInstance/GI_Zoomies.h"

DEFINE_LOG_CATEGORY(LogNetwork);

ADPPlayerController::ADPPlayerController()
{
	/*
	 * TEST: COMMENT
	Socket = CreateDefaultSubobject<UClientSocket>(TEXT("MySocket"));
	*/
	
	/// TEST
	NetworkManager = CreateDefaultSubobject<UClientNetworkManager>(TEXT("NetworkManager"));
	///

	PrivateScoreManager = CreateDefaultSubobject<UPrivateScoreManager>(TEXT("PrivateScoreManager"));
	
	UBaseLevelComponent* MainLevelComponet = CreateDefaultSubobject<UMainLevelComponent>(TEXT("MainLevelComponent"));
	UBaseLevelComponent* ResultLevelComponet = CreateDefaultSubobject<UResultLevelComponent>(TEXT("ResultLevelComponent"));

	MainLevelComponet->InitializeController(this);
	ResultLevelComponet->InitializeController(this);
	
	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::MAIN), MainLevelComponet);
	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::RESULT), ResultLevelComponet);
	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::NONE), nullptr);
}

ADPPlayerController::~ADPPlayerController()
{
	FNetLogger::EditerLog(FColor::Red, TEXT("ADPPlayerController::~ADPPlayerController"));
	FNetLogger::LogError(TEXT("PlayerController ID[~Destructor]: %d"), this->GetUniqueID());
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

/// TEST
UANetworkManager* ADPPlayerController::GetNetworkManager() const
{
	return NetworkManager;
}
///

/*
 * TEST: COMMENT
UClientSocket* ADPPlayerController::GetClientSocket() const
{
	return Socket;
}
*/

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

void ADPPlayerController::ServerSendChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	IChatGameMode* GM = GetWorld()->GetAuthGameMode<IChatGameMode>();
	check(GM && GM->GetChatManager())
	GM->GetChatManager()->BroadcastChatMessage(SenderName, Message);
}

void ADPPlayerController::ReleaseMemory()
{
	/*
	 * TEST: COMMENT
	if (Socket)
	{
		Socket->DestoryInstance();
		Socket->DestroyComponent();
		Socket = nullptr;
	}
	*/

	/// TEST
	NetworkManager->Shutdown();
	///
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

void ADPPlayerController::ConnectToServer_Implementation(ELevelComponentType Type)
{
	FNetLogger::LogError(TEXT("Am I Here?"));
	NetworkManager->Initialize(ENetworkTypeZoomies::SOCKET_STEAM_LAN);
	SwitchLevelComponent(Type);
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

	/*
	 * TEST: COMMENT
	if (Socket)
	{
		Socket->DestoryInstance();
		Socket->DestroyComponent();
		Socket = nullptr;
	}
	 */

	/// TEST
	NetworkManager->Shutdown();
	/// TEST
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
		NetworkManager->Shutdown();
	}
	this->SwitchLevelComponent(ELevelComponentType::NONE);
}
