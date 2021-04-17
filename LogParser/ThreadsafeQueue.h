#pragma once

#include <mutex>
#include <queue>

template<typename T>
class ThreadsafeQueue
{
public:
    ThreadsafeQueue() = default;
    ~ThreadsafeQueue() = default;

    ThreadsafeQueue(const ThreadsafeQueue&) = delete;
    ThreadsafeQueue& operator=(const ThreadsafeQueue&) = delete;
    ThreadsafeQueue(ThreadsafeQueue&&) = delete;
    ThreadsafeQueue& operator=(ThreadsafeQueue&&) = delete;

    void Enqueue(T value) noexcept;
    bool Front(T& value) noexcept;

private:
    mutable std::mutex m_mutex;
    std::queue<T> m_queue;
};

template <typename T>
void ThreadsafeQueue<T>::Enqueue(T value) noexcept
{
    std::lock_guard lg{ m_mutex };
    m_queue.push(std::move(value));
}

template <typename T>
bool ThreadsafeQueue<T>::Front(T& value) noexcept
{
    std::unique_lock ul{ m_mutex };
    if (m_queue.empty())
    {
        return false;
    }
    value = std::move(m_queue.front());
    m_queue.pop();
    return true;
}
