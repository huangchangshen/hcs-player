#pragma once
#include "player.h"
#include "avframe_queue.h"
#include "avsync.h"
#include "video_output.h"
#include "MyWidget.h"


class VideoWidget : public QWidget 
{
public:
    VideoWidget(QWidget* parent = nullptr) : QWidget(parent) 
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            qDebug() << "SDL initialization failed: " << SDL_GetError();
            return;
        }

        // 初始化SDL视频播放等操作

        // 如果需要传递窗口ID给SDL，可以使用winId()函数来获取窗口ID
        SDL_Window* sdlWindow = SDL_CreateWindowFrom((void*)winId());
        if (!sdlWindow) 
        {
            qDebug() << "Failed to create SDL window: " << SDL_GetError();
            return;
        }

        // 调用SDL播放视频的函数
        playVideo();
    }

    ~VideoWidget() 
    {
        // 清理SDL资源
        SDL_Quit();
    }

private:
    void playVideo() {
        // 调用SDL播放视频的函数
        // 例如：SDL_RenderVideo();
    }
};