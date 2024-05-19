#pragma once
#include <thread>

const int ERROR_LENGTH = 1024;

class MyThread
{
public:
    MyThread() {}
    ~MyThread() 
    {
        if(m_thread) 
        {
            MyThread::Stop();
        }
    }
    int Start() {}
    int Stop() 
    {
        m_abort = 1;
        if(m_thread) 
        {
            m_thread->join();
            delete m_thread;
            m_thread = NULL;
        }
        return 0;
    }
    virtual void Run() = 0;
protected:
    int m_abort = 0;
    std::thread *m_thread = NULL;
};

