// Fill out your copyright notice in the Description page of Project Settings.

#include "Marshaller.h"

TArray<uint8> Marshaller::SerializeMessage(const Message& msg)
{
	uint32_t message_size = msg.ByteSizeLong();

	TArray<uint8> buffer;
	buffer.AddUninitialized(sizeof(message_size) + message_size);

	memcpy(buffer.GetData(), &message_size, sizeof(message_size));

	msg.SerializeToArray(buffer.GetData() + sizeof(message_size), message_size);

	return buffer;
}

Message Marshaller::DeserializeMessage(const TArray<uint8>& buffer)
{
	uint32_t message_size;
	memcpy(&message_size, buffer.GetData(), sizeof(message_size));

	Message msg;
	msg.ParseFromArray(buffer.GetData() + sizeof(message_size), message_size);

	return msg;
}
