// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "message.pb.h"

/**
 * 
 */

class Marshaller
{
public:
	static TArray<uint8> SerializeMessage(const Message& msg);
	static Message DeserializeMessage(const TArray<uint8>& buffer);
};
