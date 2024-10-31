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
	UFUNCTION(Client, Reliable)
	void Client_SetOwner(APlayerController* NewOwner);
	FTimerHandle TimerHandle_InitializeDynamicTexture;
protected:
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_Owner() override;
private:
	bool bInitialized = false;
	
	void TryInItializeDynamicTexture();
	UPROPERTY(VisibleAnywhere, Category = "DynamicTexture")
	UNetworkedDynamicTextureComponent* NetworkedDynamicTexture;
};
