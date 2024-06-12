#pragma once

#include "message.pb.h"

struct MessageQueueFilter
{
	bool operator()(const Message& lhs, const Message& rhs) const;
};
