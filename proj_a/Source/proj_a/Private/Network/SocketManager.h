// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SocketManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJ_A_API USocketManager : public UObject
{
	GENERATED_BODY()
public:
	static USocketManager* getInstance();
	
	USocketManager();
	bool connect(const FString& iP, int32 port);
	int32 send(const uint8* data, int32 count) const;
	int32 receive(uint8& outData, int32 bufferSize) const;
	void close() const;

private:
	static USocketManager* Instance;
	FSocket* sock_;
	// FSocket* chatSock_;
	ISocketSubsystem* socketSubSystem_;
};
