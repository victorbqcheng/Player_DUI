#pragma once

#include <string>
#include <thread>
class util
{

public:
	static void log(std::string str);
	static void thread_sleep(int milsec);
	static void av_packet_releaser(AVPacket* p_packet);
	static void av_frame_releaser(AVFrame* p_frm);
};

#define SAFE_DELETE(p)\
if(p!=NULL)\
{\
	delete p;\
	p = NULL;\
}


