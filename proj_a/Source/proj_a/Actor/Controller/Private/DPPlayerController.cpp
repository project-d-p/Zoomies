// Fill out your copyright notice in the Description page of Project Settings.

#include "DPPlayerController.h"
#include "DPCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "DPWeaponActorComponent.h"
#include "DPConstructionActorComponent.h"
#include "DPGameModeBase.h"
#include "DPPlayerState.h"
#include "DPStateActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProtobufUtility.h"
#include "movement.pb.h"
#include "FDataHub.h"
#include "FNetLogger.h"
#include "FUdpSendTask.h"
#include "MessageMaker.h"
#include "GameHelper.h"
#include "Kismet/GameplayStatics.h"


DEFINE_LOG_CATEGORY(LogNetwork);

ADPPlayerController::ADPPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>DEFAULT_CONTEXT
	(TEXT("/Game/input/imc_character.imc_character"));
	if (DEFAULT_CONTEXT.Succeeded())
		defaultContext = DEFAULT_CONTEXT.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_MOVE
	(TEXT("/Game/input/ia_move.ia_move"));
	if (IA_MOVE.Succeeded())
		moveAction = IA_MOVE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_JUMP
	(TEXT("/Game/input/ia_jump.ia_jump"));
	if (IA_JUMP.Succeeded())
		jumpAction = IA_JUMP.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ROTATE
	(TEXT("/Game/input/ia_rotate.ia_rotate"));
	if (IA_ROTATE.Succeeded())
		rotateAction = IA_ROTATE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ACTIVE
	(TEXT("/Game/input/ia_active.ia_active"));
	if (IA_ACTIVE.Succeeded())
		activeAction = IA_ACTIVE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ADDITIONALSETTING
	(TEXT("/Game/input/ia_additionalSetting.ia_additionalSetting"));
	if (IA_ADDITIONALSETTING.Succeeded())
		additionalSettingAction = IA_ADDITIONALSETTING.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_AIM
	(TEXT("/Game/input/ia_aim.ia_aim"));
	if (IA_AIM.Succeeded())
		aimAction = IA_AIM.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_CANCEL
	(TEXT("/Game/input/ia_cancel.ia_cancel"));
	if (IA_CANCEL.Succeeded())
		cancelAction = IA_CANCEL.Object;
	
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
	Socket = CreateDefaultSubobject<UMySocket>(TEXT("MySocket"));

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset
	(TEXT("/Game/sounds/effect/character/jump_Cue.jump_Cue"));
	if (SoundAsset.Succeeded()) {
		jumpSound = SoundAsset.Object;
	}
	else
		jumpSound = nullptr;

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

void ADPPlayerController::CreateSocket()
{
	this->Socket->CreateSocket();
}

void ADPPlayerController::Connect(FString ip, uint32 port)
{
	this->Socket->Connect(ip, port);
}

void ADPPlayerController::RunTask()
{
	this->Socket->RunTask();
}


void ADPPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	character = Cast<ADPCharacter>(GetPawn());
	if (character)
	{
		state = Cast<UDPStateActorComponent>(character->GetComponentByClass(UDPStateActorComponent::StaticClass()));
		construction = Cast<UDPConstructionActorComponent>(character->GetComponentByClass(UDPConstructionActorComponent::StaticClass()));
	}
	else
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Character is null in BeginPlay"));
	}
	
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Add Mapping Context [Begin Play]"));
		SubSystem->AddMappingContext(defaultContext, 0);
	}
	
	GetWorldTimerManager().SetTimer(MovementTimerHandle, this, &ADPPlayerController::SendCompressedMovement, 0.01f, true);
	if (!HasAuthority())
	{
		GetWorldTimerManager().SetTimer(SynchronizeHandle, this, &ADPPlayerController::UpdatePlayer, 5.00f, true);
	}
}


void ADPPlayerController::SendCompressedMovement()
{
	if (HasAuthority())
	{
		return ;
	}
	if (AccumulatedMovementInput.IsNearlyZero())
	{
		return ;
	}

	if (PlayerState)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Player Name: %s"), *PlayerState->GetPlayerName());
	}
	
	FNetLogger::EditerLog(FColor::Blue, TEXT("Send Movement[Client]: %f %f"), AccumulatedMovementInput.X, AccumulatedMovementInput.Y);
	FNetLogger::EditerLog(FColor::Blue, TEXT("Send Forward[Client]: %f %f %f"), AccumulatedForwardInput.X, AccumulatedForwardInput.Y, AccumulatedForwardInput.Z);
	FNetLogger::EditerLog(FColor::Blue, TEXT("Send Right[Client]: %f %f %f"), AccumulatedRightInput.X, AccumulatedRightInput.Y, AccumulatedRightInput.Z);
	// Send Server
	// Message message = MessageMaker::MakeMessage(this, AccumulatedMovementInput, AccumulatedForwardInput, AccumulatedRightInput);
	// Socket->SendPacket(message);
	AccumulatedMovementInput = FVector2D::ZeroVector;
	AccumulatedForwardInput = FVector::ZeroVector;
	AccumulatedRightInput = FVector::ZeroVector;
}

void ADPPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ADPPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	FNetLogger::EditerLog(FColor::Red, TEXT("OnPossess"));
	
	character = Cast<ADPCharacter>(GetPawn());

	if (!character)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Character is null in OnPossess"));
		UE_LOG(LogTemp, Warning, TEXT("character null"));
		return;
	}
	
	state = Cast<UDPStateActorComponent>(character->GetComponentByClass(UDPStateActorComponent::StaticClass()));
	construction = Cast<UDPConstructionActorComponent>(character->GetComponentByClass(UDPConstructionActorComponent::StaticClass()));

	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Add Mapping Context [On Possess]"));
		SubSystem->AddMappingContext(defaultContext, 0);
	}
}

void ADPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// enhanced input component 캐스팅하고 바인딩
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
		// 플레이어 이동 ( w, a, d, s )
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Move);
		// 플레이어 점프 ( space )
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Jump);
		// 시점 변환 ( 마우스 회전 )
		EnhancedInputComponent->BindAction(rotateAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Rotate);
		//	행동, 총 발사/벽 설치/터렛 설치 ( 마우스 좌클릭 )
		EnhancedInputComponent->BindAction(activeAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Active);
		//	변경, 무기 변경/벽 회전 ( 마우스 스크롤 )
		EnhancedInputComponent->BindAction(additionalSettingAction, ETriggerEvent::Triggered, this, &ADPPlayerController::AdditionalSetting);
		//	에임 ( 마우스 우클릭 )
		EnhancedInputComponent->BindAction(aimAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Aim);	// 	key down
		EnhancedInputComponent->BindAction(aimAction, ETriggerEvent::Completed, this, &ADPPlayerController::AimReleased);
		//	취소, 채팅 끄기 ( esc - UE 에디터에서 기본 단축키 변경 필요 )
		EnhancedInputComponent->BindAction(cancelAction, ETriggerEvent::Triggered, this, &ADPPlayerController::ActionCancel);
	}
}

void ADPPlayerController::Move(const FInputActionValue& value)
{
	// UE_LOG(LogTemp, Warning, TEXT("ia_move_x : %f"), value.Get<FVector2D>().X);
	// UE_LOG(LogTemp, Warning, TEXT("ia_move_y : %f"), value.Get<FVector2D>().Y);
	const FVector2D actionValue = value.Get<FVector2D>();
	const FRotator controlRotation = GetControlRotation();
	// const FRotator yaw(0.f, controlRotation.Yaw, 0.f);

	const FVector forwardVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::X);
	const FVector rightVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::Y);
	if (HasAuthority())
	{
		// 해당 부분에서 서버로 이동 명령을 보내야 하나?
		FNetLogger::EditerLog(FColor::Blue, TEXT("Send Movement[Server]: %f %f"), actionValue.X, actionValue.Y);
	}
	else
	{
		AccumulatedForwardInput = forwardVector;
		AccumulatedRightInput = rightVector;
		AccumulatedMovementInput += actionValue;
		// 클라이언트면 보내야함
		// FNetLogger::EditerLog(FColor::Blue, TEXT("Send Movement[Client]: %f %f"), actionValue.X, actionValue.Y);
	}

	FNetLogger::EditerLog(FColor::Cyan, TEXT("actionValue: %f %f"), actionValue.X, actionValue.Y);
	FNetLogger::EditerLog(FColor::Cyan, TEXT("forwardVector: %f %f %f"), forwardVector.X, forwardVector.Y, forwardVector.Z);
	FNetLogger::EditerLog(FColor::Cyan, TEXT("rightVector: %f %f %f"), rightVector.X, rightVector.Y, rightVector.Z);

	MovementCount++;
	FNetLogger::EditerLog(FColor::Red, TEXT("MovementCount: %d"), MovementCount);

	FVector Velocity = character->GetCharacterMovement()->Velocity;
	Message message = MessageMaker::MakeMessage(this, actionValue, forwardVector, rightVector, Velocity);
	Socket->SendPacket(message);
	
	character->AddMovementInput(forwardVector, actionValue.X);
	character->AddMovementInput(rightVector, actionValue.Y);

	FNetLogger::EditerLog(FColor::Emerald, TEXT("Character Velocity Size: %f"), character->GetCharacterMovement()->Velocity.Size());
	FNetLogger::EditerLog(FColor::Emerald, TEXT("Character Velocity: %f %f %f"), character->GetCharacterMovement()->Velocity.X, character->GetCharacterMovement()->Velocity.Y, character->GetCharacterMovement()->Velocity.Z);
	
	// character->speed = character->GetCharacterMovement()->Velocity.Size();
}

void ADPPlayerController::Jump(const FInputActionValue& value)
{
	//UE_LOG(LogTemp, Warning, TEXT("ia_jump : %d"), value.Get<bool>());

	bool actionValue = value.Get<bool>();
	if (actionValue) {
		// send jump command ( id, actionValue ) ( true = jump )
		character->Jump();
		UGameplayStatics::PlaySound2D(GetWorld(), jumpSound);
	}
}

void ADPPlayerController::Rotate(const FInputActionValue& value)
{
	// UE_LOG(LogTemp, Warning, TEXT("ia_rotate_x : %f"), value.Get<FVector2D>().X);
	// UE_LOG(LogTemp, Warning, TEXT("ia_rotate_y : %f"), value.Get<FVector2D>().Y);

	FVector2D actionValue = value.Get<FVector2D>();

	// send rotate command ( id, actionValue )
	character->AddControllerYawInput(actionValue.X);
	character->AddControllerPitchInput(actionValue.Y);
	FUdpSendTask::ProtoData.set_allocated_progess_vector(ProtobufUtility::ConvertToFVecToVec3(character->GetControlRotation().Vector()));
}

void ADPPlayerController::Active(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Active"));

	if ("NONE" == state->equipmentState) {

	}
	if ("GUN" == state->equipmentState) {
		if (character->isAim) {
			character->PlayFireAnimation();
			character->weaponComponent->Attack();
		}
	}
	// if ("WALL" == state->equipmentState) {
	// 	//if (character->isAim) {
	// 		character->GetCharacterMovement()->DisableMovement();
	// 		construction->MakeWall({ 0, 0, 0 }, { 0, 0, 0 });
	// 		character->constructionComponent->placeWall = true;
	// 		// 다음 틱에 false로 바꿈
	// 		auto resetPlaceWall = [this]() {
	// 			character->constructionComponent->placeWall = false;
	// 		};
	// 		GetWorld()->GetTimerManager().SetTimerForNextTick(resetPlaceWall);
	//
	// 		FTimerHandle waitTimer;
	// 		GetWorld()->GetTimerManager().SetTimer(waitTimer, FTimerDelegate::CreateLambda([&]() {
	// 			UE_LOG(LogTemp, Warning, TEXT("wall delay 1.63"));
	// 			character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);	// movement disable -> enable
	// 			GetWorldTimerManager().ClearTimer(waitTimer);
	// 		}), 1.63f, false);
	// 	//}
	// }
	// if ("TURRET" == state->equipmentState) {
	// 	if (character->isAim) {
	// 		character->GetCharacterMovement()->DisableMovement();
	// 		character->constructionComponent->placeturret = true;
	//
	// 		auto resetPlaceTurret = [this]() {
	// 			character->constructionComponent->placeturret = false;
	// 		};
	// 		GetWorld()->GetTimerManager().SetTimerForNextTick(resetPlaceTurret);
	//
	// 		FTimerHandle waitTimer;
	// 		GetWorld()->GetTimerManager().SetTimer(waitTimer, FTimerDelegate::CreateLambda([&]() {
	// 			UE_LOG(LogTemp, Warning, TEXT("turret delay 1.63"));
	// 			character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	// 			GetWorldTimerManager().ClearTimer(waitTimer);
	// 		}), 1.63f, false);
	// 	}
	// }
}

void ADPPlayerController::AdditionalSetting(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("AdditionalSetting"));
	// Scroll Up : 1, Scroll Down : -1
	// UE_LOG(LogTemp, Warning, TEXT("->	ia_rotate_x : %f"), value.Get<FVector2D>().X);

	int stateValue = static_cast<int>(value.Get<FVector2D>().X);
	character->ChangeAnimation();
	character->stateComponent->ChangeEquipmentState(stateValue);
}

void ADPPlayerController::Aim(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Aim"));

	if ("NONE" == state->equipmentState) {
		
	}
	if ("GUN" == state->equipmentState) {
		character->PlayAimAnimation();
	}
	if ("WALL" == state->equipmentState) {
		character->isAim = true;
		// 바라보는 방향 청사진 -> 이때 scroll시 wall회전되게	// idAim = true -> active 되면 설치
	}
	if ("TURRET" == state->equipmentState) {
		character->isAim = true;
		// 바라보는 방향 청사진
	}
}

void ADPPlayerController::AimReleased(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("AimReleased"));

	if ("NONE" == state->equipmentState) {

	}
	if ("GUN" == state->equipmentState) {
		character->StopAimAnimation();
	}
	if ("WALL" == state->equipmentState) {
		character->isAim = false;
		// 청사진 제거
	}
	if ("TURRET" == state->equipmentState) {
		character->isAim = false;
		// 청사진 제거
	}
}

void ADPPlayerController::ActionCancel(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("ActionCancel"));
}

void ADPPlayerController::UpdatePlayer()
{
	ActorPosition actorPosition;

	const FString PlayerId = this->PlayerState->GetPlayerName();
	if (!FDataHub::actorPosition.Contains(PlayerId))
	{
		/** 포함되지 않았을 경우 */
		// UE_LOG(LogNetwork, Warning, TEXT("Player 1 data not found"));
		return;
	}
	actorPosition = FDataHub::actorPosition[PlayerId];
	// if (actorPosition.Position.IsSet())
	// {
	// 	FVector NewLocation = actorPosition.Position.GetValue();
	// 	AActor* ControlledActor = GetPawn();
	//
	// 	if (ControlledActor)
	// 	{
	// 		ControlledActor->SetActorLocation(NewLocation);
	// 	}
	// }
}

/*
 * 1. Handler Player Movement in Server (W, A, S, D) - Move Function with Movement Message
 */
void ADPPlayerController::HandleMovement(const Movement& movement)
{
	if (!movement.has_progess_vector())
	{
		return ;
	}
	
	FVector forwardVector = FVector(movement.forward_vector().x(), movement.forward_vector().y(), movement.forward_vector().z());
	FVector rightVector = FVector(movement.right_vector().x(), movement.right_vector().y(), movement.right_vector().z());
	FVector actionValue = FVector(movement.progess_vector().x(), movement.progess_vector().y(), movement.progess_vector().z());
	FVector velocity = FVector(movement.velocity().x(), movement.velocity().y(), movement.velocity().z());
	// float velocitySize = movement.velocity_size();

	FNetLogger::EditerLog(FColor::Cyan, TEXT("Action: %f %f %f"), actionValue.X, actionValue.Y, actionValue.Z);
	FNetLogger::EditerLog(FColor::Cyan, TEXT("Forward: %f %f %f"), forwardVector.X, forwardVector.Y, forwardVector.Z);
	FNetLogger::EditerLog(FColor::Cyan, TEXT("Right: %f %f %f"), rightVector.X, rightVector.Y, rightVector.Z);

	if (!character)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Character is null in HandleMovement"));
		return ;
	}

	character->GetCharacterMovement()->Velocity = velocity;
	
	ServerReceivedMovementCount++;
	FNetLogger::EditerLog(FColor::Red, TEXT("ServerReceivedMovementCount: %d"), ServerReceivedMovementCount);
	
	character->AddMovementInput(forwardVector, actionValue.X);
	character->AddMovementInput(rightVector, actionValue.Y);
	FNetLogger::EditerLog(FColor::Emerald, TEXT("Character Velocity Size: %f"), character->GetCharacterMovement()->Velocity.Size());
	FNetLogger::EditerLog(FColor::Emerald, TEXT("Character Velocity: %f %f %f"), character->GetCharacterMovement()->Velocity.X, character->GetCharacterMovement()->Velocity.Y, character->GetCharacterMovement()->Velocity.Z);
}
