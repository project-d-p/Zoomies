// Fill out your copyright notice in the Description page of Project Settings.

#include "DPPlayerController.h"

#include "BaseMonsterCharacter.h"
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
#include "state.pb.h"
#include "HitScan.h"
#include "ReturnTriggerVolume.h"
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

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_CATCH
	(TEXT("/Game/input/ia_catch.ia_catch"));
	if (IA_CATCH.Succeeded())
		catchAction = IA_CATCH.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_RETURN
	(TEXT("/Game/input/ia_return.ia_return"));
	if (IA_RETURN.Succeeded())
		returnAction = IA_RETURN.Object;
	
	ChatManager = CreateDefaultSubobject<UChatManager>(TEXT("ChatManager"));
	Socket = CreateDefaultSubobject<UClientSocket>(TEXT("MySocket"));
	CatchRay = CreateDefaultSubobject<UHitScan>(TEXT("Catch Ray"));

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
	
	// GetWorldTimerManager().SetTimer(MovementTimerHandle, this, &ADPPlayerController::SendCompressedMovement, 0.01f, true);
}


// void ADPPlayerController::SendCompressedMovement()
// {
// 	if (HasAuthority())
// 	{
// 		return ;
// 	}
// 	if (AccumulatedMovementInput.IsNearlyZero())
// 	{
// 		return ;
// 	}
//
// 	if (PlayerState)
// 	{
// 		FNetLogger::EditerLog(FColor::Red, TEXT("Player Name: %s"), *PlayerState->GetPlayerName());
// 	}
// 	
// 	FNetLogger::EditerLog(FColor::Blue, TEXT("Send Movement[Client]: %f %f"), AccumulatedMovementInput.X, AccumulatedMovementInput.Y);
// 	FNetLogger::EditerLog(FColor::Blue, TEXT("Send Forward[Client]: %f %f %f"), AccumulatedForwardInput.X, AccumulatedForwardInput.Y, AccumulatedForwardInput.Z);
// 	FNetLogger::EditerLog(FColor::Blue, TEXT("Send Right[Client]: %f %f %f"), AccumulatedRightInput.X, AccumulatedRightInput.Y, AccumulatedRightInput.Z);
// 	// Send Server
// 	// Message message = MessageMaker::MakeMessage(this, AccumulatedMovementInput, AccumulatedForwardInput, AccumulatedRightInput);
// 	// Socket->SendPacket(message);
// 	AccumulatedMovementInput = FVector2D::ZeroVector;
// 	AccumulatedForwardInput = FVector::ZeroVector;
// 	AccumulatedRightInput = FVector::ZeroVector;
// }

void ADPPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PlayerState == nullptr)
	{
		return ;
	}
	if (character == nullptr)
	{
		return ;
	}

	if (character->IsLocallyControlled())
	{
		FHitResult hit_result;
		this->IsCatchable(hit_result);
		this->ChangeMonsterCatchable(hit_result);
	}
	
	if (HasAuthority())
	{
		return ;
	}
	Message msg = MessageMaker::MakePositionMessage(this);
	Socket->AsyncSendPacket(msg);
}

bool ADPPlayerController::IsCatchable(FHitResult& hit_result)
{
	if (CatchRay->HitDetect(character, character->GetActorLocation(), this->GetControlRotation(), 300.0f, hit_result, false))
	{
		return true;
	}
	return false;
}

void ADPPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ADPPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	character = Cast<ADPCharacter>(GetPawn());

	if (!character)
	{
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
		// 포획 (f)
		EnhancedInputComponent->BindAction(catchAction, ETriggerEvent::Started, this, &ADPPlayerController::Catch);
		// 동물 반환 (q)
		EnhancedInputComponent->BindAction(returnAction, ETriggerEvent::Started, this, &ADPPlayerController::ReturningAnimals);
	}
}

void ADPPlayerController::Move(const FInputActionValue& value)
{
	if (character->IsStunned())
	{
		return ;
	}
	const FVector2D actionValue = value.Get<FVector2D>();
	const FRotator controlRotation = GetControlRotation();

	const FVector forwardVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::X);
	const FVector rightVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::Y);

	character->AddMovementInput(forwardVector, actionValue.X);
	character->AddMovementInput(rightVector, actionValue.Y);

	if (character->isAim)
	{
		FRotator newRotation = FRotator(0, controlRotation.Yaw, 0);
		character->SetActorRotation(newRotation);
	}
}

void ADPPlayerController::Jump(const FInputActionValue& value)
{
	//UE_LOG(LogTemp, Warning, TEXT("ia_jump : %d"), value.Get<bool>());
	if (character->IsStunned())
	{
		return ;
	}
	bool actionValue = value.Get<bool>();
	if (!actionValue) {
		return ;
	}
	character->Jump();
	UGameplayStatics::PlaySound2D(GetWorld(), jumpSound);
}

void ADPPlayerController::Rotate(const FInputActionValue& value)
{
	FVector2D actionValue = value.Get<FVector2D>();

	character->AddControllerYawInput(actionValue.X);
	character->AddControllerPitchInput(actionValue.Y);
	if (character->isAim)
	{
		FRotator rotation = GetControlRotation();
		FRotator newRotation = FRotator(0, rotation.Yaw, 0);
		character->SetActorRotation(newRotation);
	}
}

// TODO: 총 쏘는 로직 수정 필요
void ADPPlayerController::Active(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Active"));

	FNetLogger::EditerLog(FColor::Cyan, TEXT("Active with %s"), *state->equipmentState);
	if ("NONE" == state->equipmentState) {

	}
	if ("GUN" == state->equipmentState) {
		if (!character->isAim) {
			return ;
		}
		FHitResult hit_result;
		character->PlayFireAnimation();
		// 최종 발사 위치와, 방향을 알아야 함.
		FRotator final_direction;
		if (character->weaponComponent->Attack(this, hit_result, final_direction))
		{
			// Success Only Effect;
			FNetLogger::EditerLog(FColor::Cyan, TEXT("Impact Point[Controller]: %f, %f, %f"), hit_result.ImpactPoint.X, hit_result.ImpactPoint.Y, hit_result.ImpactPoint.Z);
			FNetLogger::EditerLog(FColor::Cyan, TEXT("Attack Success[Only Effect]"));
		}
		FVector position = character->weaponComponent->GetFireLocation();
		Message msg = MessageMaker::MakeFireMessage(this, position, final_direction);
		if (!HasAuthority())
		{
			Socket->AsyncSendPacket(msg);
		}
		else
		{
			this->gun_fire_count_ += 1;
			this->gun_queue_.push(msg);
		}
		character->weaponComponent->SpawnEffects(hit_result, final_direction);
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

	int stateValue = static_cast<int>(value.Get<FVector2D>().X);
	character->ChangeAnimation();
	// character->stateComponent->ChangeEquipmentState(stateValue);
}

void ADPPlayerController::Aim(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Aim"));

	if ("NONE" == state->equipmentState) {
		
	}
	if ("GUN" == state->equipmentState) {
		if (!character->isAim) {
			Message msg = MessageMaker::MakeAimMessage(this, !character->isAim);
			if (!HasAuthority())
			{
				Socket->AsyncSendPacket(msg);
			}
			else
			{
				aim_queue_.push(!character->isAim);
			}
			FRotator rotation = GetControlRotation();
			FRotator newRotation = FRotator(0, rotation.Yaw, 0);
			character->SetActorRotation(newRotation);
		}
		character->PlayAimAnimation();
	}
	if ("WALL" == state->equipmentState) {
		character->isAim = true;
	}
	if ("TURRET" == state->equipmentState) {
		character->isAim = true;
	}
}

void ADPPlayerController::AimReleased(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("AimReleased"));

	if ("NONE" == state->equipmentState) {

	}
	if ("GUN" == state->equipmentState) {
		Message msg = MessageMaker::MakeAimMessage(this, !character->isAim);
		if (!HasAuthority())
		{
			Socket->AsyncSendPacket(msg);
		}
		else
		{
			aim_queue_.push(!character->isAim);
		}
		character->StopAimAnimation();
	}
	if ("WALL" == state->equipmentState) {
		character->isAim = false;
	}
	if ("TURRET" == state->equipmentState) {
		character->isAim = false;
	}
}

void ADPPlayerController::ActionCancel(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("ActionCancel"));
}

void ADPPlayerController::Catch(const FInputActionValue& value)
{
	FHitResult hit_result;
	if (character->IsStunned())
	{
		return ;
	}
	if (!this->IsCatchable(hit_result))
	{
		return ;
	}
	if (Cast<ABaseMonsterCharacter>(hit_result.GetActor()) == nullptr)
	{
		return ;
	}
	if (Cast<ABaseMonsterCharacter>(hit_result.GetActor())->GetState() != EMonsterState::Faint)
	{
		return ;
	}
	Message msg = MessageMaker::MakeCatchMessage(this);
	if (HasAuthority())
	{
		catch_queue_.push(msg);
	}
	else
	{
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Send Catch Message"));
		Socket->AsyncSendPacket(msg);
	}
}

void ADPPlayerController::ReturningAnimals(const FInputActionValue& value)
{
	if (!character->IsAtReturnPlace())
	{
		return ;
	}
	TArray<EAnimal> animals = character->ReturnMonsters();
	// 개인 점수 증가
	ADPPlayerState* player_state = Cast<ADPPlayerState>(PlayerState);
	if (player_state)
	{
		this->PrivateScoreManager->IncreasePrivatePlayerScore(player_state->GetPlayerJob(), animals);
	}
	if (HasAuthority())
	{
		ADPGameModeBase* GM = GetWorld()->GetAuthGameMode<ADPGameModeBase>();
		if (GM)
		{
			GM->ScoreManager->IncreasePlayerScore(this, animals);
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ADPPlayerController* PC = Cast<ADPPlayerController>(It->Get());
			if (PC && PC != this) // 자기 자신을 제외한 모든 클라이언트
			{
				PC->character->ClientNotifyAnimalReturn(player_state->GetPlayerName());
			}
		}
	}
	else
	{
		this->ServerNotifyReturnAnimals();
	}
	if (character->ReturnTriggerVolume)
	{
		character->ReturnTriggerVolume->SpawnReturnEffect(animals);
	}
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
	
	const FVector forwardVector = FRotationMatrix(rotation).GetUnitAxis(EAxis::X);
	const FVector rightVector = FRotationMatrix(rotation).GetUnitAxis(EAxis::Y);

	character->AddMovementInput(forwardVector, action.X);
	character->AddMovementInput(rightVector, action.Y);

	if (character->isAim)
	{
		FRotator newRotation = FRotator(0, rotation.Yaw, 0);
		character->SetActorRotation(newRotation);
	}
}

void ADPPlayerController::HandleJump(const ::Jump& Jump)
{
	if (!character)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Character is null in HandleMovement"));
		return ;
	}
	character->Jump();
	UGameplayStatics::PlaySound2D(GetWorld(), jumpSound);
}

void ADPPlayerController::HandleFire(const Message& fire)
{
	this->gun_fire_count_ += 1;
	this->gun_queue_.push(fire);
}

void ADPPlayerController::HandleCatch(const Message& catch_)
{
	this->catch_queue_.push(catch_);
}

void ADPPlayerController::SimulateGunFire(SteamNetworkingSocket* steam_socket)
{
	if (gun_fire_count_ == 0)
	{
		return ;
	}
	while (!gun_queue_.empty())
	{
		Message fire = gun_queue_.front();
		gun_queue_.pop();
		FHitResult hit_result;
		this->character->PlayFireAnimation();
		if (character->weaponComponent->SimulateAttack(character, hit_result, fire.gunfire()))
		{
			ABaseMonsterCharacter* MC = Cast<ABaseMonsterCharacter>(hit_result.GetActor());
			if (MC)
			{
				ABaseMonsterAIController* MAC = Cast<ABaseMonsterAIController>(MC->GetOwner());
				if (MAC)
				{
					// MAC->RemovePawnAndController();
					MAC->TakeMonsterDamage(100);
				}
			}
			// Logic for Hit Success && Damage && Score
			FNetLogger::EditerLog(FColor::Cyan, TEXT("Player %s Attack Success[Simulate]"), *PlayerState->GetPlayerName());

			// Add Particle Effect
			if (!IsLocalController())
			{
				FRotator final_direction = FRotator(fire.gunfire().direction().x(), fire.gunfire().direction().y(), fire.gunfire().direction().z());
				character->weaponComponent->SpawnEffects(hit_result, final_direction);
			}
		}
		steam_socket->PushUdpFlushMessage(fire);
		gun_fire_count_ -= 1;
	}
}

void ADPPlayerController::HandleAim(const AimState& AimState)
{
	if (!character)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Character is null in HandleAim"));
		return ;
	}
	if (AimState.aim_state() == AimState::AIM_STATE_ACTIVE)
	{
		character->PlayAimAnimation();
	}
	else
	{
		character->StopAimAnimation();
	}
}

void ADPPlayerController::HandlePosition(const ActorPosition& ActorPosition)
{
	if (!character)
	{
		FNetLogger::EditerLog(FColor::Red, TEXT("Character is null in HandleAim"));
		return ;
	}
	SetState(ActorPosition);
	SetRotation(ActorPosition);
	SetPosition(ActorPosition);
}

void ADPPlayerController::SetRotation(const ActorPosition& ActorPosition)
{
	if (character->isAim == true)
	{
		FRotator control_rotation = FRotator(ActorPosition.control_rotation().x(), ActorPosition.control_rotation().y(), ActorPosition.control_rotation().z());
		FRotator final_rotation = FRotator(0, control_rotation.Yaw, 0);
		character->SetActorRotation(final_rotation);
	}
	else
	{
		FRotator rotation = FRotator(ActorPosition.rotation().x(), ActorPosition.rotation().y(), ActorPosition.rotation().z());
		FRotator newRotation = FRotator(0, rotation.Yaw, 0);
		character->SetActorRotation(newRotation);
	}
}

void ADPPlayerController::SetPosition(const ActorPosition& ActorPosition)
{
	FVector position = FVector(ActorPosition.position().x(), ActorPosition.position().y(), ActorPosition.position().z());
	FVector velocity = FVector(ActorPosition.velocity().x(), ActorPosition.velocity().y(), ActorPosition.velocity().z());

	FVector current_position = character->GetActorLocation();
	FVector current_velocity = character->GetCharacterMovement()->Velocity;
	
	FVector interpolated_position = FMath::VInterpTo(current_position, position, GetWorld()->GetDeltaSeconds(), 10.0f);
	FVector interpolated_velocity = FMath::VInterpTo(current_velocity, velocity, GetWorld()->GetDeltaSeconds(), 10.0f);
	
	character->SetActorLocation(interpolated_position);
	character->GetCharacterMovement()->Velocity = interpolated_velocity;
}

void ADPPlayerController::SetState(const ActorPosition& ActorPosition)
{
	State state_ = ActorPosition.state();
	switch (state_)
	{
	case State::STATE_Walking:
		character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		break;
	case State::STATE_Falling:
		character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		break;
	default:
		character->GetCharacterMovement()->SetMovementMode(MOVE_None);
		break;
	}
}

void ADPPlayerController::SimulateCatch(SteamNetworkingSocket* steam_socket)
{
	if (catch_queue_.empty())
	{
		return ;
	}
	while (!catch_queue_.empty())
	{
		Message catch_ = catch_queue_.front();
		catch_queue_.pop();
		::Catch catch_message = catch_.catch_();
		FVector start = FVector(catch_message.position().x(), catch_message.position().y(), catch_message.position().z());
		FRotator direction = FRotator(catch_message.rotation().x(), catch_message.rotation().y(), catch_message.rotation().z());
		
		FHitResult hit_result;
		if (!CatchRay->HitDetect(character, start, direction, 300.0f, hit_result))
		{
			continue;
		}
		ABaseMonsterCharacter* MC = Cast<ABaseMonsterCharacter>(hit_result.GetActor());
		if (!MC)
		{
			continue;
		}
		if (MC->GetState() != EMonsterState::Faint)
		{
			continue;
		}
		UClass* class_type = hit_result.GetActor()->GetClass();
		FString monster_type = class_type->GetName();
		if (!this->character->CatchMonster(monster_type))
		{
			continue ;
		}
		this->CurrentTarget = nullptr;
		ABaseMonsterAIController* MAC = Cast<ABaseMonsterAIController>(MC->GetOwner());
		check(MAC)
		MAC->RemovePawnAndController();
		::Catch reply;
		reply.set_target(TCHAR_TO_UTF8(*monster_type));
		*catch_.mutable_catch_() = reply;
		FString TestString = UTF8_TO_TCHAR(reply.target().c_str());
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Catch monster_id: %s"), *TestString);
		steam_socket->PushUdpFlushMessage(catch_);
	}
}

void ADPPlayerController::ServerNotifyReturnAnimals_Implementation()
{
	// 개인 점수 증가
	ADPPlayerState* player_state = Cast<ADPPlayerState>(PlayerState);
	if (!player_state)
	{
		return ;
	}
	FNetLogger::EditerLog(FColor::Cyan, TEXT("Server Notify Return Animals[from: %s]"), *player_state->GetPlayerName());
	
	// 클라이언트의 동물 반환 처리를 처리한다.
	TArray<EAnimal> animals = character->ReturnMonsters();
	
	this->PrivateScoreManager->IncreasePrivatePlayerScoreByServer(player_state->GetPlayerJob(), animals);
	ADPGameModeBase* GM = GetWorld()->GetAuthGameMode<ADPGameModeBase>();
	if (GM)
	{
		GM->ScoreManager->IncreasePlayerScore(this, animals);
	}

	if (character->ReturnTriggerVolume)
	{
		character->ReturnTriggerVolume->SpawnReturnEffect(animals);
	}
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ADPPlayerController* PC = Cast<ADPPlayerController>(It->Get());
		if (PC && PC != this) // 자기 자신을 제외한 모든 클라이언트
		{
			PC->character->ClientNotifyAnimalReturn(player_state->GetPlayerName());
		}
	}
}

bool ADPPlayerController::ServerNotifyReturnAnimals_Validate()
{
	// 호출이 유효한지 확인하는 로직을 구현
	return true; // 유효성 검증이 항상 참인 경우
}

void ADPPlayerController::ChangeMonsterCatchable(const FHitResult& HitResult)
{
	ABaseMonsterCharacter* NewTarget = Cast<ABaseMonsterCharacter>(HitResult.GetActor());

	if (CurrentTarget != NewTarget)
	{
		if (CurrentTarget)
		{
			CurrentTarget->SetCatchable(false);
		}
		if (NewTarget)
		{
			if (NewTarget->GetState() == EMonsterState::Faint)
			{
				CurrentTarget = NewTarget;
				CurrentTarget->SetCatchable(true);
			}
			else
			{
				CurrentTarget = nullptr;
			}
		}
		else
		{
			CurrentTarget = nullptr;
		}
	}
}
