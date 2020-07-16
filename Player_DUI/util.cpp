#include "pch.h"
#include "util.h"



void util::log(std::string str)
{
#ifdef DEBUG

#ifdef WIN32
	OutputDebugStringA(str.c_str());
#else
	std::cout << str << std::endl;
#endif

#endif
}

void util::thread_sleep(int milsec)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milsec));
}

void util::av_packet_releaser(AVPacket* p_packet)
{
// 	static int count = 0;
// 	char buf[32] = {};
// 	sprintf(buf, "count=%d\r\n", count);
// 	util::log(buf);
// 	count++;

	av_packet_unref(p_packet);
	free(p_packet);
	p_packet = NULL;
}

void util::av_frame_releaser(AVFrame* p_frm)
{
	av_frame_unref(p_frm);
	av_frame_free(&p_frm);
}

