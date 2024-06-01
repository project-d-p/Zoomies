#pragma once

#include "message.pb.h"
#include <queue>
#include <mutex>

class DoubleBuffer
{
public:
	DoubleBuffer();
	DoubleBuffer(const DoubleBuffer&) = delete;
	DoubleBuffer& operator=(const DoubleBuffer&) = delete;
	DoubleBuffer(DoubleBuffer&&) noexcept;
	DoubleBuffer& operator=(DoubleBuffer&&) noexcept;
	void Push(Message& message);
	Message Pop();
	void Swap();
	int GetSizeOfReadBuffer() const;
	bool Empty() const;
	~DoubleBuffer();
private:
	enum { READ = 0, WRITE = 1 };
	std::queue<Message>* ptr_[2];
	alignas(64) std::queue<Message> first_buffer_;
	alignas(64) std::queue<Message> second_buffer_;
	std::mutex mutex_;
};
