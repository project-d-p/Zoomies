#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <iostream>

class DelayedExecutionSystem {
public:
    DelayedExecutionSystem() = default;
    ~DelayedExecutionSystem() = default;
    
    template<typename Func, typename... Args>
    void EnqueueExecution(Func&& func, Args&&... args)
    {
        DelayedExecution delayedCall;
        delayedCall.Function = [func = std::forward<Func>(func), ...capturedArgs = std::forward<Args>(args)]() mutable {
            try {
                std::invoke(func, std::forward<Args>(capturedArgs)...);
            } catch (const std::exception& e) {
                std::cerr << "Exception occurred during function execution: " << e.what() << std::endl;
            }
        };
        DelayedExecutionQueue.push_back(std::move(delayedCall));
    }

    template<typename Class, typename Func, typename... Args>
    void EnqueueExecution(std::shared_ptr<Class> obj, Func&& func, Args&&... args)
    {
        DelayedExecution delayedCall;
        delayedCall.Function = [weakObj = std::weak_ptr<Class>(obj), func = std::forward<Func>(func), ...capturedArgs = std::forward<Args>(args)]() mutable {
            if (auto sharedObj = weakObj.lock()) {
                try {
                    (sharedObj.get()->*func)(std::forward<Args>(capturedArgs)...);
                } catch (const std::exception& e) {
                    std::cerr << "Exception occurred during member function execution: " << e.what() << std::endl;
                }
            } else {
                std::cerr << "Object is no longer valid." << std::endl;
            }
        };
        DelayedExecutionQueue.push_back(std::move(delayedCall));
    }

    void ExecuteQueuedFunctions()
    {
        for (auto& delayedCall : DelayedExecutionQueue)
        {
            try {
                delayedCall.Execute();
            } catch (const std::exception& e) {
                std::cerr << "Exception occurred during delayed execution: " << e.what() << std::endl;
            }
        }
        DelayedExecutionQueue.clear();
    }

private:
    struct DelayedExecution {
        std::function<void()> Function;
        void Execute() const { Function(); }
    };
    
    std::vector<DelayedExecution> DelayedExecutionQueue;
};
