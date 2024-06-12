#include "MessageQueueFilter.h"

bool MessageQueueFilter::operator()(const Message& lhs, const Message& rhs) const
{
	return lhs.timestamp() < rhs.timestamp();
}
