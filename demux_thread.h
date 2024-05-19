#pragma once

#include "my_thread.h"
#include "avpacket_queue.h"
#ifdef __cplusplus  ///
extern "C"
{
// 包含ffmpeg头文件
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
}
#endif


class DemuxThread : public MyThread
{
public:
    DemuxThread(AVPacketQueue *audio_queue, AVPacketQueue *video_queue);
    ~DemuxThread();
    int Init(const char *url);
    int Start();
    int Stop();
    void Run();

    AVCodecParameters *AudioCodecParameters(); // 获取音频编码参数
    AVCodecParameters *VideoCodecParameters(); // 获取视频编码参数  查找对应的解码器

    AVRational AudioStreamTimebase();  // 音视频同步
    AVRational VideoStreamTimebase();

private:
    char m_error[256] = {0};
    std::string m_url;// 文件名
    AVPacketQueue * m_audio_queue = NULL;
    AVPacketQueue * m_video_queue = NULL;

    AVFormatContext * m_ifmt_context = NULL;
    int m_AudioIndex = -1;
    int m_VideoIndex = -1;

};

