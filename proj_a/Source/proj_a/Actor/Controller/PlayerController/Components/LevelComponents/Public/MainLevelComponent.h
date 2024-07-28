#pragma once

#include "CoreMinimal.h"
#include "ANetworkManager.h"
#include "BaseLevelComponent.h"
#include "message.pb.h"
#include "SteamNetworkingSocket.h"
#include "MainLevelComponent.generated.h"

class UPrivateScoreManager;
class ABaseMonsterCharacter;
class UDPStateActorComponent;

UCLASS()
class PROJ_A_API UMainLevelComponent : public UBaseLevelComponent
{
	GENERATED_BODY()
public:
	UMainLevelComponent();

	/* Activate and Deactivate this Component */
	virtual void Activate(bool bReset=false) override;
	virtual void Deactivate() override;

	/* Set & Get Character State Component */
	void SetStateComponent();
	UDPStateActorComponent* GetStateComponent() const;

	/* Add Message to Gun Queue */
	void AddGunMessage(const Message& message);
	/* Add Message to Catch Queue */
	void AddCatchMessage(const Message& message);
	/* Add Message to Aim Queue */
	void AddAimMessage(const Message& message);

	/* Get Current Target[Catchable Monster] */
	ABaseMonsterCharacter* GetCurrentTarget() const;
	
	/* Server RPC Called by Client */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNotifyReturnAnimals();

	/* Handle Message From Client Only Execute in Server */
	void HandlePosition(const ActorPosition& ActorPosition);

	/*
	 * TEST : COMMENT
	 */
	/* Simulate Gun Fire */
	// void SimulateGunFire(SteamNetworkingSocket* SteamSocket);
	void SimulateGunFire(UANetworkManager* NetworkManager); // TEST
	/* Simulate Catch */
	// void SimulateCatch(SteamNetworkingSocket* SteamSocket);
	void SimulateCatch(UANetworkManager* NetworkManager); // TEST
	/* Simulate Aim */
	// void SimulateAim(SteamNetworkingSocket* SteamSocket);
	void SimulateAim(UANetworkManager* NetworkManager); // TEST
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* Implement ServerNotifyReturnAnimals */
	void ServerNotifyReturnAnimals_Implementation();
	bool ServerNotifyReturnAnimals_Validate();

private:
	/* Called By TickComponent */
	bool IsCatchable(FHitResult& HitResult);
	void ChangeMonsterCatchable(const FHitResult& HitResult);

	/* Used to Set State, Rotation, Position */
	void SetState(const ActorPosition& ActorPosition);
	void SetRotation(const ActorPosition& ActorPosition);
	void SetPosition(const ActorPosition& ActorPosition);

	/* Not Used */
	// void HandleMovement(const Movement& movement, const float& server_delta);
	// void HandleJump(const Jump& Jump);
	
	UPROPERTY()
	UDPStateActorComponent* StateComponent;
	UPROPERTY()
	class UHitScan* CatchRay = nullptr;

	UPROPERTY()
	ABaseMonsterCharacter* CurrentTarget;
	
	std::queue<Message> GunQueue;
	std::queue<Message> CatchQueue;
	std::queue<Message> AimQueue;
};
