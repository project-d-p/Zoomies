// Fill out your copyright notice in the Description page of Project Settings.

#include "DPPlayerController.h"
#include "DPCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "DPWeaponActorComponent.h"
#include "DPConstructionActorComponent.h"
#include "DPGameModeBase.h"
#include "DPStateActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NetComp.h"
#include "ProtobufUtility.h"
#include "movement.pb.h"
#include "FDataHub.h"
#include "FNetLogger.h"
#include "FUdpSendTask.h"

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

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_CHAT
	(TEXT("/Game/input/ia_chat.ia_chat"));
	if (IA_CHAT.Succeeded())
		chatAction = IA_CHAT.Object;
}

void ADPPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	character = Cast<ADPCharacter>(GetPawn());
	
	if (!character)
	{
		UE_LOG(LogTemp, Warning, TEXT("character null"));
		return;
	}
	else {
		state = Cast<UDPStateActorComponent>(character->GetComponentByClass(UDPStateActorComponent::StaticClass()));
		construction = Cast<UDPConstructionActorComponent>(character->GetComponentByClass(UDPConstructionActorComponent::StaticClass()));
	}
	
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
		SubSystem->AddMappingContext(defaultContext, 0);
}

void ADPPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdatePlayer();
}

void ADPPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
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
		//	채팅 열기 ( enter )
		EnhancedInputComponent->BindAction(chatAction, ETriggerEvent::Triggered, this, &ADPPlayerController::OpenChat);
	}
}

void ADPPlayerController::Move(const FInputActionValue& value)
{
	// UE_LOG(LogTemp, Warning, TEXT("ia_move_x : %f"), value.Get<FVector2D>().X);
	// UE_LOG(LogTemp, Warning, TEXT("ia_move_y : %f"), value.Get<FVector2D>().Y);

	const FVector2D actionValue = value.Get<FVector2D>();
	const FRotator controlRotation = GetControlRotation();
	const FRotator yaw(0.f, controlRotation.Yaw, 0.f);

	const FVector forwardVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::X);
	const FVector rightVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::Y);
	
	// UNetComp::inputTCP(actionValue, 0);
	UNetComp::InputUDP(actionValue);
	character->AddMovementInput(forwardVector, actionValue.X);
	character->AddMovementInput(rightVector, actionValue.Y);
}

void ADPPlayerController::Jump(const FInputActionValue& value)
{
	//UE_LOG(LogTemp, Warning, TEXT("ia_jump : %d"), value.Get<bool>());

	bool actionValue = value.Get<bool>();
	if (actionValue)
		// send jump command ( id, actionValue ) ( true = jump )
		character->Jump();
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
	if ("WALL" == state->equipmentState) {
		//if (character->isAim) {
			character->GetCharacterMovement()->DisableMovement();
			construction->MakeWall({ 0, 0, 0 }, { 0, 0, 0 });
			character->constructionComponent->placeWall = true;
			// 다음 틱에 false로 바꿈
			auto resetPlaceWall = [this]() {
				character->constructionComponent->placeWall = false;
			};
			GetWorld()->GetTimerManager().SetTimerForNextTick(resetPlaceWall);

			FTimerHandle waitTimer;
			GetWorld()->GetTimerManager().SetTimer(waitTimer, FTimerDelegate::CreateLambda([&]() {
				UE_LOG(LogTemp, Warning, TEXT("wall delay 1.63"));
				character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);	// movement disable -> enable
				GetWorldTimerManager().ClearTimer(waitTimer);
			}), 1.63f, false);
		//}
	}
	if ("TURRET" == state->equipmentState) {
		if (character->isAim) {
			character->GetCharacterMovement()->DisableMovement();
			character->constructionComponent->placeturret = true;

			auto resetPlaceTurret = [this]() {
				character->constructionComponent->placeturret = false;
			};
			GetWorld()->GetTimerManager().SetTimerForNextTick(resetPlaceTurret);

			FTimerHandle waitTimer;
			GetWorld()->GetTimerManager().SetTimer(waitTimer, FTimerDelegate::CreateLambda([&]() {
				UE_LOG(LogTemp, Warning, TEXT("turret delay 1.63"));
				character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				GetWorldTimerManager().ClearTimer(waitTimer);
			}), 1.63f, false);
		}
	}
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

void ADPPlayerController::OpenChat(const FInputActionValue& value)
{
	ServerSendChatMessage(TEXT("Hello, this is a test message!"));
}

void ADPPlayerController::ServerSendChatMessage_Implementation(const FString& Message)
{
	if (ADPGameModeBase* GM = Cast<ADPGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		FString SenderName = TEXT("Unknown");
		GM->BroadcastChatMessage(SenderName, Message);
	}
}

bool ADPPlayerController::ServerSendChatMessage_Validate(const FString& Message)
{
	return !Message.IsEmpty();
}

void ADPPlayerController::ClientReceiveChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Cyan,
			FString::Printf(TEXT("%s: %s"), *SenderName, *Message));
	}
}

void ADPPlayerController::UpdatePlayer()
{
	Movement movement;
	if (!FDataHub::EchoData.Contains("player1")) {
		// UE_LOG(LogNetwork, Warning, TEXT("Player 1 data not found"));
		return;
	}
	movement = FDataHub::EchoData["player1"];

	// UE_LOG(LogTemp, Warning, TEXT("Progress: %f %f %f"), movement.progess_vector().x(), movement.progess_vector().y(), movement.progess_vector().z());
	if (movement.has_progess_vector())
	{
		FVector rightVector = character->GetActorRightVector();
		FVector forwardVector = character->GetActorForwardVector();
		FVector actionValue = FVector(movement.progess_vector().x(), movement.progess_vector().y(), movement.progess_vector().z());
		
		character->AddMovementInput(forwardVector, actionValue.X);
		character->AddMovementInput(rightVector, actionValue.Y);
	}

	// XXX: 추후 PlayerPosition 메시지가 정의되어 들어오면 새롭게 정의필요. 아래 주석 코드 참조.

	// PlayerPosition result = FDataHub::PlayerPositions["1"];
	// if (result.has_position())
	// 	character->SetActorLocation(FVector(result.position().x(), result.position().y(), result.position().z()));
	// if (result.jumpResult)
	// 	character->Jump();
	// if (result.rotateResult)
	// 	character->
	// character->SetActorRotation();
	// character->SetActorLocationAndRotation();
}
