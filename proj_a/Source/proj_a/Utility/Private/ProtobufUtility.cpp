#include "ProtobufUtility.h"

Movement ProtoData;

Vec3* ProtobufUtility::ConvertToFVecToVec3(const FVector& InVector)
{
	Vec3* OutVec = new Vec3();
	OutVec->set_x(InVector.X);
	OutVec->set_y(InVector.Y);
	OutVec->set_z(InVector.Z);
	return OutVec;
}

void ProtobufUtility::ClearProtoData()
{
	ProtoData.Clear();
}
