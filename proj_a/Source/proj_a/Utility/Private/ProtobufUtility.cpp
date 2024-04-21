#include "ProtobufUtility.h"

Movement ProtoData;

Vec3 ConvertToFVecToVec3(const FVector& InVector)
{
	Vec3 OutVec;
	OutVec.set_x(InVector.X);
	OutVec.set_y(InVector.Y);
	OutVec.set_z(InVector.Z);
	return OutVec;
}

void ClearProtoData()
{
	ProtoData.Clear();
}
