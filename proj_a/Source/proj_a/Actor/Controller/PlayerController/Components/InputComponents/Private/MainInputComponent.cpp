#include "MainInputComponent.h"

#include "DPCharacter.h"
#include "DPGameModeBase.h"
#include "DPPlayerController.h"
#include "DPWeaponActorComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainLevelComponent.h"
#include "MessageMaker.h"
#include "DPStateActorComponent.h"
#include "ReturnTriggerVolume.h"

UMainInputComponent::UMainInputComponent()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext>DEFAULT_CONTEXT
	(TEXT("/Game/input/imc_character.imc_character"));
	if (DEFAULT_CONTEXT.Succeeded())
		MainLevelContext = DEFAULT_CONTEXT.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_MOVE
	(TEXT("/Game/input/ia_move.ia_move"));
	if (IA_MOVE.Succeeded())
		MoveAction = IA_MOVE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_JUMP
	(TEXT("/Game/input/ia_jump.ia_jump"));
	if (IA_JUMP.Succeeded())
		JumpAction = IA_JUMP.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ROTATE
	(TEXT("/Game/input/ia_rotate.ia_rotate"));
	if (IA_ROTATE.Succeeded())
		RotateAction = IA_ROTATE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ACTIVE
	(TEXT("/Game/input/ia_active.ia_active"));
	if (IA_ACTIVE.Succeeded())
		ActiveAction = IA_ACTIVE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_ADDITIONALSETTING
	(TEXT("/Game/input/ia_additionalSetting.ia_additionalSetting"));
	if (IA_ADDITIONALSETTING.Succeeded())
		AdditionalSettingAction = IA_ADDITIONALSETTING.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_AIM
	(TEXT("/Game/input/ia_aim.ia_aim"));
	if (IA_AIM.Succeeded())
		AimAction = IA_AIM.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_CANCEL
	(TEXT("/Game/input/ia_cancel.ia_cancel"));
	if (IA_CANCEL.Succeeded())
		CancelAction = IA_CANCEL.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_CATCH
	(TEXT("/Game/input/ia_catch.ia_catch"));
	if (IA_CATCH.Succeeded())
		CatchAction = IA_CATCH.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction>IA_RETURN
	(TEXT("/Game/input/ia_return.ia_return"));
	if (IA_RETURN.Succeeded())
		ReturnAction = IA_RETURN.Object;
}

void UMainInputComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
	BindMainLevelActions();
}

void UMainInputComponent::Deactivate()
{
	Super::Deactivate();
	UnbindMainLevelActions();
}

void UMainInputComponent::BindMainLevelActions()
{
	ADPPlayerController* PlayerController = GetPlayerController();
	if (!PlayerController) return ;
	if (!PlayerController->IsLocalController()) return ;
	
	// Mapping Context 추가
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		SubSystem->AddMappingContext(MainLevelContext, 0);
	}

	// Enhanced Input Component 가져오기 및 바인딩
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		// 플레이어 이동 ( w, a, d, s )
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UMainInputComponent::Move);
		// 플레이어 점프 ( space )
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &UMainInputComponent::Jump);
		// 시점 변환 ( 마우스 회전 )
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &UMainInputComponent::Rotate);
		//	행동, 총 발사/벽 설치/터렛 설치 ( 마우스 좌클릭 )
		EnhancedInputComponent->BindAction(ActiveAction, ETriggerEvent::Triggered, this, &UMainInputComponent::Active);
		//	변경, 무기 변경/벽 회전 ( 마우스 스크롤 )
		EnhancedInputComponent->BindAction(AdditionalSettingAction, ETriggerEvent::Triggered, this, &UMainInputComponent::AdditionalSetting);
		//	에임 ( 마우스 우클릭 )
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &UMainInputComponent::Aim);	// 	key down
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &UMainInputComponent::AimReleased);
		//	취소, 채팅 끄기 ( esc - UE 에디터에서 기본 단축키 변경 필요 )
		EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Triggered, this, &UMainInputComponent::ActionCancel);
		// 포획 (f)
		EnhancedInputComponent->BindAction(CatchAction, ETriggerEvent::Started, this, &UMainInputComponent::CatchAnimals);
		// 동물 반환 (q)
		EnhancedInputComponent->BindAction(ReturnAction, ETriggerEvent::Started, this, &UMainInputComponent::ReturningAnimals);
	}
}

void UMainInputComponent::UnbindMainLevelActions()
{
	ADPPlayerController* PlayerController = GetPlayerController();
	if (!PlayerController) return ;

	// Enhanced Input Component 바인딩 제거
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		EnhancedInputComponent->ClearActionBindings();
	}

	// Mapping Context 제거
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		if (MainLevelContext)
		{
			SubSystem->RemoveMappingContext(MainLevelContext);
		}
	}
}

void UMainInputComponent::Move(const FInputActionValue& value)
{
	ADPPlayerController* PlayerController = GetPlayerController();
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;
	if (Character->IsStunned())
	{
		return ;
	}
	const FVector2D actionValue = value.Get<FVector2D>();
	const FRotator controlRotation = PlayerController->GetControlRotation();

	const FVector forwardVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::X);
	const FVector rightVector = FRotationMatrix(controlRotation).GetUnitAxis(EAxis::Y);

	Character->AddMovementInput(forwardVector, actionValue.X);
	Character->AddMovementInput(rightVector, actionValue.Y);

	if (Character->isAim)
	{
		FRotator newRotation = FRotator(0, controlRotation.Yaw, 0);
		Character->SetActorRotation(newRotation);
	}
}

void UMainInputComponent::Jump(const FInputActionValue& value)
{
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;
	if (Character->IsStunned())
	{
		return ;
	}
	bool actionValue = value.Get<bool>();
	if (!actionValue) {
		return ;
	}
	Character->Jump();

	USoundComponent* SoundComponent = OwningLevelComponent->GetSoundComponent();
	if (SoundComponent)
		SoundComponent->PlayJumpSound();
}

void UMainInputComponent::Rotate(const FInputActionValue& value)
{
	ADPPlayerController* PlayerController = GetPlayerController();
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;
	
	FVector2D actionValue = value.Get<FVector2D>();

	Character->AddControllerYawInput(actionValue.X);
	Character->AddControllerPitchInput(actionValue.Y);
	if (Character->isAim)
	{
		FRotator rotation = PlayerController->GetControlRotation();
		FRotator newRotation = FRotator(0, rotation.Yaw, 0);
		Character->SetActorRotation(newRotation);
	}
}

// TODO: 총 쏘는 로직 수정 필요
void UMainInputComponent::Active(const FInputActionValue& value)
{
	ADPPlayerController* PlayerController = GetPlayerController();
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;
	UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(OwningLevelComponent);
	if (!MainLevelComponent) return ;
	UDPStateActorComponent* State = MainLevelComponent->GetStateComponent();
	if (!State) return ;

	if ("NONE" == State->equipmentState) {

	}
	if ("GUN" == State->equipmentState) {
		if (!Character->isAim) {
			return ;
		}
		FHitResult hit_result;
		Character->PlayFireAnimation();
		// 최종 발사 위치와, 방향을 알아야 함.
		FRotator final_direction;
		if (Character->weaponComponent->Attack(PlayerController, hit_result, final_direction))
		{
		}
		FVector position = Character->weaponComponent->GetFireLocation();
		Message msg = MessageMaker::MakeFireMessage(PlayerController, position, final_direction);
		if (!PlayerController->HasAuthority())
		{
			UClientSocket* Socket = PlayerController->GetClientSocket();
			Socket->AsyncSendPacket(msg);
		}
		else
		{
			MainLevelComponent->AddGunMessage(msg);
		}
		Character->weaponComponent->SpawnEffects(hit_result, final_direction);
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

void UMainInputComponent::AdditionalSetting(const FInputActionValue& value)
{
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;

	int stateValue = static_cast<int>(value.Get<FVector2D>().X);
	Character->ChangeAnimation();
	// character->stateComponent->ChangeEquipmentState(stateValue);
}

void UMainInputComponent::Aim(const FInputActionValue& value)
{
	ADPPlayerController* PlayerController = GetPlayerController();
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;
	UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(OwningLevelComponent);
	if (!MainLevelComponent) return ;
	UDPStateActorComponent* State = MainLevelComponent->GetStateComponent();
	if (!State) return ;
	
	if ("NONE" == State->equipmentState) {
		
	}
	if ("GUN" == State->equipmentState) {
		if (!Character->isAim) {
			Message msg = MessageMaker::MakeAimMessage(PlayerController, !Character->isAim);
			if (!PlayerController->HasAuthority())
			{
				UClientSocket* Socket = PlayerController->GetClientSocket();
				Socket->AsyncSendPacket(msg);
			}
			else
			{
				MainLevelComponent->AddAimMessage(msg);
			}
			FRotator rotation = PlayerController->GetControlRotation();
			FRotator newRotation = FRotator(0, rotation.Yaw, 0);
			Character->SetActorRotation(newRotation);
		}
		Character->PlayAimAnimation();
	}
	if ("WALL" == State->equipmentState) {
		Character->isAim = true;
	}
	if ("TURRET" == State->equipmentState) {
		Character->isAim = true;
	}
}

void UMainInputComponent::AimReleased(const FInputActionValue& value)
{
	ADPPlayerController* PlayerController = GetPlayerController();
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;

	UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(OwningLevelComponent);
	if (!MainLevelComponent) return ;
	UDPStateActorComponent* State = MainLevelComponent->GetStateComponent();
	if (!State) return ;

	if ("NONE" == State->equipmentState) {

	}
	if ("GUN" == State->equipmentState) {
		Message msg = MessageMaker::MakeAimMessage(PlayerController, !Character->isAim);
		if (!PlayerController->HasAuthority())
		{
			UClientSocket* Socket = PlayerController->GetClientSocket();
			Socket->AsyncSendPacket(msg);
		}
		else
		{
			MainLevelComponent->AddAimMessage(msg);
		}
		Character->StopAimAnimation();
	}
	if ("WALL" == State->equipmentState) {
		Character->isAim = false;
	}
	if ("TURRET" == State->equipmentState) {
		Character->isAim = false;
	}
}

void UMainInputComponent::ActionCancel(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("ActionCancel"));
}

void UMainInputComponent::CatchAnimals(const FInputActionValue& value)
{
	ADPPlayerController* PlayerController = GetPlayerController();
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;
	UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(OwningLevelComponent);
	if (!MainLevelComponent) return ;
	
	if (Character->IsStunned())
	{
		return ;
	}
	if (!MainLevelComponent->GetCurrentTarget())
	{
		return ;
	}
	Message msg = MessageMaker::MakeCatchMessage(PlayerController);
	if (PlayerController->HasAuthority())
	{
		MainLevelComponent->AddCatchMessage(msg);
	}
	else
	{
		UClientSocket* Socket = PlayerController->GetClientSocket();
		Socket->AsyncSendPacket(msg);
	}
}

void UMainInputComponent::ReturningAnimals(const FInputActionValue& value)
{
	ADPPlayerController* PlayerController = GetPlayerController();
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return ;
	UMainLevelComponent* MainLevelComponent = Cast<UMainLevelComponent>(OwningLevelComponent);
	if (!MainLevelComponent) return ;
	
	if (!Character->IsAtReturnPlace())
	{
		return ;
	}
	TArray<EAnimal> animals = Character->ReturnMonsters();
	// 개인 점수 증가
	ADPPlayerState* PlayerState = Cast<ADPPlayerState>(PlayerController->PlayerState);
	if (PlayerState)
	{
		PlayerController->GetPrivateScoreManagerComponent()->IncreasePrivatePlayerScore(PlayerState->GetPlayerJob(), animals);
		uint32 score = PlayerController->GetPrivateScoreManagerComponent()->GetPrivatePlayerScore();
		FString playerName = PlayerState->GetPlayerName();
		FNetLogger::EditerLog(FColor::Cyan, TEXT("Player Score[%s] : %d"), *playerName, score);
	}
	if (PlayerController->HasAuthority())
	{
		ADPGameModeBase* GM = GetWorld()->GetAuthGameMode<ADPGameModeBase>();
		if (GM)
		{
			GM->ScoreManager->IncreasePlayerScore(PlayerController, animals);
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ADPPlayerController* PC = Cast<ADPPlayerController>(It->Get());
			if (PC && PC != PlayerController) // 자기 자신을 제외한 모든 클라이언트
			{
				ADPCharacter* OtherCharacter = Cast<ADPCharacter>(PC->GetPawn());
				OtherCharacter->ClientNotifyAnimalReturn(PlayerState->GetPlayerName());
			}
		}
	}
	else
	{
		MainLevelComponent->ServerNotifyReturnAnimals();
	}
	if (Character->ReturnTriggerVolume)
	{
		Character->ReturnTriggerVolume->SpawnReturnEffect(animals);
	}
}
