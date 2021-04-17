#include <system_error>
#include "ThreadPool.h"

ThreadPool::~ThreadPool()
{
    m_done = true;

    for (auto& t : m_threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}

void ThreadPool::AddTask(std::function<void()> task)
{
    if (m_threads.size() < std::thread::hardware_concurrency())
    {
        try
        {
            m_threads.emplace_back(std::thread(&ThreadPool::Worker, this));
        }
        catch (const std::system_error&)
        {
            m_done = true;
            throw;
        }
    }

    m_queue.Enqueue(std::function<void()>(task));
}

void ThreadPool::Worker() noexcept
{
    while (!m_done)
    {
        std::function<void()> task;
        if (m_queue.Front(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }
}
