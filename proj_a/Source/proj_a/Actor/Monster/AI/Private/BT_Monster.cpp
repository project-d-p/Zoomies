#include "BT_Monster.h"

#include "BB_Monster.h"
#include "BehaviorTree/Composites/BTComposite_Sequence.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"

UBT_Monster::UBT_Monster()
{
	BlackboardAsset = CreateDefaultSubobject<UBB_Monster>(TEXT("BlackboardData"));

	UBTTask_MoveTo* MoveToTask = CreateDefaultSubobject<UBTTask_MoveTo>(TEXT("MoveToTask"));
	MoveToTask->GetSelectedBlackboardKey() = "SafeLocation";

	RootNode = CreateDefaultSubobject<UBTComposite_Sequence>(TEXT("RootSequence"));
	FBTCompositeChild CompositeChild;
	CompositeChild.ChildTask = MoveToTask;
	RootNode->Children.Add(CompositeChild);
}
