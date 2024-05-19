#pragma once

#include "my_thread.h"
#include "avpacket_queue.h"
#include "avframe_queue.h"

class DecodeThread : public MyThread
{
public:
    DecodeThread(AVPacketQueue* packet_queue, AVFrameQueue* frame_queue);
    ~DecodeThread();
    int Init(AVCodecParameters* par);
    int Start();
    int Stop();
    void Run();
private:
    char m_error[256] = { 0 };
    AVCodecContext* m_codec_context = NULL;
    AVPacketQueue* m_packet_queue = NULL;
    AVFrameQueue* m_frame_queue = NULL;
};

