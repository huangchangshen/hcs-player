#pragma once
#include "demux_thread.h"
#include "decode_thread.h"
#include "audio_output.h"
#include "video_output.h"
#include "avsync.h"

int Play(const char* str, QWidget* parent, QSplitter * splitter);