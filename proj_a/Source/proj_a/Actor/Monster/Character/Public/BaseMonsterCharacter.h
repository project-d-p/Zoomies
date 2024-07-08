#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseMonsterCharacter.generated.h"

enum EMonsterState
{
	Idle,
	Faint,
};

UCLASS()
class PROJ_A_API ABaseMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseMonsterCharacter();
	virtual ~ABaseMonsterCharacter() override;
	
	void TakeDamage(float Dmg);
	void ScaleCapsuleSize(float ScaleFactor);
	EMonsterState GetState() const { return CurrentState; } 
	
	UPROPERTY(Replicated)
	int32 MonsterId;
private:
	const float MaxHp = 100.f;
	float CurrentHp = 100.f;
	EMonsterState CurrentState = Idle;
	float MoveSpeed;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SyncPosition();
	bool bCaught = false;
};
