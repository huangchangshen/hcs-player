#include "audio_output.h"

AudioOutput::AudioOutput(AVSync* avsync, AVRational time_base, const AudioParams& audio_params, AVFrameQueue* frame_queue)
    :m_avsync(avsync), m_time_base(time_base), m_src_tgt(audio_params), m_frame_queue(frame_queue)
{

}

AudioOutput::~AudioOutput()
{
    
}
FILE* dump_pcm = NULL; // һ֡��ƵPCM���� ��С��ռ���ֽڣ� �����Ŷ೤ʱ��
void AudioCallBack(void* udata, Uint8* stream, int len) {

    // ��frame queue��ȡ������PCM�����ݣ���䵽stream
    AudioOutput* is = (AudioOutput*)udata;
    int len1 = 0;
    int audio_size = 0;

    if (!dump_pcm) 
    {
        dump_pcm = fopen("dump.pcm", "wb");
    }

    while (len > 0)
    {
        if (is->m_audio_buf_index == is->m_audio_buf_size)
        {
            is->m_audio_buf_index = 0;
            AVFrame* frame = is->m_frame_queue->Pop(10);
            if (frame)
            {
                is->m_pts = frame->pts;
                // ��������������
                // ��ô�ж�Ҫ��Ҫ���ز���
                if (((frame->format != is->m_dst_tgt.fmt) || (frame->sample_rate != is->m_dst_tgt.freq)
                    || (frame->channel_layout != is->m_dst_tgt.channel_layout)) && (!is->m_swr_ctx)) 
                {
                    is->m_swr_ctx = swr_alloc_set_opts(NULL, is->m_dst_tgt.channel_layout,
                        (enum AVSampleFormat)is->m_dst_tgt.fmt,is->m_dst_tgt.freq,
                        frame->channel_layout,(enum AVSampleFormat)frame->format, frame->sample_rate, 0, NULL);

                    if (!is->m_swr_ctx || swr_init(is->m_swr_ctx) < 0) 
                    {
                        std::cerr << "audio_output:: !is->m_swr_ctx || swr_init(is->m_swr_ctx) < 0" << std::endl;
                        swr_free((SwrContext**)(&is->m_swr_ctx));
                        return;
                    }
                }
                if (is->m_swr_ctx) 
                { // �ز���
                    const uint8_t** in = (const uint8_t**)frame->extended_data;
                    uint8_t** out = &is->m_audio_buf1;
                    int out_samples = frame->nb_samples * is->m_dst_tgt.freq / frame->sample_rate + 256;
                    int out_bytes = av_samples_get_buffer_size(NULL, is->m_dst_tgt.channels, out_samples, is->m_dst_tgt.fmt, 0);
                    if (out_bytes < 0) 
                    {
                        std::cerr << "audio_output:: av_samples_get_buffer_size error " << std::endl;
                        return;
                    }
                    av_fast_malloc(&is->m_audio_buf1, &is->m_audio_buf1_size, out_bytes);

                    int len2 = swr_convert(is->m_swr_ctx, out, out_samples, in, frame->nb_samples); // ������������
                    if (len2 < 0) 
                    {
                        std::cerr << "audio_output:: swr_convert error " << std::endl;
                        return;
                    }
                    is->m_audio_buf = is->m_audio_buf1;
                    is->m_audio_buf_size = av_samples_get_buffer_size(NULL, is->m_dst_tgt.channels, len2, is->m_dst_tgt.fmt, 1);
                }
                else 
                { // û���ز���
                    audio_size = av_samples_get_buffer_size(NULL, frame->channels, frame->nb_samples, (enum AVSampleFormat)frame->format, 1);
                    av_fast_malloc(&is->m_audio_buf1, &is->m_audio_buf1_size, audio_size);
                    is->m_audio_buf = is->m_audio_buf1;
                    is->m_audio_buf_size = audio_size;
                    memcpy(is->m_audio_buf, frame->data[0], audio_size);
                }
                av_frame_free(&frame);
            }
            else
            {
                std::cout << "ERROR !!!!!!!!!!!!!!!" << std::endl;
                // û�ж�������������
                is->m_audio_buf = NULL;
                is->m_audio_buf_size = 512;
            }
        }
        len1 = is->m_audio_buf_size - is->m_audio_buf_index;
        if (len1 > len)
            len1 = len;
        if (!is->m_audio_buf) 
        {
            memset(stream, 0, len1);
        }
        else {
            // ����������Ч������
            memcpy(stream, is->m_audio_buf + is->m_audio_buf_index, len1);
            fwrite((uint8_t*)is->m_audio_buf + is->m_audio_buf_index, 1, len1, dump_pcm);
            fflush(dump_pcm);
        }
        len -= len1;
        stream += len1;
        is->m_audio_buf_index += len1;
    }

    // ����ʱ��
    if (is->m_pts != AV_NOPTS_VALUE)
    {
        double pts = is->m_pts * av_q2d(is->m_time_base);
        //        LogInfo("audio pts:%0.3lf\n", pts);
        is->m_avsync->SetClock(pts);
    }
}

int AudioOutput::Init()
{
    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        std::cerr << "audio_output:: SDL_Init(SDL_INIT_AUDIO) error " << std::endl;
        return -1;
    }

    SDL_AudioSpec wanted_spec;
    wanted_spec.channels = 2;// ֻ֧��2channel�����
    wanted_spec.freq = m_src_tgt.freq;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.silence = 0;
    wanted_spec.callback = AudioCallBack;
    wanted_spec.userdata = this;
    wanted_spec.samples = 1024; // ��������

    int ret = SDL_OpenAudio(&wanted_spec, NULL);  
    if (ret != 0) 
    {
        std::cerr << "audio_output:: SDL_OpenAudio error " << std::endl;
        return -1;
    }

    m_dst_tgt.channels = wanted_spec.channels; //spec.channels;
    m_dst_tgt.fmt = AV_SAMPLE_FMT_S16;
    m_dst_tgt.freq = wanted_spec.freq;// spec.freq;
    m_dst_tgt.channel_layout = av_get_default_channel_layout(2);
    m_dst_tgt.frame_size = 1024;//m_src_tgt.frame_size;
    SDL_PauseAudio(0);
}

int AudioOutput::DeInit()
{
    SDL_PauseAudio(1);
    SDL_CloseAudio();
    return 0;
}