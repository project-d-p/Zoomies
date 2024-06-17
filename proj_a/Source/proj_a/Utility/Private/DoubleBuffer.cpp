#include "DoubleBuffer.h"

DoubleBuffer::DoubleBuffer()
{
	ptr_[READ] = &first_buffer_;
	ptr_[WRITE] = &second_buffer_;
}

DoubleBuffer::~DoubleBuffer()
{
	{
		std::queue<Message> empty;
		std::swap(*(ptr_[READ]), empty);
	}
	{
		std::queue<Message> empty;
		std::swap(*(ptr_[WRITE]), empty);
	}
}

void DoubleBuffer::Push(const Message& message)
{
	std::lock_guard<std::mutex> lock(mutex_);
	ptr_[WRITE]->push(message);
}

Message DoubleBuffer::Pop()
{
	Message ret = ptr_[READ]->front();
	ptr_[READ]->pop();
	return ret;
}

void DoubleBuffer::Swap()
{
	std::lock_guard<std::mutex> lock(mutex_);
	std::swap(this->ptr_[0], this->ptr_[1]);
	std::queue<Message> empty;
	std::swap(*(this->ptr_[WRITE]), empty);
}

int DoubleBuffer::GetSizeOfReadBuffer() const
{
	return ptr_[READ]->size();
}

bool DoubleBuffer::Empty() const
{
	return ptr_[READ]->empty();
}

DoubleBuffer::DoubleBuffer(DoubleBuffer&& other) noexcept
{
	std::lock_guard<std::mutex> lock(other.mutex_);
	ptr_[READ] = std::move(other.ptr_[READ]);
	ptr_[WRITE] = std::move(other.ptr_[WRITE]);
	first_buffer_ = std::move(other.first_buffer_);
	second_buffer_ = std::move(other.second_buffer_);
}

DoubleBuffer& DoubleBuffer::operator=(DoubleBuffer&& other) noexcept
{
	if (this != &other) {
		std::lock_guard<std::mutex> lock(mutex_);
		std::lock_guard<std::mutex> lockOther(other.mutex_);
		ptr_[READ] = std::move(other.ptr_[READ]);
		ptr_[WRITE] = std::move(other.ptr_[WRITE]);
		first_buffer_ = std::move(other.first_buffer_);
		second_buffer_ = std::move(other.second_buffer_);
	}
	return *this;
}

std::queue<Message> DoubleBuffer::GetReadBuffer()
{
	// std::lock_guard<std::mutex> lock(mutex_);
	return *ptr_[READ];
}