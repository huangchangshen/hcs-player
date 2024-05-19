#pragma once
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>

template <typename T>
class MyQueue
{
public:
    MyQueue() {}
    ~MyQueue() {}
    void Abort() {
        m_abort = 1;
        m_condition_variable.notify_all();
    }
    // 放入数据
    int Push(T val) 
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(1 == m_abort) 
        {
            return -1;
        }
        m_queue.push(val);
        m_condition_variable.notify_one();
        return 0;
    }
    // 读取数据
    int Pop(T &val, const int timeout = 0) 
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_queue.empty()) 
        {
            // 等待push或者超时唤醒
            m_condition_variable.wait_for(lock, std::chrono::milliseconds(timeout), [this] {
                return !m_queue.empty() | m_abort;
            });
        }
        if(1 == m_abort) 
        {
            return -1;
        }
        else if(m_queue.empty()) 
        {
            return -2;
        }
        val = m_queue.front();
        m_queue.pop();
        return 0;
    }

    int Front(T &val) 
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(1 == m_abort) 
        {
            return -1;
        }
        if(m_queue.empty()) 
        {
            return -2;
        }
        val = m_queue.front();
        return 0;
    }

    int Size() 
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }


private:
    int m_abort = 0;
    std::mutex m_mutex;
    std::condition_variable m_condition_variable;
    std::queue<T> m_queue;
};

