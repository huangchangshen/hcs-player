#pragma once
#ifdef __cplusplus  ///
extern "C"
{
// 包含ffmpeg头文件
//#include "libavutil/avutil.h"
#include "SDL.h"
#include "libswresample/swresample.h"
}
#endif

#include "avsync.h"
#include "avframe_queue.h"
typedef struct AudioParams
{
    int freq; //采样率
    int channels;
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
}AudioParams;


class AudioOutput
{
public:
    AudioOutput(AVSync *avsync, AVRational time_base, const AudioParams &audio_params, AVFrameQueue *frame_queue);
    ~AudioOutput();
    int Init();
    int DeInit();

public:
    int64_t m_pts = AV_NOPTS_VALUE;
    AudioParams m_src_tgt; // 解码后的参数
    AudioParams m_dst_tgt; // SDL实际输出的格式
    AVFrameQueue * m_frame_queue = NULL;

    struct SwrContext * m_swr_ctx = NULL;

    uint8_t * m_audio_buf = NULL;
    uint8_t * m_audio_buf1 = NULL;
    uint32_t m_audio_buf_size = 0;
    uint32_t m_audio_buf1_size = 0;
    uint32_t m_audio_buf_index = 0;

    AVSync * m_avsync = NULL;
    AVRational m_time_base;
};
