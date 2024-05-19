#include "avframe_queue.h"

AVFrameQueue::AVFrameQueue()
{
}

AVFrameQueue::~AVFrameQueue()
{
}

void AVFrameQueue::Abort()
{
    release();
    m_AVF_Queue.Abort();
}

int AVFrameQueue::Size()
{
    return m_AVF_Queue.Size();
}

int AVFrameQueue::Push(AVFrame* frame)
{
    AVFrame* temp_frame = av_frame_alloc();
    av_frame_move_ref(temp_frame, frame);
    int ret = m_AVF_Queue.Push(temp_frame);
    if (ret < 0)
    {
        std::cerr << "m_AVFQueue.Push error !" << std::endl;
        return ret;
    }
    return 0;
}

AVFrame* AVFrameQueue::Pop(const int& timeout)
{
    AVFrame* temp_frame = nullptr;
    int ret = m_AVF_Queue.Pop(temp_frame, timeout);
    if (ret < 0)
    {
        std::cerr << "m_AVFQueue.Pop error !" << ret << std::endl;
        return temp_frame;
    }
    return temp_frame;
}

AVFrame* AVFrameQueue::Front()
{
    AVFrame* frame = nullptr;
    int ret = m_AVF_Queue.Front(frame);
    if (ret < 0)
    {
        std::cerr << "m_AVFQueue.Front error !" << std::endl;
        return nullptr;
    }
    return frame;
}

void AVFrameQueue::release()
{
    while (true)
    {
        AVFrame* frame = nullptr;
        int ret = m_AVF_Queue.Pop(frame, 1);
        if (ret < 0)
        {
            break;
        }
        av_frame_free(&frame);
    }
}
