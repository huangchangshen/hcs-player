#include "video_output.h"

VideoOutput::VideoOutput(QWidget* parent, AVSync* avsync, AVRational time_base, AVFrameQueue* frame_queue, int video_width, int video_height)
	: QWidget(parent), m_avsync(avsync), m_time_base(time_base),
	m_frame_queue(frame_queue), m_video_width(video_width), m_video_height(video_height)
{

}

VideoOutput::~VideoOutput() 
{
	SDL_Quit();
	m_frame_queue->Abort();
}

int VideoOutput::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL_INIT_VIDEO error !" << std::endl;
		return -1;
	}

	//m_win = SDL_CreateWindow("hcs_player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	//	m_video_width, m_video_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);	

	m_win = SDL_CreateWindowFrom((void*)winId());

	if (!m_win)
	{
		std::cerr << "SDL_CreateWindowFrom error !" << std::endl;
		goto faild;
	}

	m_renderer = SDL_CreateRenderer(m_win, -1, 0);	//	渲染器
	if (!m_renderer)
	{
		std::cerr << "SDL_CreateRenderer error !" << std::endl;
		goto faild;
	}

	// ------------------------------------------------------------------------------------------------------------

	m_sdlSurface = SDL_CreateRGBSurface(0, 1600, 1000, 32, 0, 0, 0, 0);	//创造表面， 用于缩放视频

	// -------------------------------------------------------------------------------------------------------------

	m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING,
		m_video_width, m_video_height);	// 纹理

	if (!m_texture)
	{
		std::cerr << "SDL_CreateTexture error !" << std::endl;
		goto faild;
	}

	m_yuv_buf_size = m_video_width * m_video_height * 1.5;	// yuv420 
	m_yuv_buf = (uint8_t*)malloc(m_yuv_buf_size);
	return 0;

faild:	
	//std::cout << "error ------------------------------------------------" << std::endl;
	if (m_win) SDL_DestroyWindow(m_win);
	if (m_renderer) SDL_DestroyRenderer(m_renderer);
	if (m_texture) SDL_DestroyTexture(m_texture);
	return -1;
}

int VideoOutput::MainLoop()
{
	SDL_Event event;
	while (true)
	{
		RefreshLoopWaitEvent(&event);

		switch (event.type)
		{
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				std::cout << "esc key down !" << std::endl;
				return 0;
			}
			break;
		case SDL_QUIT:
			SDL_DestroyRenderer(m_renderer);
			SDL_DestroyWindow(m_win);
			SDL_QuitSubSystem(SDL_INIT_VIDEO);
			std::cout << "SDL_QUIT !" << std::endl;
			return -1;
			break;
		default:
			break;
		}
	}
	return 0;
}


#define REFRESH_RATE 0.01
void VideoOutput::RefreshLoopWaitEvent(SDL_Event* event)
{
	double remain_time = 0.0;
	SDL_PumpEvents();
	while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT))
	{
		if (remain_time > 0.0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((int64_t)(remain_time * 1000.0)));
		}
		remain_time = REFRESH_RATE;

		videoRefresh(&remain_time);
		SDL_PumpEvents();
	}
}

void VideoOutput::videoRefresh(double* remaining_time)
{
	AVFrame* frame = nullptr;
	frame = m_frame_queue->Front();	
	if (!frame)
	{
		std::cout << "not frame will be freshed " << std::endl;
		return;
	}

	double pts = frame->pts * av_q2d(m_time_base);	
	double diff = pts - m_avsync->GetClock();

	if (diff > 0)
	{
		*remaining_time = FFMIN(*remaining_time, diff);
		return;
	}

	m_rect.x = 0;
	m_rect.y = 0;
	m_rect.w = m_video_width;
	m_rect.h = m_video_height;

	SDL_UpdateYUVTexture(m_texture, &m_rect, frame->data[0], frame->linesize[0],
		frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2]);

	SDL_RenderClear(m_renderer);
	SDL_RenderCopy(m_renderer, m_texture, NULL, &m_rect);
	SDL_RenderPresent(m_renderer);

	frame = m_frame_queue->Pop(1);
	//if (!frame)
	//{
	//	std::cerr << " ERROR !!!!!!!!!!!!!!!!" << std::endl;
	//}
	av_frame_free(&frame);
}