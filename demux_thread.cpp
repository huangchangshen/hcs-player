#include "demux_thread.h"

extern const int ERROR_LENGTH;

DemuxThread::DemuxThread(AVPacketQueue* audio_queue, AVPacketQueue* video_queue)
    : m_audio_queue(audio_queue), m_video_queue(video_queue)
{
    std::cout << "DemuxThread constructed " << std::endl;
}

DemuxThread::~DemuxThread()
{
    std::cout << "DemuxThread destructed " << std::endl;
    if (m_thread)
        Stop();
}

AVCodecParameters* DemuxThread::AudioCodecParameters()
{
    if (m_AudioIndex != -1)
    {
        return m_ifmt_context->streams[m_AudioIndex]->codecpar;
    }
    return nullptr;
}

AVCodecParameters* DemuxThread::VideoCodecParameters()
{
    if (m_VideoIndex != -1)
    {
        return m_ifmt_context->streams[m_VideoIndex]->codecpar;
    }
    return nullptr;
}

AVRational DemuxThread::AudioStreamTimebase()
{
    if (m_AudioIndex != -1)
    {
        return m_ifmt_context->streams[m_AudioIndex]->time_base;
    }
    return AVRational{ 0, 0 };
}

AVRational DemuxThread::VideoStreamTimebase()
{
    if (m_VideoIndex != -1)
    {
        return m_ifmt_context->streams[m_VideoIndex]->time_base;
    }
    return AVRational{ 0, 0 };
}

int DemuxThread::Init(const char* url)
{
    std::cout << "DemuxThread Initted " << url << std::endl;
    int ret = 0;
    m_url = std::string("./video/") + url;
    m_ifmt_context = avformat_alloc_context();  //����������
    std::cout << m_url.c_str() << std::endl;
    ret = avformat_open_input(&m_ifmt_context, m_url.c_str(), NULL, NULL);  //����url���ļ���
    if (ret < 0)
    {
        av_strerror(ret, m_error, ERROR_LENGTH);
        std::cerr << "DemuxThread:: avformat_open_input error : " << m_error << std::endl;
        return -1;
    }

    ret = avformat_find_stream_info(m_ifmt_context, nullptr);   // ��ȡ������Ϣ
    if (ret < 0)
    {
        av_strerror(ret, m_error, ERROR_LENGTH);
        std::cerr << "DemuxThread::avformat_find_stream_info error : " << m_error << std::endl;
        return -1;
    }

    av_dump_format(m_ifmt_context, 0, url, 0);  //��ӡ��Ϣ

    // ��ȡ stream index
    m_AudioIndex = av_find_best_stream(m_ifmt_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    m_VideoIndex = av_find_best_stream(m_ifmt_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    std::cout << "AudioIndex is : " << m_AudioIndex << " VideoIndex is : " << m_VideoIndex << std::endl;
    if (m_AudioIndex < 0 || m_VideoIndex < 0)
    {
        std::cerr << "DemuxThread::AudioIndex error or VideoIndex error !" << std::endl;
        return -1;
    }
    return 0;
}

int DemuxThread::Start()
{
    m_thread = new std::thread(&DemuxThread::Run, this);
    if (m_thread == nullptr)
    {
        std::cerr << "DemuxThread::thread create failed !" << std::endl;
        return -1;
    }
    return 0;
}

int DemuxThread::Stop()
{
    MyThread::Stop();   //�����߳���Դ
    avformat_close_input(&m_ifmt_context);
    return 0;
}

//
void DemuxThread::Run()
{
    int ret = 0;
    AVPacket apk;
    std::cout << "Run  !!!!!!!!!!!!!!!" << std::endl;
    std::cout << m_abort << std::endl;

    while (m_abort != 1)
    {
        //控制队列占内存大小
        if (m_audio_queue->Size() > 100 || m_video_queue->Size() > 100)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); //睡眠一秒
            continue;
        }

        ret = av_read_frame(m_ifmt_context, &apk);
        if (ret < 0)
        {
            std::cerr << "av_read_frame error !" << std::endl;
            break;
            //continue;
        }

        if (apk.stream_index == m_AudioIndex)
        {
            m_audio_queue->Push(&apk);
            //av_packet_unref(&apk);
            std::cout << "DemuxThread::AudioQueue size: " << m_audio_queue->Size() << std::endl;
        }
        else if (apk.stream_index == m_VideoIndex)
        {
            m_video_queue->Push(&apk);
            //av_packet_unref(&apk);
            std::cout << "DemuxThread::VideoQueue size: " << m_video_queue->Size() << std::endl;
        }
        else
        {
            av_packet_unref(&apk);
        }
    }
    std::cout << "Run finished !" << std::endl;
}
