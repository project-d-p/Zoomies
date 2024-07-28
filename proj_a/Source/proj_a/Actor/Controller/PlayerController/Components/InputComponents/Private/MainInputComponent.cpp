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
	
	// Mapping Context �߰�
	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		SubSystem->AddMappingContext(MainLevelContext, 0);
	}

	// Enhanced Input Component �������� �� ���ε�
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		// �÷��̾� �̵� ( w, a, d, s )
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UMainInputComponent::Move);
		// �÷��̾� ���� ( space )
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &UMainInputComponent::Jump);
		// ���� ��ȯ ( ���콺 ȸ�� )
		EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &UMainInputComponent::Rotate);
		//	�ൿ, �� �߻�/�� ��ġ/�ͷ� ��ġ ( ���콺 ��Ŭ�� )
		EnhancedInputComponent->BindAction(ActiveAction, ETriggerEvent::Triggered, this, &UMainInputComponent::Active);
		//	����, ���� ����/�� ȸ�� ( ���콺 ��ũ�� )
		EnhancedInputComponent->BindAction(AdditionalSettingAction, ETriggerEvent::Triggered, this, &UMainInputComponent::AdditionalSetting);
		//	���� ( ���콺 ��Ŭ�� )
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &UMainInputComponent::Aim);	// 	key down
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &UMainInputComponent::AimReleased);
		//	���, ä�� ���� ( esc - UE �����Ϳ��� �⺻ ����Ű ���� �ʿ� )
		EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Triggered, this, &UMainInputComponent::ActionCancel);
		// ��ȹ (f)
		EnhancedInputComponent->BindAction(CatchAction, ETriggerEvent::Started, this, &UMainInputComponent::CatchAnimals);
		// ���� ��ȯ (q)
		EnhancedInputComponent->BindAction(ReturnAction, ETriggerEvent::Started, this, &UMainInputComponent::ReturningAnimals);
	}
}

void UMainInputComponent::UnbindMainLevelActions()
{
	ADPPlayerController* PlayerController = GetPlayerController();
	if (!PlayerController) return ;

	// Enhanced Input Component ���ε� ����
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
	{
		EnhancedInputComponent->ClearActionBindings();
	}

	// Mapping Context ����
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

// TODO: �� ��� ���� ���� �ʿ�
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
		// ���� �߻� ��ġ��, ������ �˾ƾ� ��.
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
	// ���� ���� ����
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
			if (PC && PC != PlayerController) // �ڱ� �ڽ��� ������ ��� Ŭ���̾�Ʈ
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
