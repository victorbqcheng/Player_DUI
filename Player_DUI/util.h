#pragma once
#include "pch.h"
#include <map>
#include <string>

using METADATA_P = std::map<std::string, std::string>;
class util
{
public:
	static void log(std::string const& str);
	static void av_packet_releaser(AVPacket* p_packet);
	static void av_frame_releaser(AVFrame* p_frm);
	static void av_dict_2_map(AVDictionary* dict, METADATA_P& m);
};
#define SAFE_CONTINEU(ret)\
	if (ret != 0)\
	{\
		return -1;\
	}
