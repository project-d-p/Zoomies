#include "BB_Monster.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBB_Monster::UBB_Monster()
{
	FBlackboardEntry Entry;
	Entry.EntryName = "SafeLocation";
    Entry.KeyType = CreateDefaultSubobject<UBlackboardKeyType_Vector>(TEXT("SafeLocationKeyType"));
	Keys.Add(Entry);

	// FBlackboardEntry AIStateEntry;
	// AIStateEntry.EntryName = "AIState";
	// UBlackboardKeyType_Enum* EnumKeyType = CreateDefaultSubobject<UBlackboardKeyType_Enum>(TEXT("AIStateKeyType"));
	// EnumKeyType->EnumName = FName(TEXT("EAIState"));
	// EnumKeyType->EnumType = StaticEnum<EAIState>();
	// AIStateEntry.KeyType = EnumKeyType;
	// Keys.Add(AIStateEntry);
}
