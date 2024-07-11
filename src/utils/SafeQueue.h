#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>

template<typename T>
class SafeQueue
{
private:
    std::queue<T> queue;
    mutable std::mutex queueMutex;
    std::condition_variable cvQueue;
    std::atomic<bool> enabled = true;

public:
    SafeQueue() = default;
    SafeQueue(SafeQueue<T>& safeQueue) = delete;
    SafeQueue& operator=(SafeQueue<T>& safeQueue) = delete;
    ~SafeQueue() {};

    void enqueue(T&& item)
    {
        std::unique_lock<std::mutex> lg(queueMutex);
        queue.push(item);
        cvQueue.notify_one();
    }

    std::optional<T> dequeue()
    {
        std::unique_lock<std::mutex> ul(queueMutex);

        while (enabled && queue.empty())
        {
            cvQueue.wait(ul);
        }

        if (!enabled && queue.empty())
            return std::nullopt;
        
        T temp = queue.front();
        queue.pop();
        return std::optional(std::move(temp));
    }

    [[nodiscard]] bool isEmpty()
    {
        std::unique_lock<std::mutex> ul(queueMutex);
        return queue.empty();
    }

    [[nodiscard]] unsigned int size() const
    {
        std::unique_lock<std::mutex> ul(queueMutex);
        return queue.size();
    }

    void close()
    {
        enabled = false;
        cvQueue.notify_all();
    }
};
