#pragma once

#include "CoreMinimal.h"
#include "DPCharacter.h"
#include "GameFramework/Character.h"
#include "BaseMonsterCharacter.generated.h"

UCLASS()
class PROJ_A_API ABaseMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseMonsterCharacter();
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	float MaxHp = 100.f;
	float MoveSpeed;
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	void SyncPosition();
	virtual void Tick(float DeltaTime) override;
	
	bool bCaught = false;
};
