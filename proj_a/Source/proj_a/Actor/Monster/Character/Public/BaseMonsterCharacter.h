#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "BaseMonsterCharacter.generated.h"

UCLASS()
class PROJ_A_API ABaseMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseMonsterCharacter();

	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	float MaxHp;
	float MoveSpeed;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* characterMontage;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
};
