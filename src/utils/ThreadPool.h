#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "../commands/Command.h"

using Path = std::filesystem::path;

class ThreadPool
{
private:
    uint8_t numThreads = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1;
    std::atomic_bool enabled = true;
    std::queue<std::function<void()>> tasks;
    std::queue<std::thread> threads;
    std::mutex tasksMutex;
    
public:
    ThreadPool()
    {
        try
        {
            for (uint8_t i = 0; i < numThreads; i++)
            {
                threads.push(std::thread(&ThreadPool::worker, this));
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
        joinThreads();
    }

    void addTask(std::function<void()> task)
    {
        std::lock_guard<std::mutex> lg(tasksMutex);
        tasks.push(task);
    }

private:
    void worker()
    {
        while (enabled)
        {
            std::lock_guard<std::mutex> lg(tasksMutex);
            if (!tasks.empty())
            {
                std::function<void()> task = tasks.front();
                tasks.pop();
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

    void joinThreads()
    {
        for (uint8_t i = 0; i < threads.size(); i++)
        {
            threads.front().join();
            threads.pop();
        }
    }
};