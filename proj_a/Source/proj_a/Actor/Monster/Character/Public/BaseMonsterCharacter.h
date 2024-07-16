#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PathManager.h"
#include "BaseMonsterCharacter.generated.h"

UENUM()
enum class EMonsterState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Faint UMETA(DisplayName = "Faint"),
};

struct FCapsuleProperties
{
	float Radius = 150.f;
	float HalfHeight = 100.f;
};

UCLASS()
class PROJ_A_API ABaseMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseMonsterCharacter();
	virtual ~ABaseMonsterCharacter() override;
	
	void TakeMonsterDamage(float Dmg);
	UFUNCTION()
	void OnRep_FaintCharacterMotion();
	void ScaleCapsuleSize(float ScaleFactor);
	EMonsterState GetState() const { return CurrentState; }
	void SetCatchable(bool bCond);

	UPROPERTY(Replicated)
	int32 MonsterId;

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* arrowSparkle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "effects")
	class UNiagaraSystem* sparkleEffect;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetComponent* widgetComponent;
	
	FCapsuleProperties DefaultCP;
	FCapsuleProperties FaintCP;
private:
	const float MaxHp = 100.f;
	float CurrentHp = 100.f;

	UPROPERTY(ReplicatedUsing=OnRep_FaintCharacterMotion)
	EMonsterState CurrentState = EMonsterState::Idle;

	float MoveSpeed;
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SyncPosition();
	bool bCaught = false;

	FTransform MeshAdjMtx;
	FTransform FaintStateMtx;
	FTransform CB_FaintStateMtx;
};
