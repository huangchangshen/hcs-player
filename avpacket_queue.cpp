#include "avpacket_queue.h"

AVPacketQueue::AVPacketQueue()
{

}

AVPacketQueue::~AVPacketQueue()
{

}

void AVPacketQueue::Abort()
{
    release();
    m_AVP_Queue.Abort();
}


int AVPacketQueue::Size()
{
    return  m_AVP_Queue.Size();
}


int AVPacketQueue::Push(AVPacket* val)
{
    AVPacket* tmp_pkt = av_packet_alloc(); //av_packet_free
    av_packet_move_ref(tmp_pkt, val); // ����c++����ָ���mov����   ���ڴ����ü���
    return m_AVP_Queue.Push(tmp_pkt);
}

AVPacket* AVPacketQueue::Pop(const int timeout)
{
    AVPacket* tmp_pkt = NULL;
    int ret = m_AVP_Queue.Pop(tmp_pkt, timeout);
    if (ret < 0) 
    {
        std::cerr << "AVPacketQueue:: Pop error !" << std::endl;
        return NULL;
    }
    return tmp_pkt;
}

void AVPacketQueue::release()
{
    while (true) 
    {
        AVPacket* packet = NULL;
        int ret = m_AVP_Queue.Pop(packet, 1);
        if (ret < 0) 
            break;
        av_packet_free(&packet);
    }
}
