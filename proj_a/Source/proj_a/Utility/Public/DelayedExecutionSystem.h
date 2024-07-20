#pragma once

#include <functional>
#include <vector>
#include <memory>

class DelayedExecutionSystem {
public:
	DelayedExecutionSystem() = default;
	~DelayedExecutionSystem()
	{
		DelayedExecutionQueue.clear();
	}
	
	template<typename Func, typename... Args>
	void EnqueueExecution(Func&& func, Args&&... args)
	{
		DelayedExecution delayedCall;
		delayedCall.Function =
			[func = std::forward<Func>(func), ... capturedArgs = std::forward<Args>(args)]() mutable {
			func(std::forward<Args>(capturedArgs)...);
		};
		DelayedExecutionQueue.push_back(std::move(delayedCall));
	}
	
	void ExecuteQueuedFunctions()
	{
		for (auto& delayedCall : DelayedExecutionQueue)
			delayedCall.Execute();
		DelayedExecutionQueue.clear();
	}

private:
	struct DelayedExecution {
		std::function<void()> Function;
		void Execute() { Function(); }
	};
	
	std::vector<DelayedExecution> DelayedExecutionQueue;
};