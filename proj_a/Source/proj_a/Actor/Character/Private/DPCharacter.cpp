// Fill out your copyright notice in the Description page of Project Settings.

#include "DPCharacter.h"

#include "BaseMonsterCharacter.h"
#include "CharacterData.h"
#include "ChasePlayerMonsterAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DPHpActorComponent.h"
#include "DPConstructionActorComponent.h"
#include "DPPlayerState.h"
#include "DPWeaponActorComponent.h"
#include "DPStateActorComponent.h"
#include "DPWeaponGun.h"
#include "DynamicTextureComponent.h"
#include "FDataHub.h"
#include "FNetLogger.h"
#include "MonsterSlotComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "proj_a.h"
#include "ReturnTriggerVolume.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "DSP/LFO.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"
#include "proj_a/GameInstance/GI_Zoomies.h"
#include "proj_a/MatchingLobby/GS_MachingLobby/GS_MatchingLobby.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"
#include "proj_a/MatchingLobby/PC_MatchingLobby/PC_MatchingLobby.h"
#include "Serialization/BulkDataRegistry.h"

// Sets default values
ADPCharacter::ADPCharacter()
{
	bReplicates = true;
	bIsStunned = false;
	
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	hpComponent = CreateDefaultSubobject<UDPHpActorComponent>(TEXT("HPComponent"));
	constructionComponent = CreateDefaultSubobject<UDPConstructionActorComponent>(TEXT("ConstructionComponent"));
	weaponComponent = CreateDefaultSubobject<UDPWeaponActorComponent>(TEXT("WeaponComponent"));
	stateComponent = CreateDefaultSubobject<UDPStateActorComponent>(TEXT("StateComponent"));
	monsterSlotComponent = CreateDefaultSubobject<UMonsterSlotComponent>(TEXT("MonsterSlotComponent"));

	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	//sceneCaptureSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SCENECAPTURESPRINGARM"));
	//sceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SCENECAPTURE"));

	// tag
	Tags.Add(FName("player"));

	springArm->SetupAttachment(RootComponent);
	camera->SetupAttachment(springArm);

	//sceneCaptureSpringArm->SetupAttachment(RootComponent);
	//sceneCapture->SetupAttachment(sceneCaptureSpringArm);

	/* If initialize before the parent constructor, it may be ignored by Unreal Engine. please check USkeletalMeshComponent::SetSkeletalMesh */
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CHARACTER
	(TEXT("/Game/model/steve/StickManForMixamo.StickManForMixamo"));
	if (SK_CHARACTER.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CHARACTER.Object);
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, 270.f, 0.f));
	GetMesh()->SetRelativeScale3D(FVector(0.35f, 0.35f, 0.35f));

	springArm->TargetArmLength = 400.0f;
	springArm->bUsePawnControlRotation = true;
	camera->SetRelativeLocation(FVector(0.f, 50.f, 150.f));

	// player collision object type
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Player"));

	// minimap
	//static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RENDERTARGET
	//(TEXT("/Game/widget/minimap/rt_minimap.rt_minimap"));
	//if (RENDERTARGET.Succeeded()){
	//	sceneCapture->TextureTarget = RENDERTARGET.Object;
	//}

	//sceneCaptureSpringArm->TargetArmLength = 700.0f;
	//sceneCaptureSpringArm->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	//sceneCaptureSpringArm->bInheritPitch = false;
	//sceneCaptureSpringArm->bInheritYaw = false;
	//sceneCaptureSpringArm->bInheritRoll = false;
	//sceneCaptureSpringArm->bDoCollisionTest = false;
	//sceneCapture->ProjectionType = ECameraProjectionMode::Type::Orthographic;
	//sceneCapture->OrthoWidth = 1024.0f;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_CHARACTER
	(TEXT("/Game/animation/steve/steveAnimation.steveAnimation_C"));
	if (ANIM_CHARACTER.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(ANIM_CHARACTER.Class);
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> CHARACTER_MONTAGE
	(TEXT("/Game/animation/steve/characterAnimMontage.characterAnimMontage"));
	if (CHARACTER_MONTAGE.Succeeded()) {
		characterMontage = CHARACTER_MONTAGE.Object;
	}

	syncer = CreateDefaultSubobject<UCharacterPositionSync>(TEXT("My Syncer"));


	// disable move replication : set bReplicateMovement to false
	if (UWorld* World = GetWorld())
	{
		FString CurrentLevelName = World->GetMapName();
		if (CurrentLevelName.Contains("mainLevel"))
		{
			AActor::SetReplicatingMovement(false);
		}
		else
		{
			AActor::SetReplicatingMovement(true);
		}
	}
	// Set Mass and Collision Profile
	GetCharacterMovement()->Mass = 0.1f;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_PlayerChannel);
	GetMesh()->SetCollisionObjectType(ECC_PlayerChannel);

	// Enable hit events
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->CustomDepthStencilValue = 2;

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> CAMERASHAKE
	(TEXT("/Game/etc/bp_cameraShake.bp_cameraShake_C"));
	if (CAMERASHAKE.Succeeded()) {
		cameraShake = CAMERASHAKE.Class;
	}
	
	StunArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("StunArrow"));
	StunArrow->SetupAttachment(GetMesh(), FName("Stun_Pos"));
	StunArrow->SetRelativeLocation(FVector(0, -20.f, 0));
	StunArrow->SetHiddenInGame(true);
	
	StunEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StunEffectComponent"));
	StunEffectComponent->SetupAttachment(StunArrow);
	StunEffectComponent->SetAutoActivate(false);
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> STUN
	(TEXT("/Game/effect/ns_stun.ns_stun"));
	if (STUN.Succeeded()) {
		StunEffect = STUN.Object;
		StunEffectComponent->SetAsset(StunEffect);
	}

	static ConstructorHelpers::FClassFinder<UNameTag> NAME_TAG
	(TEXT("/Game/widget/widget_NameTag.widget_NameTag_C"));
	if (NAME_TAG.Succeeded())
	{
		NameTag_BP = NAME_TAG.Class;
		
		NameTag_WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameTag_WidgetComponent"));
		NameTag_WidgetComponent->SetupAttachment(RootComponent);
		NameTag_WidgetComponent->SetDrawAtDesiredSize(true);
		NameTag_WidgetComponent->SetRelativeLocation(FVector(0, 0, 100));
		NameTag_WidgetComponent->SetWorldScale3D(FVector(0.6f, 0.6f, 0.6f));
	}

	postProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("postProcessComponent"));
	postProcessComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> PostProcessMaterial
	(TEXT("MaterialInstanceConstant'/Game/material/postprocess/mpp_dizzle_Inst.mpp_dizzle_Inst'"));
	if (PostProcessMaterial.Succeeded())
	{
		postProcessComponent->AddOrUpdateBlendable(PostProcessMaterial.Object, 1.0f);
		postProcessComponent->BlendWeight = 0.0f;
	}

	if (UWorld* World = GetWorld())
	{
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World);
		
		if (CurrentLevelName == "matchLobby")
		{
			RemoveSpringArm();
			if (LobbyInfoWidgetComponent && LobbyInfoWidgetComponent->IsValidLowLevel())
			{
				LobbyInfoWidgetComponent->DestroyComponent();
				LobbyInfoWidgetComponent = nullptr;
			}
	
			LobbyInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LobbyInfoWidgetComponent"));
			static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/widget/WBP_MatchLobby/widget_LobbyInfo.widget_LobbyInfo_C"));
			if (WidgetClass.Succeeded())
			{
				LobbyInfoWidgetComponent->SetWidgetClass(WidgetClass.Class);
			}
			LobbyInfoWidgetComponent->SetVisibility(true);
			LobbyInfoWidgetComponent->SetWidgetSpace( EWidgetSpace::World);
			LobbyInfoWidgetComponent->SetupAttachment(GetMesh());
			LobbyInfoWidgetComponent->SetRelativeLocation(FVector(0, 0, 650));
			LobbyInfoWidgetComponent->SetRelativeScale3D(FVector(1.4f, 1.4f, 1.4f));
			LobbyInfoWidgetComponent->SetDrawSize(FVector2D(260,100));
			LobbyInfoWidgetComponent->SetRelativeRotation(FRotator(-180, -90, 180));
			// Adjust for back view
			LobbyInfoWidgetComponentBack = CreateDefaultSubobject<UWidgetComponent>(TEXT("LobbyInfoWidgetComponentBack"));
			if (WidgetClass.Succeeded())
			{
				LobbyInfoWidgetComponentBack->SetWidgetClass(WidgetClass.Class);
			}
			LobbyInfoWidgetComponentBack->SetVisibility(true);
			LobbyInfoWidgetComponentBack->SetWidgetSpace(EWidgetSpace::World);
			LobbyInfoWidgetComponentBack->SetupAttachment(GetMesh());
			LobbyInfoWidgetComponentBack->SetRelativeLocation(FVector(0, 0, 650));
			LobbyInfoWidgetComponentBack->SetRelativeScale3D(FVector(1.4f, 1.4f, 1.4f));
			LobbyInfoWidgetComponentBack->SetDrawSize(FVector2D(260, 100));
			LobbyInfoWidgetComponentBack->SetRelativeRotation(FRotator(0, -90, 0));

		}
	}

	// Crown
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CrownMesh
	(TEXT("/Game/model/objects/crown/crown1/crown_merged.crown_merged"));
	if (CrownMesh.Succeeded())
	{
		Crown = CrownMesh.Object;
	}

	GetCharacterMovement()->SetWalkableFloorAngle(80.f);
	GetCharacterMovement()->MaxStepHeight = 50.f;
}

ADPCharacter::~ADPCharacter()
{
}

void ADPCharacter::OnHostMigration(UWorld* World, UDataManager* DataManager)
{
	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->network_failure_manager_->OnHostMigration().Remove(OnHostMigrationDelegate);
	}
	UCharacterData* CharacterData = NewObject<UCharacterData>(DataManager, UCharacterData::StaticClass());
	if (CharacterData)
	{
		CharacterData->InitializeData();
		FString PlayerName = TEXT("");
		if (GetPlayerState())
		{
			PlayerName = GetPlayerState()->GetPlayerName();
		}
		CharacterData->SetActorName(PlayerName);
		CharacterData->SetActorLocation(GetActorLocation());
		CharacterData->SetActorRotation(GetActorRotation());

		TArray<EAnimal> CapturedAnimals = ReturnMonsters();
		for (EAnimal Animal : CapturedAnimals)
		{
			CharacterData->AddCapturedAnimal(Animal);
		}
		DataManager->AddDataToArray(TEXT("CharacterData"), CharacterData);
	}
}

void ADPCharacter::SetNameTag_Implementation()
{
	APlayerState* PS = GetPlayerState();
	if (!PS)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			SetNameTag();
		}), 0.1f, false);
		return ;
	}

	FString Name = PS->GetPlayerName();
	if (NameTag_Instance)
	{
		NameTag_Instance->SetName(Name);
	}
	if (NameTag_WidgetComponent && !IsLocallyControlled())
	{
		NameTag_WidgetComponent->SetVisibility(true);
	}
}

void ADPCharacter::UpdateLobbyInfo() 
{
	APlayerState* PS = GetPlayerState();
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			UpdateLobbyInfo();
		}), 0.1f, false);
		return;
	}
	
	AGS_MatchingLobby* GS = Cast<AGS_MatchingLobby>(GetWorld()->GetGameState());
	if (!PS || !GS)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			UpdateLobbyInfo();
		}), 0.1f, false);
		return ;
	}
	
	FString Name = PS->GetPlayerName();

	if (LobbyInfoWidgetComponent == nullptr || LobbyInfoWidgetComponentBack == nullptr)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			UpdateLobbyInfo();
		}), 0.1f, false);
		return ;
	}
	
	UUserWidget* Widget = LobbyInfoWidgetComponent->GetUserWidgetObject();
	UUserWidget* WidgetBack = LobbyInfoWidgetComponentBack->GetUserWidgetObject();
	if (Widget && WidgetBack)
	{
		int32 PlayerIndex = GS->FindIndexByPlayerId(PS->GetPlayerId());
		FString Command = FString::Printf(TEXT("Update %d"), PlayerIndex);
		Widget->CallFunctionByNameWithArguments(*Command, *GLog, nullptr, true);
		WidgetBack->CallFunctionByNameWithArguments(*Command, *GLog, nullptr, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("&*Widget is null for Character"));
	}
}

// void ADPCharacter::SetNameTag()
// {
// 	APlayerState* PS = GetPlayerState();
// 	if (!PS)
// 	{
// 		FTimerHandle TimerHandle;
// 		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
// 		{
// 			SetNameTag();
// 		}), 0.1f, false);
// 		return ;
// 	}
//
// 	FString Name = PS->GetPlayerName();
// 	if (NameTag_Instance)
// 	{
// 		NameTag_Instance->SetName(Name);
// 	}
// 	if (NameTag_WidgetComponent && !IsLocallyControlled())
// 	{
// 		NameTag_WidgetComponent->SetVisibility(true);
// 	}
// }

// Called when the game starts or when spawned
void ADPCharacter::BeginPlay()
{
	Super::BeginPlay();

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (!HasAuthority())
	{
		if (GameInstance)
		{
			OnHostMigrationDelegate = GameInstance->network_failure_manager_->OnHostMigration().AddUObject(this, &ADPCharacter::OnHostMigration);
		}
	}
	if (!NameTag_BP)
	{
		check(false);
	}
	NameTag_Instance = CreateWidget<UNameTag>(GetWorld(), NameTag_BP);
	if (NameTag_Instance)
	{
		NameTag_WidgetComponent->SetWidget(NameTag_Instance);
		NameTag_WidgetComponent->SetVisibility(false);
	}
	
	// GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ADPCharacter::TryInItializeDynamicTexture);
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AReturnTriggerVolume::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		ReturnTriggerVolume = Cast<AReturnTriggerVolume>(FoundActors[0]);
	}

	stateComponent->currentEquipmentState = 0;
	hpComponent->Hp = 100.f;
	hpComponent->IsDead = false;
	constructionComponent->placeWall = false;
	constructionComponent->placeturret = false;
	bUseControllerRotationYaw = false;
}

void ADPCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->network_failure_manager_->OnHostMigration().Remove(OnHostMigrationDelegate);
	}
}

// void ADPCharacter::TryInItializeDynamicTexture()
// {
// 	FNetworkedDynamicTextureComponentInitializer Initializer;
// 	Initializer.DynamicMaterialInstance = dynamicMaterialInstance;
// 	Initializer.SkeletalMeshComponent = GetMesh();
// 	APlayerController* PC = GetWorld()->GetFirstPlayerController();
// 	if (PC)
// 	{
// 		Initializer.PlayerState = GetPlayerState<APlayerState>();
// 		Initializer.TextureTransferManager = PC->FindComponentByClass<UTextureTransferManager>();
// 	}
// 	if (DynamicTextureComponent->InitializeTexture(Initializer))
// 		DynamicTextureComponent->LoadTexture();
// }

// Called every frame
void ADPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()) {
		currentVelocity = GetCharacterMovement()->Velocity;
		speed = currentVelocity.Size();
	}
	
	if (!IsLocallyControlled())
	{
		UpdateNameTagRotation();
	}

	if (UWorld* World = GetWorld())
	{
		FString CurrentLevelName = World->GetMapName();
		if (CurrentLevelName.Contains("mainLevel") == false)
		{
			return ;
		}
	}
	
	if (this->GetLocalRole() == ROLE_SimulatedProxy)
	{
		syncer->SyncWithServer(this);
		syncer->SyncGunFire(this);
		syncer->SyncReturnAnimal(this);
	}
	if (!HasAuthority())
	{
		syncer->SyncCatch(this);
	}

	if (bIsStunned)
	{
		// Rotate the arrow component
		FRotator NewRotation = StunArrow->GetRelativeRotation();
		NewRotation.Roll += DeltaTime * 180.0f; // Rotate 180 degrees per second
		StunArrow->SetRelativeRotation(NewRotation);
	}

	if (HasAuthority())
	{
		CheckCollisionWithMonster();
	}
}

void ADPCharacter::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);
	//FNetLogger::EditerLog(FColor::Red, TEXT("OnPlayerStateChanged"));
	if (NewPlayerState == nullptr)
	{
		return ;
	}

	UGI_Zoomies* GameInstance = Cast<UGI_Zoomies>(GetGameInstance());
	check(GameInstance);
	UDataManager* DataManager = GameInstance->network_failure_manager_->GetDataManager();
	check(DataManager);

	UDataArray* CharacterDataArray = DataManager->GetDataArray(TEXT("CharacterData"));
	if (!CharacterDataArray)
	{
		return ;
	}

	FString PlayerName = NewPlayerState->GetPlayerName();
	UCharacterData* MyCharacterData = nullptr;
	for (UBaseData* Data : CharacterDataArray->DataArray)
	{
		UCharacterData* SavedCharacterData = Cast<UCharacterData>(Data);
		if (SavedCharacterData && SavedCharacterData->GetActorName() == PlayerName)
		{
			MyCharacterData = SavedCharacterData;
			break;
		}
	}
	if (!MyCharacterData)
	{
		return ;
	}
	if (IsLocallyControlled())
	{
		this->SetActorLocation(MyCharacterData->GetActorLocation());
		this->SetActorRotation(MyCharacterData->GetActorRotation());
	}
	TArray<EAnimal> CapturedAnimals = MyCharacterData->GetCapturedAnimals();
	for (EAnimal Animal : CapturedAnimals)
	{
		monsterSlotComponent->AddMonsterToSlot(this, Animal);
	}
}

// Called to bind functionality to input
void ADPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ADPCharacter::PlayAimAnimation()
{
	if (characterMontage && !isAim ) {
		isAim = true;
		PlayAnimMontage(characterMontage, 1.f, "aim");

		springArm->TargetArmLength = 270.0f;
	}
}

void ADPCharacter::StopAimAnimation()
{
	if (characterMontage) {
		isAim = false;
		StopAnimMontage(characterMontage);

		springArm->TargetArmLength = 400.0f;
	}
}

void ADPCharacter::PlayFireAnimation()
{
	if (characterMontage) {
		PlayAnimMontage(characterMontage, 1.f, "fire");
	}

	if (camera && cameraShake) {
		FVector cameraLocation = camera->GetComponentLocation();
		UGameplayStatics::PlayWorldCameraShake(this, cameraShake, cameraLocation, 0.0f, 500.0f);
	}
}

void ADPCharacter::ChangeAnimation()
{
	if (characterMontage) {
		PlayAnimMontage(characterMontage, 1.f, "changeWeapon");
	}
}

void ADPCharacter::PlaceConstructionAnimation()
{
}

void ADPCharacter::DestroyConstructionAnimation()
{
}

void ADPCharacter::DyingAnimation()
{
}

bool ADPCharacter::CatchMonster(const FString& monster_type)
{
	return monsterSlotComponent->AddMonsterToSlot(this, monster_type);
}

void ADPCharacter::SetAtReturnPlace(bool isReturnPlace)
{
	this->mIsAtReturnPlace = isReturnPlace;
}

bool ADPCharacter::IsAtReturnPlace() const
{
	return this->mIsAtReturnPlace;
}

void ADPCharacter::RemoveSpringArm()
{
	camera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void ADPCharacter::OnRep_SyncRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? 1000.0f : 600.0f;
}

void ADPCharacter::SetStunned(bool bCond)
{
	this->bIsStunned = bCond;
}

bool ADPCharacter::IsStunned() const
{
	return this->bIsStunned;
}

void ADPCharacter::ClientNotifyAnimalReturn_Implementation(const FString& player_name)
{
	FDataHub::PushReturnAnimalDA(player_name, true);
}

// void ADPCharacter::OnRep_PlayerState()
// {
// 	Super::OnRep_PlayerState();
//
// 	TryInItializeDynamicTexture();
// }
//
// void ADPCharacter::OnRep_Controller()
// {
// 	Super::OnRep_Controller();
//
// 	TryInItializeDynamicTexture();
// }

void ADPCharacter::UpdateNameTagRotation()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return ;
	}
	if (NameTag_WidgetComponent)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			// FNetLogger::LogError(TEXT("Here is Wrong! : PlayerController"));
			if (!PlayerController->PlayerCameraManager)
			{
				return ;
			}
			FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
			FRotator NewRotation = (CameraLocation - NameTag_WidgetComponent->GetComponentLocation()).Rotation();
           
			NewRotation.Pitch = 0.0f;
			NewRotation.Roll = 0.0f;
            
			NameTag_WidgetComponent->SetWorldRotation(NewRotation);
		}
	}
}

void ADPCharacter::CheckCollisionWithMonster()
{
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<FVector> Direction = {
		FVector::ForwardVector,
		-FVector::UpVector,
	};

	for (int i = 0; i < Direction.Num(); i++)
	{
		if (GetWorld()->SweepSingleByChannel(
			HitResult,
			GetActorLocation(),
			GetActorLocation() + Direction[i] * 50.f,
			GetActorRotation().Quaternion(),
			ECC_MonsterChannel,
			GetCapsuleComponent()->GetCollisionShape(),
			QueryParams
		))
		{
			if (ABaseMonsterCharacter* MC = Cast<ABaseMonsterCharacter>(HitResult.GetActor()))
			{
				/* Only ChasePlayerMonsterAIController can hit the player */
				AChasePlayerMonsterAIController* HC = Cast<AChasePlayerMonsterAIController>(MC->GetController());
				if (!HC)
					continue;
				
				ACharacter* TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), HC->GetPlayerIndex());
				if (TargetPlayer && TargetPlayer == this)
				{
					/* Monster collided with its current target player; initiate search for a new player target */
					HC->SetRandomPlayerIndex();
				}
				OnServerHit(HitResult);
			}
		}
	}
}

TArray<EAnimal> ADPCharacter::ReturnMonsters()
{
	return monsterSlotComponent->RemoveMonstersFromSlot();
}

void ADPCharacter::CharacterRun_Implementation()
{
	bIsRunning = true;
	OnRep_SyncRunning();
}

void ADPCharacter::CharacterRunReleased_Implementation()
{
	bIsRunning = false;
	OnRep_SyncRunning();
}

void ADPCharacter::ApplyStunEffect()
{
	if (!bIsStunned)
	{
		bIsStunned = true;
		StunEffectComponent->Activate(true);
	}
	if (!HasAuthority())
	{
		StunEffectComponent->Activate(true);
	}
	if (IsLocallyControlled()) {
		postProcessComponent->BlendWeight = 1.f;
	}
}

void ADPCharacter::RemoveStunEffect()
{
	if (!IsValid(this)) return;
	if (HasAuthority())
	{
		bIsStunned = false;
		bIsInvincible = true;
		FTimerHandle TimerInvincibleHandle;
		FTimerDelegate TimerInvincibleDelegate;
		TWeakObjectPtr<ADPCharacter> WeakThis(this);
		TimerInvincibleDelegate.BindLambda([WeakThis, this]()
		{
			if (WeakThis.IsValid())
			{
				bIsInvincible = false;
			}
		});
		GetWorld()->GetTimerManager().SetTimer(TimerInvincibleHandle, TimerInvincibleDelegate, 2.0f, false);
	}
	StunEffectComponent->Deactivate();
	// TODO: Invincible Effect
	if (IsLocallyControlled()) {
		postProcessComponent->BlendWeight = 0.f;
	}
}

void ADPCharacter::ApplyKockback_Implementation(const FHitResult& HitResult)
{
	// �浹 �������� ĳ���� ��ġ���� ������ ���
	FVector KnockbackDirection = GetActorLocation() - HitResult.ImpactPoint;
	// FVector KnockbackDirection = -HitResult.ImpactNormal;
	KnockbackDirection.Z = 20.0f;
    
	if (!KnockbackDirection.IsNearlyZero())
	{
		KnockbackDirection.Normalize();
	}
	else
	{
		KnockbackDirection = GetActorForwardVector() * -1;
	}

	// �˹� �ӵ� ����
	float KnockbackSpeed = 2000.0f;

	// Character Movement Component ��������
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		// Save previous movement mode
		EMovementMode PreviousMovementMode = MovementComponent->MovementMode;

		// Set Falling Movement Mode
		MovementComponent->SetMovementMode(MOVE_Falling);

		// Set Knockback Velocity
		MovementComponent->Velocity = KnockbackDirection * KnockbackSpeed;

		FTimerHandle ResetTimerHandle;
		TWeakObjectPtr<ADPCharacter> WeakThis(this);
		isKnockback = true;
		GetWorldTimerManager().SetTimer(ResetTimerHandle, [WeakThis, this, MovementComponent, PreviousMovementMode]() {
			if (WeakThis.IsValid())
			{
				// Reset Movement Mode
				MovementComponent->SetMovementMode(PreviousMovementMode);
				isKnockback = false;
			}
		}, 0.5f, false);
	}
}

void ADPCharacter::OnRep_SyncInvincible()
{
}

bool ADPCharacter::IsInvincible()
{
	return bIsInvincible;
}

void ADPCharacter::OnServerHit(const FHitResult& HitResult)
{
	if (this->IsStunned())
	{
		return ;
	}

	if (this->IsInvincible())
	{
		return ;
	}
	
	ABaseMonsterCharacter* monster = Cast<ABaseMonsterCharacter>(HitResult.GetActor());
	if (!monster)
	{
		return ;
	}
	
	if (monster->GetState() == EMonsterState::Faint)
	{
		return ;
	}
	this->ApplyKockback(HitResult);
	this->ApplyStunEffect();
	
	FTimerDelegate timerCollisionDelegate;

	TWeakObjectPtr<ADPCharacter> WeakThis(this);
	timerCollisionDelegate.BindLambda([WeakThis, this]()
	{
		if (WeakThis.IsValid())
		{
			this->RemoveStunEffect();
		}
	});
	float stunTime = 1.5f;
	GetWorld()->GetTimerManager().SetTimer(timerCollisionHandle, timerCollisionDelegate, stunTime, false);
}

void ADPCharacter::OnRep_SyncStunned()
{
	if (bIsStunned)
	{
		this->ApplyStunEffect();
	}
	else
	{
		this->RemoveStunEffect();
	}
}

FVector ADPCharacter::GetCameraLocation() const
{
	FVector cameraLocation = camera->GetComponentLocation();
	return cameraLocation;
}

void ADPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADPCharacter, bIsStunned);
	DOREPLIFETIME(ADPCharacter, bIsRunning);
}

void ADPCharacter::SetCrownMesh_Implementation()
{
	UStaticMeshComponent* CrownMeshComponent = NewObject<UStaticMeshComponent>(this);
	
	CrownMeshComponent->SetStaticMesh(Crown);
	CrownMeshComponent->SetRelativeLocation(FVector(0, 0, 100));
	CrownMeshComponent->SetRelativeRotation(FRotator(0, -90, -90));
	CrownMeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	// CrownMeshComponent->SetupAttachment(RootComponent);

	CrownMeshComponent->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("MonsterSlot_0"));
	CrownMeshComponent->RegisterComponent();
	CrownMeshComponent->SetVisibility(true);
}