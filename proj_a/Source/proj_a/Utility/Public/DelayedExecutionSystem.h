#pragma once

#include <functional>
#include <vector>
#include <memory>

class DelayedExecutionSystem {
public:
	DelayedExecutionSystem() = default;
	~DelayedExecutionSystem() = default;
	
	template<typename Func, typename... Args>
	void EnqueueExecution(Func&& func, Args&&... args)
	{
		DelayedExecution delayedCall;
		delayedCall.Function = [func = std::forward<Func>(func), ... capturedArgs = std::forward<Args>(args)]() mutable {
			std::invoke(func, std::forward<Args>(capturedArgs)...);
		};
		DelayedExecutionQueue.push_back(std::move(delayedCall));
	}
    
	template<typename Class, typename Func, typename... Args>
	void EnqueueExecution(Class* obj, Func&& func, Args&&... args)
	{
		DelayedExecution delayedCall;
		delayedCall.Function = [obj, func = std::forward<Func>(func), ... capturedArgs = std::forward<Args>(args)]() mutable {
			(obj->*func)(std::forward<Args>(capturedArgs)...);
		};
		DelayedExecutionQueue.push_back(std::move(delayedCall));
	}
    
	void ExecuteQueuedFunctions()
	{
		for (auto& delayedCall : DelayedExecutionQueue)
			delayedCall.Execute();
	}

private:
	struct DelayedExecution {
		std::function<void()> Function;
		void Execute() const { Function(); }
	};
    
	std::vector<DelayedExecution> DelayedExecutionQueue;
};