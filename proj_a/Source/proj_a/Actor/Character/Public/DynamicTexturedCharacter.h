#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NetworkedDynamicTextureComponent.h"
#include "DynamicTexturedCharacter.generated.h"

UCLASS()
class ADynamicTexturedCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	ADynamicTexturedCharacter();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInstanceDynamic* dynamicMaterialInstance;
	UFUNCTION(BlueprintCallable, Category = "DynamicTexture")
	UNetworkedDynamicTextureComponent* GetDynamicTextureComponent() const { return NetworkedDynamicTexture; }
	UPROPERTY(EditAnywhere, Category = "DynamicTexture")
	bool bPlayerAssigned = false;
protected:
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_Owner() override;
private:
	bool bInitialized = false;
	FTimerHandle TimerHandle_InitializeDynamicTexture;
	
	void TryInItializeDynamicTexture();
	UPROPERTY(VisibleAnywhere, Category = "DynamicTexture")
	UNetworkedDynamicTextureComponent* NetworkedDynamicTexture;
};
