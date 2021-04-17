#pragma once

#include <atomic>
#include <functional>
#include <thread>
#include <vector>
#include "ThreadsafeQueue.h"

class ThreadPool
{
public:
    ThreadPool() = default;
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    void AddTask(std::function<void()> task);

private:
    void Worker() noexcept;

private:
    std::atomic_bool m_done{ false };
    ThreadsafeQueue<std::function<void()>> m_queue;
    std::vector<std::thread> m_threads;
};
