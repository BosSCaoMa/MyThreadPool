//
// Created by Administrator on 2025/10/12.
//

#ifndef MYTHREADPOOL_MYTHREADPOOL_H
#define MYTHREADPOOL_MYTHREADPOOL_H

#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <map>
#include <future>

class MyThreadPool {
public:
    MyThreadPool(int size = 4);
    ~MyThreadPool();

    template<typename F, typename... Args>
    auto addTask(F&& f, Args&&... args) -> future<typename result_of<F(Args...)>::type>
    {
        using returnType = typename result_of<F(Args...)>::type;
        auto task = make_shared<packaged_task<returnType()>>(
            bind(forward<F>(f), forward<Args>(args)...)
        );
        future<returnType> res = task->get_future();
        {
            unique_lock<mutex> lock(m_queueMutex);
            m_tasks.emplace([task]() { (*task)(); });
        }
        m_condition.notify_one();
        return res;
    }
private:
    void manager();
    void worker();
private:
    std::thread* m_manager;
    std::map<std::thread::id, std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::vector<std::thread::id> m_ids;
    int m_minThreads = 4;
    int m_maxThreads = std::thread::hardware_concurrency();

    std::atomic<int> m_curThreads;
    std::atomic<int> m_idleThreads;
    std::atomic<int> m_exitNumber;
    std::atomic<bool> m_stop;

    std::mutex m_idsMutex;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
};


#endif //MYTHREADPOOL_MYTHREADPOOL_H
