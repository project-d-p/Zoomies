#pragma once

#include "vector.pb.h"
#include "movement.pb.h"

extern Movement ProtoData;

class ProtobufUtility
{
public:
	static Vec3* ConvertToFVecToVec3(const FVector& InVector);
	void ClearProtoData();
};
