//
// Created by Administrator on 2025/10/12.
//

#include "MyThreadPool.h"
#include <iostream>
using namespace std;
MyThreadPool::MyThreadPool(int size) {
    m_idleThreads = m_curThreads = size;
    m_manager = new thread(&MyThreadPool::manager, this);
    for (int i = 0; i < size; ++i) {
        thread t(&MyThreadPool::worker, this);
        m_workers.insert(make_pair(t.get_id(), std::move(t)));
    }
}

void MyThreadPool::addTask(std::function<void()> f) {
    {
        unique_lock<mutex> locker(m_queueMutex);
        m_tasks.push(f);
    }
    m_condition.notify_one();
}

void MyThreadPool::worker() {
    while (!m_stop.load()) {
        function<void()> task;
        {
            unique_lock<mutex> locker(m_queueMutex);
            while (!m_stop.load() && m_tasks.empty()) {
                m_condition.wait(locker);
                if (m_exitNumber > 0) {
                    m_exitNumber--;
                    m_curThreads--;
                    m_idleThreads--;
                    cout<<"delete a thread : "<< this_thread::get_id()<<endl;
                    unique_lock<mutex> t(m_idsMutex);
                    m_ids.emplace_back(this_thread::get_id());
                }
            }
            if (!m_tasks.empty()) {
                task = std::move(m_tasks.front());
                m_tasks.pop();
            }
        }

        if (task) {
            m_idleThreads--;
            task();
            m_idleThreads++;
        }
    }
}

void MyThreadPool::manager() {
    while (!m_stop.load()) {
        this_thread::sleep_for(chrono::seconds(2));
        int cur = m_curThreads.load();
        int idle = m_idleThreads.load();
        if (idle > cur / 2 && cur > m_minThreads) {
            m_exitNumber.store(2);
            m_condition.notify_all(); // 为什么通知全部
            unique_lock<mutex> locker(m_idsMutex);
            for (const auto& id : m_ids) {
                auto it = m_workers.find(id);
                if (it != m_workers.end()) {
                    cout << "############## 线程 " << (*it).first << "被销毁了...." << endl;
                    (*it).second.join();
                    m_workers.erase(id);
                }
            }
            m_ids.clear();
        }
        else if (idle == 0 && cur < m_maxThreads) {
            thread t(&MyThreadPool::worker, this);
            cout << "+++++++++++++++ 添加了一个线程, id: " << t.get_id() << endl;
            m_workers.insert(make_pair(t.get_id(), move(t)));
            m_idleThreads++;
            m_curThreads++;
        }
    }
}

MyThreadPool::~MyThreadPool() {
    m_stop = true;
    m_condition.notify_all();
    for (auto& it : m_workers) {
        auto& task = it.second;
        if (task.joinable()) {
            task.join();
        }
    }
    if (m_manager->joinable()) {
        m_manager->join();
    }
    delete m_manager;
}

