#pragma once

#ifdef __cplusplus  ///
extern "C"
{
// 包含ffmpeg头文件
//#include "libavutil/avutil.h"
#include "SDL.h"
#include "libavutil/time.h"
}
#endif

#include "MyWidget.h"
#include "avframe_queue.h"
#include "avsync.h"


class VideoOutput : public QWidget
{
public:
    VideoOutput(QWidget* parent, AVSync *avsync, AVRational time_base, AVFrameQueue *frame_queue, int video_width, int video_height);
    ~VideoOutput();

    int Init();
    int MainLoop();
    void RefreshLoopWaitEvent(SDL_Event *event);
    
private:
    void videoRefresh(double *remaining_time);
    AVFrameQueue * m_frame_queue = NULL;
    SDL_Event m_event; // 事件
    SDL_Rect m_rect;    
    SDL_Window * m_win = NULL;  // 窗口
    SDL_Renderer * m_renderer = NULL;   //渲染器
    SDL_Texture * m_texture = NULL;     //纹理

    AVSync * m_avsync = NULL;
    AVRational m_time_base;         //时间基

    int m_video_width = 0;
    int m_video_height = 0;
    uint8_t * m_yuv_buf = NULL;
    int      m_yuv_buf_size = 0;
};

