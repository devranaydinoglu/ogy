#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include "SafeQueue.h"
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

class ThreadPool
{
private:
    int numThreads = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1;
    std::vector<std::thread> threads;
    std::atomic<bool> enabled = true;
    SafeQueue<std::function<void()>> tasks;
    std::atomic<int> numPendingTasks = 0;

public:
    ThreadPool()
    {
        try
        {
            for (uint8_t i = 0; i < numThreads; i++)
            {
                threads.emplace_back(std::thread(&ThreadPool::worker, this));
            }
        }
        catch(const std::exception& e)
        {
            enabled = false;
            joinThreads();
            std::cerr << e.what() << '\n';
        }
    }

    ~ThreadPool()
    {
        enabled = false;
        tasks.close();
        joinThreads();
    }

    template<typename Func, typename... Args>
    auto addTask(Func&& f, Args&&... args)
    {
        using ReturnType = typename std::invoke_result_t<Func>;
        
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
        std::future<ReturnType> resultFuture = task->get_future();

        tasks.enqueue([task](){(*task)();});
        numPendingTasks++;

        return resultFuture;
    }

private:
    void worker()
    {
        while (enabled)
        {
            std::function<void()> task;

            auto taskOptional = tasks.dequeue();
            if (!taskOptional.has_value())
                continue;
            
            task = taskOptional.value();
            task();
            numPendingTasks--;
        }
    }

    void joinThreads()
    {
        for (int i = 0; i < threads.size(); i++)
        {
            threads.at(i).join();
        }

        threads.clear();
    }
};
