#include "player.h"

int Play(const char* str, QWidget * parent, QSplitter * splitter)
{
    AVPacketQueue* audio_packet_queue = new AVPacketQueue();
    AVPacketQueue* video_packet_queue = new AVPacketQueue();

    AVFrameQueue* audio_frame_queue = new AVFrameQueue();
    AVFrameQueue* video_frame_queue = new AVFrameQueue();

    AVSync avsync;
    avsync.InitClock();


    // 1.解复用
    DemuxThread* demux_thread = new DemuxThread(audio_packet_queue, video_packet_queue);
    std::cout << "demux_thread new" << std::endl;
    int ret = demux_thread->Init(str);
    if (ret < 0)
    {
        std::cerr << "Stop demux_thread->Init !" << std::endl;
        return 0;
    }
    std::cout << "demux_thread Init" << std::endl;
    ret = demux_thread->Start();
    if (ret < 0)
    {
        std::cerr << "Stop demux_thread->start !" << std::endl;
        return 0;
    }
    std::cout << "demux_thread Start" << std::endl;

    // 2.解码
    // 2.1 音频解码
    DecodeThread* decode_audio_thread = new DecodeThread(audio_packet_queue, audio_frame_queue);
    std::cout << "decode_audio_thread new" << std::endl;
    ret = decode_audio_thread->Init(demux_thread->AudioCodecParameters());
    if (ret < 0)
    {
        std::cerr << "decode_audio_thread error !" << std::endl;
        return -1;
    }
    std::cout << "decode_audio_thread Init" << std::endl;
    ret = decode_audio_thread->Start();
    if (ret < 0)
    {
        std::cerr << "decode_audio_thread start error !" << std::endl;
        return -1;
    }
    std::cout << "decode_audio_thread Start" << std::endl;


    // 2.2 视频解码
    DecodeThread* decode_video_thread = new DecodeThread(video_packet_queue, video_frame_queue);
    std::cout << "decode_video_thread new" << std::endl;
    ret = decode_video_thread->Init(demux_thread->VideoCodecParameters());
    if (ret < 0)
    {
        std::cerr << "decode_video_thread error !" << std::endl;
        return -1;
    }
    std::cout << "decode_video_thread Init" << std::endl;
    ret = decode_video_thread->Start();
    if (ret < 0)
    {
        std::cerr << "decode_video_thread error !" << std::endl;
        return -1;
    }
    std::cout << "decode_video_thread Start" << std::endl;


    /*

    typedef struct SDL_AudioSpec {
        int freq;                   //采样率
        SDL_AudioFormat format;     // 音频设备
        Uint8 channels;             // 声道数
        Uint8 silence;              // ������仺�����ľ���ֵ
        Uint16 samples;             // 采样数
        Uint16 padding;             // 
        Uint32 size;                // 
        SDL_AudioCallback callback; // 重采样回调
        void *userdata;             // 用户数据
    } SDL_AudioSpec;

*/

// 3.Init audio params
    AudioParams audio_params = { 0 };
    memset(&audio_params, 0, sizeof(AudioParams));
    audio_params.channels = demux_thread->AudioCodecParameters()->channels;
    audio_params.channel_layout = demux_thread->AudioCodecParameters()->channel_layout;
    audio_params.fmt = (enum AVSampleFormat)demux_thread->AudioCodecParameters()->format;
    audio_params.freq = demux_thread->AudioCodecParameters()->sample_rate;
    audio_params.frame_size = demux_thread->AudioCodecParameters()->frame_size;

    AudioOutput* audio_output = new AudioOutput(&avsync, demux_thread->AudioStreamTimebase(), audio_params, audio_frame_queue);
    ret = audio_output->Init();     // 初始化音频输出
    if (ret < 0)
    {
        std::cerr << "audio_output->Init error ! " << std::endl;
        return -1;
    }

    //std::this_thread::sleep_for(std::chrono::seconds(120));
    
    //4.初始化视频输出
    int width = demux_thread->VideoCodecParameters()->width;
    int height = demux_thread->VideoCodecParameters()->height;
    
    //double rate = 1000 / height;
    //height = 1000; 
    //width = width * rate;

   /* VideoOutput* video_output = new VideoOutput(parent, &avsync, demux_thread->VideoStreamTimebase(), video_frame_queue,
        demux_thread->VideoCodecParameters()->width, demux_thread->VideoCodecParameters()->height);*/

    VideoOutput* video_output = new VideoOutput(parent, &avsync, demux_thread->VideoStreamTimebase(), video_frame_queue,
      width , height);

    splitter->addWidget(video_output);
    //splitter->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    ret = video_output->Init();
    if (ret < 0)
    {
        std::cerr << "video_output->Init error ! " << std::endl;
        return -1;
    }
    // 循环刷新
    video_output->MainLoop();

    // 释放资源
    demux_thread->Stop();
    if (demux_thread)
    {
        delete demux_thread;
        demux_thread = nullptr;
    }

    decode_audio_thread->Stop();
    if (decode_audio_thread)
    {
        delete decode_audio_thread;
        decode_audio_thread = nullptr;
    }

    decode_video_thread->Stop();
    if (decode_video_thread)
    {
        delete decode_video_thread;
        decode_video_thread = nullptr;
    }

    audio_output->DeInit();
    if (audio_output)
    {
        delete audio_output;
        audio_output = nullptr;
    }

    
    if (video_output)
    {
        delete video_output;
        video_output = nullptr;
    }


    return 0;
}
