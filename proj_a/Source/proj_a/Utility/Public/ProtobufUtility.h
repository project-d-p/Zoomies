#pragma once

#include "vector.pb.h"

class ProtobufUtility
{
public:
	static Vec3* ConvertToFVecToVec3(const FVector& InVector);
};
