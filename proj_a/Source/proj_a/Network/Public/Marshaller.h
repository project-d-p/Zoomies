// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/BufferArchive.h"

/**
 * 
 */

class Marshaller
{
public:
	template <typename T>
	static TArray<uint8> serializeData(const T& data)
	{
		FBufferArchive archive;

		// for (auto d : data)
		// 	archive << d;

		TArray<uint8> outData;
		outData.Append(archive.GetData(), archive.Num());
		archive.FlushCache();
		archive.Empty();

		return outData;
	}

	template <typename T>
	static T deserializeData(const TArray<uint8>& inData)
	{
		T data;
		FMemoryReader Reader(inData);

		// for (auto d : data)
		// 	Reader << d;

		Reader.FlushCache();
		Reader.Close();

		return data;
	}

	template <typename T>
	static TArray<uint8> protoSerializeData(const T& data)
	{
		UE_LOG(LogTemp, Warning, TEXT("Somone called protoSerializeData"));

		std::string buffer;
		data.SerializeToString(&buffer);
		
		TArray<uint8> OutData;
		OutData.Append(reinterpret_cast<const uint8*>(buffer.data()), buffer.size());
		
		return OutData;
	}

	template <typename T>
	static bool protoDeserializeData(const TArray<uint8>& inData, T& outData)
	{
		return outData.ParseFromArray(inData.GetData(), inData.Num());
	}
};
