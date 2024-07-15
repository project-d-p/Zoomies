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

DEFINE_LOG_CATEGORY(LogNetwork);

ADPPlayerController::ADPPlayerController()
{
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
	Socket = CreateDefaultSubobject<UClientSocket>(TEXT("MySocket"));

	UBaseLevelComponent* MainLevelComponet = CreateDefaultSubobject<UMainLevelComponent>(TEXT("MainLevelComponent"));
	UBaseLevelComponent* ResultLevelComponet = CreateDefaultSubobject<UResultLevelComponent>(TEXT("ResultLevelComponent"));
	MainLevelComponet->InitializeController(this);
	ResultLevelComponet->InitializeController(this);

	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::MAIN), MainLevelComponet);
	LevelComponents.Add(static_cast<uint32>(ELevelComponentType::RESULT), ResultLevelComponet);
	
	// TODO: Change to private score manager later
	PrivateScoreManager = CreateDefaultSubobject<UPrivateScoreManager>(TEXT("PrivateScoreManager"));
}

void ADPPlayerController::SendChatMessageToServer(const FString& Message)
{
	if (ChatManager == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChatManager is null"));
		return;
	}

	// XXX: Change to client Steam nickname later
	FString SenderName = "Unknown";
	if (HasAuthority())
	{
		ADPGameModeBase* GM = UGameHelper::GetInGameMode(GetWorld());
		if (GM)
		{
			GM->SendChatToAllClients(SenderName, Message);
		}
	}
	else
	{
		ChatManager->ServerSendChatMessage(SenderName, Message);
	}
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

void ADPPlayerController::ReleaseMemory()
{
	if (Socket)
	{
		Socket->DestoryInstance();
		Socket->DestroyComponent();
	}
	if (ChatManager)
	{
		ChatManager->DestroyComponent();
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
}

void ADPPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Set StateComponet in MainLevel
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
		FNetLogger::EditerLog(FColor::Cyan, TEXT("ActiveComponent is null"));
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
		ActiveComponent->Deactivate();
		ActiveComponent->SetComponentTickEnabled(false);
        
		// 입력 컴포넌트 비활성화
		if (UBaseInputComponent* InputComp = ActiveComponent->GetInputComponent())
		{
			InputComp->Deactivate();
		}
		ActiveComponent = nullptr;
	}
}

void ADPPlayerController::ActivateComponent(ELevelComponentType Type)
{
	UBaseLevelComponent* SelectedComponent = LevelComponents[static_cast<uint32>(Type)];
	if (SelectedComponent)
	{
		SelectedComponent->Activate(true);
		SelectedComponent->SetComponentTickEnabled(true);
		
		// 입력 컴포넌트 활성화
		if (UBaseInputComponent* InputComp = SelectedComponent->GetInputComponent())
		{
			FNetLogger::EditerLog(FColor::Cyan, TEXT("Activate InputComponent"));
			InputComp->Activate(true);
		}
		ActiveComponent = SelectedComponent;
	}
}
