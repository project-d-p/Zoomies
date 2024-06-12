#pragma once

#include "message.pb.h"
#include <queue>
#include <mutex>

class DoubleBuffer
{
public:
	// consturctor, destructor, copy constructor, copy assignment operator, move constructor, move assignment operator
	DoubleBuffer();
	DoubleBuffer(const DoubleBuffer&) = delete;
	DoubleBuffer& operator=(const DoubleBuffer&) = delete;
	DoubleBuffer(DoubleBuffer&&) noexcept;
	DoubleBuffer& operator=(DoubleBuffer&&) noexcept;
	~DoubleBuffer();

	// member functions
	Message Pop();
	void Push(const Message& message);
	void Swap();
	bool Empty() const;
	int GetSizeOfReadBuffer() const;
	std::queue<Message> GetReadBuffer();
private:
	enum { READ = 0, WRITE = 1 };
	std::queue<Message>* ptr_[2];
	alignas(64) std::queue<Message> first_buffer_;
	alignas(64) std::queue<Message> second_buffer_;
	std::mutex mutex_;
};
