#pragma once
#include "pch.h"
#include <string>

class util
{
public:
	static void log(std::string const& str);
	static void av_packet_releaser(AVPacket* p_packet);
	static void av_frame_releaser(AVFrame* p_frm);
};

