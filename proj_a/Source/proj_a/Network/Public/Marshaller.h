// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "message.pb.h"
#include "Serialization/BufferArchive.h"

/**
 * 
 */

class Marshaller
{
public:
	template <typename T>
	static TArray<uint8> serializeData(const Movement& data)
	{
		FBufferArchive archive;

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

	static TArray<uint8> SerializeMovement(const Message& movement) {
		// 데이터를 바이트 배열로 직렬화
		uint32_t message_size = movement.ByteSizeLong();

		TArray<uint8> buffer; // uint8의 TArray 생성
		buffer.AddUninitialized(sizeof(message_size) + message_size);

		memcpy(buffer.GetData(), &message_size, sizeof(message_size));
		
		movement.SerializeToArray(buffer.GetData() + sizeof(message_size), message_size);
		
		return buffer;
	}
	
	template <typename T>
	static TArray<uint8> protoSerializeData(const T& data)
	{
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
