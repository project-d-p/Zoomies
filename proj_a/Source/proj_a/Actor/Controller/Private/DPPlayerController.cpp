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
#include "Kismet/KismetMathLibrary.h"


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
	Socket = CreateDefaultSubobject<UClientSocket>(TEXT("MySocket"));

	static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset
	(TEXT("/Game/sounds/effect/character/jump_Cue.jump_Cue"));
	if (SoundAsset.Succeeded()) {
		jumpSound = SoundAsset.Object;
	}
	else
		jumpSound = nullptr;
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

// void ADPPlayerController::CreateSocket()
// {
// 	this->Socket->CreateSocket();
// }

void ADPPlayerController::Connect()
{
	// this->Socket->Connect(ip, port);
	this->Socket->Connect("127.0.0.1", 4242);
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

void ADPPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
	{
		// if (PlayerState == nullptr)
		// {
		// 	return ;
		// }
		// FNetLogger::EditerLog(FColor::Red, TEXT("Tick: %f"), DeltaSeconds);
		//
		// FRotator Rotation = character->GetControlRotation();
		// // FVector Velocity = character->GetCharacterMovement()->Velocity;
		// FVector Velocity = character->GetCharacterMovement()->GetLastUpdateVelocity();
		// Message message = MessageMaker::MakeMovementMessage(this, TODO, Rotation, Velocity);
		// Socket->AsyncSendPacket(message);
		
	}
	else
	{
	}
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

	// enhanced input component ĳ�����ϰ� ���ε�
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
		// �÷��̾� �̵� ( w, a, d, s )
		EnhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Move);
		// �÷��̾� ���� ( space )
		EnhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Jump);
		// ���� ��ȯ ( ���콺 ȸ�� )
		EnhancedInputComponent->BindAction(rotateAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Rotate);
		//	�ൿ, �� �߻�/�� ��ġ/�ͷ� ��ġ ( ���콺 ��Ŭ�� )
		EnhancedInputComponent->BindAction(activeAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Active);
		//	����, ���� ����/�� ȸ�� ( ���콺 ��ũ�� )
		EnhancedInputComponent->BindAction(additionalSettingAction, ETriggerEvent::Triggered, this, &ADPPlayerController::AdditionalSetting);
		//	���� ( ���콺 ��Ŭ�� )
		EnhancedInputComponent->BindAction(aimAction, ETriggerEvent::Triggered, this, &ADPPlayerController::Aim);	// 	key down
		EnhancedInputComponent->BindAction(aimAction, ETriggerEvent::Completed, this, &ADPPlayerController::AimReleased);
		//	���, ä�� ���� ( esc - UE �����Ϳ��� �⺻ ����Ű ���� �ʿ� )
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

	FVector velocity = character->GetCharacterMovement()->Velocity;
	Message message = MessageMaker::MakeMovementMessage(this, actionValue, controlRotation, velocity);
	Socket->AsyncSendPacket(message);

	character->AddMovementInput(forwardVector, actionValue.X);
	character->AddMovementInput(rightVector, actionValue.Y);
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
	// FUdpSendTask::ProtoData.set_allocated_progess_vector(ProtobufUtility::ConvertToFVecToVec3(character->GetControlRotation().Vector()));
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
	// 		// ���� ƽ�� false�� �ٲ�
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
		// �ٶ󺸴� ���� û���� -> �̶� scroll�� wallȸ���ǰ�	// idAim = true -> active �Ǹ� ��ġ
	}
	if ("TURRET" == state->equipmentState) {
		character->isAim = true;
		// �ٶ󺸴� ���� û����
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
		// û���� ����
	}
	if ("TURRET" == state->equipmentState) {
		character->isAim = false;
		// û���� ����
	}
}

void ADPPlayerController::ActionCancel(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("ActionCancel"));
}

void ADPPlayerController::UpdatePlayer()
{
	const FString PlayerId = this->PlayerState->GetPlayerName();
	FNetLogger::EditerLog(FColor::Green, TEXT("Update Player: %s"), *PlayerId);
	if (!FDataHub::actorPosition.Contains(PlayerId))
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Player %s is not in actorPosition"), *PlayerId);
		return;
	}
	ActorPosition actorPosition = FDataHub::actorPosition[PlayerId];
	FVector position = FVector(actorPosition.position().x(), actorPosition.position().y(), actorPosition.position().z());
	FVector velocity = FVector(actorPosition.velocity().x(), actorPosition.velocity().y(), actorPosition.velocity().z());
	FVector current_position = character->GetActorLocation();
	FVector curren_velocity = character->GetCharacterMovement()->Velocity;
	if (this->GetLocalRole() == ROLE_AutonomousProxy)
	{
		float significantDifference = 100.0f;
		float distance = FVector::Dist(position, current_position);

		if (distance > significantDifference)
		{
			FNetLogger::EditerLog(FColor::Red, TEXT("Player %s is too far away from server"), *PlayerId);
			character->SetActorLocation(position);
			character->GetCharacterMovement()->Velocity = velocity;
		}
	}
	// else
	// {
	// 	FVector UnitVector = velocity.GetSafeNormal();
	// 	FVector LookAtTarget = current_position + UnitVector;
	// 	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(current_position, LookAtTarget);
	// 	
	// 	FRotator FinalRotation = FRotator(0, NewRotation.Yaw, 0);
	// 		
	// 	character->SetActorRotation(FinalRotation);
	//
	// 	FVector interpolated_position = FMath::VInterpTo(current_position, position, GetWorld()->GetDeltaSeconds(), 10.0f);
	// 	FVector interpolated_velocity = FMath::VInterpTo(curren_velocity, velocity, GetWorld()->GetDeltaSeconds(), 10.0f);
	// 	
	// 	character->SetActorLocation(interpolated_position);
	// 	character->GetCharacterMovement()->Velocity = interpolated_velocity;
	// }
}

/*
 * 1. Handler Player Movement in Server (W, A, S, D) - Move Function with Movement Message
 */
void ADPPlayerController::HandleMovement(const Movement& movement, const float& server_delta)
{
	FVector velocity = FVector(movement.velocity().x(), movement.velocity().y(), movement.velocity().z());
	FRotator rotation = FRotator(movement.rotation().x(), movement.rotation().y(), movement.rotation().z());
	FVector2d action = FVector2d(movement.action().x(), movement.action().y());
	float delta = movement.delta_time();
	
	if (!character)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Character is null in HandleMovement"));
		return ;
	}
	
	character->GetCharacterMovement()->Velocity = velocity;
	// character->speed = velocity.Size();
	
	// FVector CurrentLocation = character->GetActorLocation();
	// FVector NewLocation = character->GetActorLocation() + (velocity * delta);
	// character->SetActorLocation(NewLocation);

	// FVector UnitVector = velocity.GetSafeNormal();
	// FVector LookAtTarget = CurrentLocation + UnitVector;
	// FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, LookAtTarget);
	//
	// FRotator FinalRotation = FRotator(0, NewRotation.Yaw, 0);
	// 	
	// character->SetActorRotation(FinalRotation);
	
	const FVector forwardVector = FRotationMatrix(rotation).GetUnitAxis(EAxis::X);
	const FVector rightVector = FRotationMatrix(rotation).GetUnitAxis(EAxis::Y);

	character->AddMovementInput(forwardVector, action.X * delta / server_delta);
	character->AddMovementInput(rightVector, action.Y * delta / server_delta);
}
