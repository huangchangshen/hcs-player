#include "decode_thread.h"

extern const int ERROR_LENGTH;

DecodeThread::DecodeThread(AVPacketQueue* packet_queue, AVFrameQueue* frame_queue)
    : m_packet_queue(packet_queue), m_frame_queue(frame_queue)
{

}

DecodeThread::~DecodeThread()
{
    if (m_thread)
    {
        Stop();
    }

    if (m_codec_context)
    {
        avcodec_close(m_codec_context);
    }
}

int DecodeThread::Init(AVCodecParameters* parameters)
{
    std::cout << "Init" << std::endl;
    if (!parameters)
    {
        std::cerr << "parameters is null ! " << std::endl;
        return -1;
    }
    int ret = 0;
    m_codec_context = avcodec_alloc_context3(NULL);

    std::cout << "avcodec_alloc_context3" << std::endl;

    ret = avcodec_parameters_to_context(m_codec_context, parameters);
    if (ret < 0)
    {
        av_strerror(ret, m_error, ERROR_LENGTH);
        std::cerr << "avcodec_parameters_to_context error : " << m_error << std::endl;
        return -1;
    }

    std::cout << "avcodec_parameters_to_context" << std::endl;

    //�ҵ�������
    const AVCodec* codec = avcodec_find_decoder(m_codec_context->codec_id); //���Ӳ������


    if (!codec)
    {
        std::cerr << "avcodec_find_decoder error !" << std::endl;
        return -1;
    }

    std::cout << "avcodec_find_decoder" << std::endl;

    // �򿪽����������ģ� ���󶨽�����
    ret = avcodec_open2(m_codec_context, codec, nullptr);
    if (ret < 0)
    {
        av_strerror(ret, m_error, ERROR_LENGTH);
        std::cerr << "avcodec_open2 error : " << m_error << std::endl;
        return -1;
    }

    std::cout << "Init success !" << std::endl;
    return 0;
}

int DecodeThread::Start()
{
    m_thread = new std::thread(&DecodeThread::Run, this);
    if (!m_thread)
    {
        std::cerr << "Start error !" << std::endl;
        return -1;
    }
    return 0;
}

int DecodeThread::Stop()
{
    return MyThread::Stop();
}

void DecodeThread::Run()
{
    AVFrame* frame = av_frame_alloc();

    while (m_abort != 1)
    {
        if (m_frame_queue->Size() > 100)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); //����һ��
            continue;
        }

        AVPacket* pkt = m_packet_queue->Pop(1);
        if (pkt == nullptr)
        {
            std::cout << "decode_thread pop null !" << std::endl;
            continue;
        }

        int ret = avcodec_send_packet(m_codec_context, pkt);

        if (ret < 0)
        {
            av_strerror(ret, m_error, ERROR_LENGTH);
            std::cerr << "DecodeThread::avcodec_send_packet error :" << m_error << std::endl;
            av_packet_free(&pkt);
            //�ͷ� packet
            continue;
        }

        // �յ���packet

        std::cout << "DecodeThread::frame queue size : " << m_frame_queue->Size() << std::endl;

        av_packet_free(&pkt);

        //��ȡ������frame
        while (true)
        {
            ret = avcodec_receive_frame(m_codec_context, frame);
            //std::cout << "-----------------" << ret << std::endl;
            if (ret == 0)
            {
                m_frame_queue->Push(frame);
                continue;
            }
            else if (ret == AVERROR(EAGAIN))
            {
                av_strerror(ret, m_error, ERROR_LENGTH);
                //std::cerr << "DecodeThread::receive again :" << m_error << std::endl;
                break;
            }
            else if (ret == AVERROR_EOF)
            {
                //std::cout << "DecodeThread::decode finished" << std::endl
                break;
            }
            else
            {
                m_abort = 1;
                av_strerror(ret, m_error, ERROR_LENGTH);
                //std::cerr << "DecodeThread::avcodec_receive_frame error :" << m_error << std::endl;
                break;
            }
        }
    }

}
