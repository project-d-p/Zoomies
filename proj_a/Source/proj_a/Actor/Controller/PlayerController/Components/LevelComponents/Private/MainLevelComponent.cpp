#include "MainLevelComponent.h"

#include "DPCharacter.h"
#include "DPPlayerController.h"
#include "DPStateActorComponent.h"
#include "BaseMonsterCharacter.h"
#include "DPGameModeBase.h"
#include "DPInGameState.h"
#include "DPWeaponActorComponent.h"
#include "FNetLogger.h"
#include "HitScan.h"
#include "MainInputComponent.h"
#include "CompileMode.h"
#include "MessageMaker.h"
#include "ReturnTriggerVolume.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "proj_a/Component/InGame/Score/PrivateScoreManager.h"

UMainLevelComponent::UMainLevelComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	InputComponent = CreateDefaultSubobject<UMainInputComponent>(TEXT("MainInputComponent"));
	SoundComponent = CreateDefaultSubobject<USoundComponent>(TEXT("MainSoundComponent"));
	CatchRay = CreateDefaultSubobject<UHitScan>(TEXT("CatchRay"));

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/widget/widget_ingame1.widget_ingame1_C"));
	if (WidgetBPClass.Succeeded())
	{
		WidgetClass = WidgetBPClass.Class;
	}

	InputComponent->SetLevelComponent(this);

}

void UMainLevelComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (Cast<APlayerController>(GetOwner())->IsLocalPlayerController())
	{
		if (!InGameWidget)
		{
			InGameWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), WidgetClass);
		}
	}
	if (InGameWidget)
	{
		InGameWidget->AddToViewport();
	}
}

void UMainLevelComponent::Deactivate()
{
	Super::Deactivate();

	if (InGameWidget)
	{
		InGameWidget->RemoveFromParent();
		InGameWidget = nullptr;
	}
}

void UMainLevelComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Cast<APlayerController>(GetOwner())->IsLocalPlayerController())
	{
		if (!InGameWidget)
		{
			InGameWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), WidgetClass);
		}
	}
}

void UMainLevelComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UMainLevelComponent::SetStateComponent()
{
	if (const ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter()))
	{
		StateComponent = Cast<UDPStateActorComponent>(Character->GetComponentByClass(UDPStateActorComponent::StaticClass()));
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMainLevelComponent::SetStateComponent, 1.0f, false);
	}
}

UDPStateActorComponent* UMainLevelComponent::GetStateComponent() const
{
	return StateComponent;
}

void UMainLevelComponent::AddGunMessage(const Message& message)
{
	GunQueue.push(message);
}

void UMainLevelComponent::AddCatchMessage(const Message& message)
{
	CatchQueue.push(message);
}

void UMainLevelComponent::AddAimMessage(const Message& message)
{
	AimQueue.push(message);
}

const TargetInfo& UMainLevelComponent::GetCurrentTarget() const
{
	return CurrentTarget;
}

void UMainLevelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PrimaryComponentTick.bCanEverTick == false)
	{
		return ;
	}

	ADPPlayerController* PlayerController = GetPlayerController();
	if (PlayerController == nullptr)
	{
		return ;
	}
	if (PlayerController->PlayerState == nullptr)
	{
		return ;
	}
	
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (Character == nullptr)
	{
		return ;
	}

	if (Character->IsLocallyControlled())
	{
		FHitResult hit_result;
		this->IsCatchable(hit_result);
		this->ChangeMonsterCatchable(hit_result);
	}
	
	if (PlayerController->HasAuthority())
	{
		return ;
	}
	
	Message msg = MessageMaker::MakePositionMessage(PlayerController);

	UANetworkManager* NetworkManager = PlayerController->GetNetworkManager();
	NetworkManager->SendData(msg);
}

bool UMainLevelComponent::IsCatchable(FHitResult& HitResult)
{
	ADPPlayerController* PlayerController = GetPlayerController();
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	FVector Start = Character->GetCameraLocation();

	FVector ImpactPoint = Start + Character->GetControlRotation().Vector() * 3000.0f;
	if (CatchRay->HitDetect(Character, Start, PlayerController->GetControlRotation(), 3000.0f, HitResult, false))
	{
		ImpactPoint = HitResult.ImpactPoint;
	}

	FVector Direction = (ImpactPoint - Character->GetActorLocation()).GetSafeNormal();
	
	if (CatchRay->HitDetect(Character, Character->GetActorLocation(), Direction.Rotation(), 300.0f, HitResult, false))
	{
		CurrentTarget.Location = Character->GetActorLocation();
		CurrentTarget.Rotation = Direction.Rotation();
		return true;
	}
	HitResult = FHitResult();
	return false;
}

void UMainLevelComponent::ChangeMonsterCatchable(const FHitResult& HitResult)
{
	ABaseMonsterCharacter* NewTarget = Cast<ABaseMonsterCharacter>(HitResult.GetActor());

	if (CurrentTarget.CurrentTarget != NewTarget)
	{
		if (CurrentTarget.CurrentTarget)
		{
			CurrentTarget.CurrentTarget->SetCatchable(false);
		}
		if (NewTarget)
		{
			if (NewTarget->GetState() == EMonsterState::Faint)
			{
				CurrentTarget.CurrentTarget = NewTarget;
				CurrentTarget.CurrentTarget->SetCatchable(true);
			}
			else
			{
				CurrentTarget.CurrentTarget = nullptr;
			}
		}
		else
		{
			CurrentTarget.CurrentTarget = nullptr;
		}
	}
}

void UMainLevelComponent::ServerNotifyReturnAnimals_Implementation()
{
	ADPPlayerController* PlayerController = GetPlayerController();
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return;
	
	ADPPlayerState* PlayerState = Cast<ADPPlayerState>(PlayerController->PlayerState);
	if (!PlayerState)
	{
		return ;
	}

	TArray<EAnimal> animals = Character->ReturnMonsters();
	if (animals.Num() != 0)
	{
		PlayerState->IncreaseScore(animals);
		FString playerName = PlayerState->GetPlayerName();
		if (Character->ReturnTriggerVolume)
		{
			Character->ReturnTriggerVolume->SpawnReturnEffect(animals);
		}
	}
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ADPPlayerController* PC = Cast<ADPPlayerController>(It->Get());
		if (PC && PC != PlayerController)
		{
			ADPCharacter* OtherCharacter = Cast<ADPCharacter>(PC->GetPawn());
#if EDITOR_MODE || LAN_MODE
			OtherCharacter->ClientNotifyAnimalReturn(PlayerState->GetPlayerName());
#else
			OtherCharacter->ClientNotifyAnimalReturn(PlayerState->GetUniqueId()->ToString());
#endif
		}
	}
}

bool UMainLevelComponent::ServerNotifyReturnAnimals_Validate()
{
	// ȣ���� ��ȿ���� Ȯ���ϴ� ������ ����
	return true; // ��ȿ�� ������ �׻� ���� ���
}

void UMainLevelComponent::HandlePosition(const ActorPosition& ActorPosition)
{
	SetState(ActorPosition);
	SetRotation(ActorPosition);
	SetPosition(ActorPosition);
}

void UMainLevelComponent::SetRotation(const ActorPosition& ActorPosition)
{
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return;
	if (Character->isAim == true)
	{
		FRotator control_rotation = FRotator(ActorPosition.control_rotation().x(), ActorPosition.control_rotation().y(), ActorPosition.control_rotation().z());
		FRotator final_rotation = FRotator(0, control_rotation.Yaw, 0);
		Character->SetActorRotation(final_rotation);
	}
	else
	{
		FRotator rotation = FRotator(ActorPosition.rotation().x(), ActorPosition.rotation().y(), ActorPosition.rotation().z());
		FRotator newRotation = FRotator(0, rotation.Yaw, 0);
		Character->SetActorRotation(newRotation);
	}
}

void UMainLevelComponent::SetPosition(const ActorPosition& ActorPosition)
{
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return;
	FVector position = FVector(ActorPosition.position().x(), ActorPosition.position().y(), ActorPosition.position().z());
	FVector velocity = FVector(ActorPosition.velocity().x(), ActorPosition.velocity().y(), ActorPosition.velocity().z());

	FVector current_position = Character->GetActorLocation();
	FVector current_velocity = Character->GetCharacterMovement()->Velocity;
	
	FVector interpolated_position = FMath::VInterpTo(current_position, position, GetWorld()->GetDeltaSeconds(), 10.0f);
	FVector interpolated_velocity = FMath::VInterpTo(current_velocity, velocity, GetWorld()->GetDeltaSeconds(), 10.0f);
	
	Character->SetActorLocation(interpolated_position);
	Character->GetCharacterMovement()->Velocity = interpolated_velocity;
}

void UMainLevelComponent::SetState(const ActorPosition& ActorPosition)
{
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return;
	
	switch (ActorPosition.state())
	{
	case State::STATE_Walking:
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		break;
	case State::STATE_Falling:
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		break;
	default:
		Character->GetCharacterMovement()->SetMovementMode(MOVE_None);
		break;
	}
}

void UMainLevelComponent::SimulateGunFire(UANetworkManager* NetworkManager)
{
	ADPPlayerController* PlayerController = GetPlayerController();
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return;
	
	while (!GunQueue.empty())
	{
		Message fire = GunQueue.front();
		GunQueue.pop();
		FHitResult hit_result;
		Character->PlayFireAnimation();
		if (Character->weaponComponent->SimulateAttack(Character, hit_result, fire.gunfire()))
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
			// Add Particle Effect
			if (!PlayerController->IsLocalController())
			{
				FRotator final_direction = FRotator(fire.gunfire().direction().x(), fire.gunfire().direction().y(), fire.gunfire().direction().z());
				Character->weaponComponent->SpawnEffects(hit_result, final_direction);
			}
		}
		NetworkManager->SendData(fire);
	}
}

void UMainLevelComponent::SimulateCatch(UANetworkManager* NetworkManager)
{
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return;
	
	while (!CatchQueue.empty())
	{
		Message catch_ = CatchQueue.front();
		CatchQueue.pop();
		::Catch catch_message = catch_.catch_();
		FVector start = FVector(catch_message.position().x(), catch_message.position().y(), catch_message.position().z());
		FRotator direction = FRotator(catch_message.rotation().x(), catch_message.rotation().y(), catch_message.rotation().z());
		
		FHitResult hit_result;
		if (!CatchRay->HitDetect(Character, start, direction, 300.0f, hit_result, false))
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
		if (!Character->CatchMonster(monster_type))
		{
			continue ;
		}
		this->CurrentTarget.CurrentTarget = nullptr;
		ABaseMonsterAIController* MAC = Cast<ABaseMonsterAIController>(MC->GetOwner());
		check(MAC)
		MAC->RemovePawnAndController();
		::Catch reply;
		reply.set_target(TCHAR_TO_UTF8(*monster_type));
		*catch_.mutable_catch_() = reply;
		FString TestString = UTF8_TO_TCHAR(reply.target().c_str());
		NetworkManager->SendData(catch_);
	}
}

void UMainLevelComponent::SimulateAim(UANetworkManager* NetworkManager)
{
	ADPCharacter* Character = Cast<ADPCharacter>(GetPlayerCharacter());
	if (!Character) return;

	while (!AimQueue.empty())
	{
		Message aimState = AimQueue.front();
		AimQueue.pop();
		if (aimState.aim_state().aim_state() == AimState::AIM_STATE_ACTIVE)
		{
			Character->PlayAimAnimation();
		}
		else
		{
			Character->StopAimAnimation();
		}
		NetworkManager->SendData(aimState);
	}
}

UUserWidget* UMainLevelComponent::GetInGameWidget() const
{
	return InGameWidget;
}

void UMainLevelComponent::BeginDestroy()
{
	Super::BeginDestroy();

	if (InGameWidget)
	{
		InGameWidget->RemoveFromParent();
		InGameWidget = nullptr;
	}
}
