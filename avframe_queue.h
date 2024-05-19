#pragma once
#include "my_queue.h"

#ifdef __cplusplus  ///
extern "C"
{
#include "libavcodec/avcodec.h"
}
#endif

class AVFrameQueue
{
public:
    AVFrameQueue();
    ~AVFrameQueue();
    void Abort();
    int Push(AVFrame *val);
    AVFrame *Pop(const int& timeout);
    AVFrame *Front();
    int Size();
private:
    void release();
    MyQueue<AVFrame *> m_AVF_Queue;
};